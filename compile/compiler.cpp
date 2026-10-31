//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "compiler.h"
#include "errors.h"
#include "release_assert.h"
#include "symtab.h"
#include "types.h"
#include "utils.h"
#include "zx81consts.h"
#include "zx81float.h"
#include <array>
#include <sstream>
#include <string>

Compiler::Compiler(Basic* basic_)
    : basic(basic_) {
}

void Compiler::compile() {
    symtab.init();

    // replace all @line by the line number
    fix_line_refs();

    // remove empty lines with no labels,
    // now that GOTO @line have been resolved
    remove_empty_basic_lines();

    int last_end_addr = 0;
    for (pass = 1; pass <= 2; pass++) {
        compile_prog();

        if (addr != last_end_addr) {	// source size changed
            pass = 0;    // keep on pass 1
        }

        last_end_addr = addr;
    }

    compile_vars();
}

void Compiler::write_p_file(const std::string& filename) {
    mem.write_p_file(filename);
}

void Compiler::fix_line_refs() {
    std::unordered_map<std::string, int> line_map;

    // map label to line number
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            if (!line.label.empty()) {
                release_assert(line.line_num >= 0);
                line_map[line.label] = line.line_num;
            }
        }
    }

    // replace @line by line number
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            for (auto& token : line.tokens) {
                if (token.code == ZX81char::T_line_num_ref) {
                    auto it = line_map.find(token.ident);
                    release_assert(it != line_map.end());
                    token.code = ZX81char::T_integer;
                    token.ivalue = it->second;
                }
            }
        }
    }

    // remove the last line if it has only a label and/or line number and no text
    while (!basic->source_lines.empty() &&
            basic->source_lines.back().type == SourceLine::Type::Basic &&
            basic->source_lines.back().tokens.size() == 1 &&
            basic->source_lines.back().tokens.front().code == C_newline) {
        basic->source_lines.pop_back();
    }
}

// remove empty lines without labels
void Compiler::remove_empty_basic_lines() {
    for (size_t i = 0; i < basic->source_lines.size(); i++) {
        auto& line = basic->source_lines[i];
        if (line.type == SourceLine::Type::Basic &&
                line.label.empty() &&
                line.tokens.size() == 1 &&
                line.tokens.front().code == C_newline) {
            basic->source_lines.erase(basic->source_lines.begin() + i);
            i--;
        }
    }
}

void Compiler::compile_prog() {
    addr = PROG;
    line_addr = 0;
    bool last_basic = false;
    for (auto& line : basic->source_lines) {
        switch (line.type) {
        case SourceLine::Type::Basic:
            if (line_addr > 0 && !last_basic) {	// end last asm statement
                mem.poke(addr++, C_newline);
                end_basic_line();
            }
            start_basic_line(line.line_num);
            compile_basic(line);
            end_basic_line();
            last_basic = true;
            break;

        case SourceLine::Type::Asm:			// concatenate to last basic line
            if (line_addr == 0) {			// create REM line
                start_basic_line(0);
                mem.poke(addr++, C_REM);
            }
            else if (last_basic) {
                addr--;						// overwrite newline
            }
            asmpc = addr;
            assemble(line);
            end_basic_line();
            last_basic = false;
            break;

        default:
            release_assert(0);
        }
    }

    if (line_addr > 0 && !last_basic) {
        mem.poke(addr++, C_newline);
        end_basic_line();
    }

    // video to stkend
    mem.init_video_to_stkend(addr, basic->dfile_collapsed, basic->dfile_lines);

    // E_PPC
    if (addr == PROG) {
        mem.dpoke(E_PPC, 0);
    }
    else {
        mem.dpoke(E_PPC, mem.dpeek_be(PROG));
    }

    // autostart
    if (basic->autostart) {
        int line_addr = mem.get_line_addr(basic->autostart_line);
        mem.dpoke(NXTLIN, line_addr);
    }
    else {
        mem.dpoke(NXTLIN, mem.dpeek(D_FILE));
    }

    // fast
    if (basic->fast) {
        mem.poke(CDFLAG, FlagFast);
    }
    else {
        mem.poke(CDFLAG, FlagSlow);
    }

    addr = mem.dpeek(E_LINE);
}

void Compiler::start_basic_line(int line_num) {
    line_addr = addr;
    mem.dpoke_be(addr, line_num);
    addr += 2;
    mem.dpoke(addr, 0); // placeholder for size
    addr += 2;
}

void Compiler::end_basic_line() {
    mem.dpoke(line_addr + 2, addr - line_addr - 4);		// fill size
    line_addr = 0;
}

void Compiler::compile_basic(SourceLine& line) {
    // define label on possibly multiple passes 1
    if (pass == 1 && !line.label.empty()) {
        symtab.update(Symbol::Type::Addr, line.label, line_addr, line.loc);
    }

    // process each token
    for (auto& token : line.tokens) {
        switch (token.code) {
        case T_none:
            break;
        case T_integer:
            compile_number(token.ivalue, std::to_string(token.ivalue), line.loc);
            break;
        case T_float:
            compile_number(token.fvalue, token.svalue, line.loc);
            break;
        case T_string:
            compile_string(token.svalue, line.loc);
            break;
        case T_ident:
            compile_ident(token.ident, line.loc);
            break;
        case T_rem_code:
            for (auto& c : token.bytes) {
                mem.poke(addr++, c);
            }
            break;
        case T_line_addr_ref:
            compile_line_addr_ref(token.ident, line.loc);
            break;
        case T_line_num_ref:
            release_assert(0);  // already mapped by fix_line_refs()
            break;
        default:
            release_assert(token.code < 0x100);
            mem.poke(addr++, token.code);
        }
    }
}

void Compiler::compile_number(double value, const std::string& value_str,
                              const SourceLoc& loc) {
    std::ostringstream oss;

    // encode digits
    Bytes str_bytes = encode_zx81(value_str, loc);
    addr = mem.poke_bytes(addr, str_bytes);

    // encode number marker
    mem.poke(addr++, C_number);

    // encode fp number
    std::array<Byte, 5> fp_bytes = float_to_zx81(value);
    addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
}

void Compiler::compile_string(const std::string& value, const SourceLoc& loc) {
    Bytes str_bytes = encode_zx81(value, loc);
    mem.poke(addr++, C_dquote);
    for (auto& c : str_bytes) {
        mem.poke(addr++, c);
    }
    mem.poke(addr++, C_dquote);
}

void Compiler::compile_ident(const std::string& ident, const SourceLoc& loc) {
    Bytes str_bytes = encode_zx81(ident, loc);
    for (auto& c : str_bytes) {
        mem.poke(addr++, c);
    }
}

void Compiler::compile_line_addr_ref(const std::string& ident,
                                     const SourceLoc& loc) {
    auto symbol = symtab.get(ident);
    if (!symbol && pass == 2) {
        error(loc, "undefined symbol:" + ident);
    }
    if (symbol) {
        compile_number(symbol->value, std::to_string(symbol->value), loc);
    }
    else {
        compile_number(0, "0", loc);
    }
}

void Compiler::compile_vars() {
    addr = mem.dpeek(VARS);

    Bytes str_bytes;
    std::array<Byte, 5> fp_bytes{ 0 };
    int last_dim = 0;
    int size = 0;

    for (auto& var : basic->basic_vars) {
        Bytes name_bytes = encode_zx81(var.name, var.loc);
        release_assert(name_bytes.size() > 0);

        switch (var.type) {
        case BasicVar::Type::Number:
            // put name
            if (var.name.size() == 1) {
                mem.poke(addr++, (name_bytes[0] & 0x3f) | 0x60);			// 011-letter
            }
            else {
                mem.poke(addr++, (name_bytes[0] & 0x3f) | 0xa0);			// 101-letter
                for (size_t i = 1; i < name_bytes.size() - 1; i++) {
                    mem.poke(addr++, name_bytes[i] & 0x7f);                 // 00-letter
                }
                mem.poke(addr++, name_bytes.back() | 0x80);		            // 10-letter
            }

            // put value
            fp_bytes = float_to_zx81(var.value);
            addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
            break;

        case BasicVar::Type::ArrayNumbers:
            // put name
            mem.poke(addr++, (name_bytes[0] & 0x1f) | 0x80);				// 100-letter

            // put dimensions
            size = 1
                   + 2 * INT(var.dims.size())
                   + 5 * INT(var.values.size());
            mem.dpoke(addr, size);
            addr += 2;
            mem.poke(addr++, var.dims.size() & 0xff);

            for (auto& dim : var.dims) {
                mem.dpoke(addr, dim);
                addr += 2;
            }

            // put values
            for (auto& value : var.values) {
                fp_bytes = float_to_zx81(value);
                addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
            }
            break;

        case BasicVar::Type::ForNextLoop:
            // put name
            mem.poke(addr++, (name_bytes[0] & 0x3f) | 0xe0);				// 111-letter

            // put values
            fp_bytes = float_to_zx81(var.value);
            addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            fp_bytes = float_to_zx81(var.limit);
            addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            fp_bytes = float_to_zx81(var.step);
            addr = mem.poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            mem.dpoke(addr, var.line_num);
            addr += 2;
            break;

        case BasicVar::Type::String:
            // put name
            mem.poke(addr++, (name_bytes[0] & 0x1f) | 0x40);				// 010-letter

            mem.dpoke(addr, INT(var.str.size()));
            addr += 2;

            str_bytes = encode_zx81(var.str, var.loc);
            addr = mem.poke_bytes(addr, str_bytes);
            break;

        case BasicVar::Type::ArrayStrings:
            // put name
            mem.poke(addr++, (name_bytes[0] & 0x1f) | 0xc0);				// 110-letter

            // put dimensions
            last_dim = var.dims.back();
            size = 1
                   + 2 * INT(var.dims.size())
                   + last_dim * INT(var.strs.size());
            mem.dpoke(addr, size);
            addr += 2;
            mem.poke(addr++, INT(var.dims.size()));

            for (auto& dim : var.dims) {
                mem.dpoke(addr, dim);
                addr += 2;
            }

            // put strings
            for (auto& str : var.strs) {
                str_bytes = encode_zx81(str, var.loc);
                addr = mem.poke_bytes(addr, str_bytes);
            }
            break;

        default:
            release_assert(0);
        }
    }

    mem.poke(addr++, 0x80);
    mem.init_e_line_to_stkend(addr);
    addr = mem.dpeek(E_LINE);
}

void Compiler::assemble(SourceLine& line) {
    (void)line;
}
