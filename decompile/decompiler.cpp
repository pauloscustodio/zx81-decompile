//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "basic_info.h"
#include "ctl_file.h"
#include "decompiler.h"
#include "disasm.h"
#include "errors.h"
#include "memory.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81encode.h"
#include <algorithm>
#include <cmath>
#include <consts.h>
#include <string>
#include <utility>
#include <vector>

Decompiler::Decompiler(Memory* mem_, Basic* basic_, CtlFile* ctl_file_,
                       DisasmCode* disasm_)
    : mem(mem_), basic(basic_), ctl_file(ctl_file_), disasm(disasm_) {
}

Decompiler::~Decompiler() {
    delete basic_info;
}

void Decompiler::decompile() {
    release_assert(mem);
    release_assert(basic);
    release_assert(ctl_file);
    release_assert(disasm);

    decompile_sysvars();
    decompile_basic();
    decompile_vars();
    disassemble();
}

BasicInfo* Decompiler::get_basic_info() {
    return basic_info;
}

void Decompiler::decompile_sysvars() {
    basic->video_lines = mem->get_trimmed_video_lines();
    basic->video_collapsed = mem->is_video_collapsed();

    basic->autostart = mem->get_autostart();
    basic->autostart_line_num = mem->get_autostart_line_num();

    basic->workspace_bytes = mem->get_workspace_bytes();

    basic->fast = mem->get_fast();
}

void Decompiler::decompile_basic() {
    basic->source_lines.clear();
    addr = PROG;
    while (addr < mem->dpeek(D_FILE)) {
        SourceLine line(SourceLine::Type::Basic, SourceLoc());
        line.addr = addr;
        line.basic_line.line_num = mem->dpeek_be(addr);
        int size = mem->dpeek(addr + 2);

        addr += 4;
        end = addr + size;

        decompile_basic_line(line.basic_line);
        basic->source_lines.push_back(std::move(line));
    }
}

void Decompiler::decompile_basic_line(BasicLine& basic_line) {
    if (decompile_rem_code(basic_line)) {
    }
    else {
        while (addr < end - 1) {
            if (decompile_number(basic_line)) {
            }
            else if (decompile_ident(basic_line)) {
            }
            else if (decompile_string(basic_line)) {
            }
            else {
                Token token;
                token.code = static_cast<ZX81char>(mem->peek(addr++));
                basic_line.tokens.push_back(std::move(token));
            }
        }
        decompile_newline(basic_line);
    }
}

bool Decompiler::decompile_rem_code(BasicLine& basic_line) {
    if (mem->peek(addr) == C_REM) {
        bool is_code = false;
        for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
            if ((mem->peek(p) & 0x40) != 0) {	    // special char
                is_code = true;
                break;
            }
        }
        if (is_code) {
            Token rem;
            rem.code = C_REM;
            basic_line.tokens.push_back(std::move(rem));

            Token bytes;
            bytes.code = T_rem_code;
            for (int p = addr + 1; p < end - 1; p++) {
                bytes.bytes.push_back(mem->peek(p));
            }
            disasm->set_unknown(addr, INT(bytes.bytes.size()));
            basic_line.tokens.push_back(std::move(bytes));

            addr = end - 1;
            decompile_newline(basic_line);
            return true;
        }
    }
    return false;
}

bool Decompiler::decompile_number(BasicLine& basic_line) {
    Token token;
    std::string num;

    // get mantissa
    int p = addr;
    int num_dots = 0;
    int num_digits = 0;
    while (true) {
        int c = mem->peek(p);
        if (c == C_dot) {
            p++;
            num_dots++;
            num.push_back('.');
            if (num_dots > 1) {
                return false;
            }
        }
        else if (c >= C_0 && c <= C_9) {
            p++;
            num_digits++;
            num.push_back(c - C_0 + '0');
        }
        else {
            break;
        }
    }
    if (num_digits == 0) {
        return false;
    }

    // get exponent
    int c = mem->peek(p);
    if (c == C_E) {
        p++;
        num.push_back('E');
        c = mem->peek(p);
        if (c == C_plus || c == C_minus) {
            p++;
            num.push_back(c == C_plus ? '+' : '-');
        }
        c = mem->peek(p);
        if (c < C_0 || c > C_9) {
            return false;
        }

        while (c >= C_0 && c <= C_9) {
            p++;
            num.push_back(c - C_0 + '0');
            c = mem->peek(p);
        }
    }

    double value1 = atof(num.c_str());

    // get number marker
    c = mem->peek(p);
    if (c != C_number) {
        return false;
    }

    p++;

    // get fp value
    double value2 = mem->fpeek(p);
    p += 5;

    if (fabs(value1 - value2) > Epsilon) {
        error("number " + std::to_string(value1) + " != " + std::to_string(value2) +
              " at " + fmt_hex(addr, 4));
    }

    token.code = T_float;
    token.svalue = num;
    token.fvalue = value1;
    basic_line.tokens.push_back(std::move(token));
    addr = p;
    return true;
}

bool Decompiler::decompile_ident(BasicLine& basic_line) {
    Token token;
    int p = addr;
    while (true) {
        int c = mem->peek(p);
        if (c >= C_0 && c <= C_9) {
            token.ident.push_back(c - C_0 + '0');
            p++;
            continue;
        }
        if (c >= C_A && c <= C_Z) {
            token.ident.push_back(c - C_A + 'A');
            p++;
            continue;
        }
        break;
    }
    if (p == addr) {				// no letters found
        return false;
    }
    else {
        token.code = T_ident;
        basic_line.tokens.push_back(std::move(token));
        addr = p;
        return true;
    }
}

bool Decompiler::decompile_string(BasicLine& basic_line) {
    Token token;
    int p = addr;
    if (mem->peek(p) != C_dquote) {
        return false;
    }
    p++;
    while (true) {
        int c = mem->peek(p);
        if (c == C_newline) {
            return false;
        }
        if (c == C_dquote) {
            break;
        }

        token.svalue += decode_zx81_str_char(c);

        p++;
    }
    p++;		// skip end dquote

    token.code = T_string;
    basic_line.tokens.push_back(std::move(token));
    addr = p;
    return true;
}

void Decompiler::decompile_newline(BasicLine& basic_line) {
    Token token;
    token.code = static_cast<ZX81char>(mem->peek(addr++));
    if (token.code != C_newline) {
        error("missing newline");
    }

    basic_line.tokens.push_back(std::move(token));
}

void Decompiler::decompile_vars() {
    addr = mem->dpeek(VARS);
    int c = 0;
    while ((c = mem->peek(addr)) != 0x80) {
        BasicVar var;
        var.addr = addr;
        int addr0 = addr;

        if ((c & 0xe0) == 0x60) {		// single letter variable
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::Number;
            var.name = decode_zx81(c);
            var.value = mem->fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
            var.type = BasicVar::Type::Number;

            // first letter
            addr++;
            c &= 0x3f;
            c |= 0x20;
            var.name = decode_zx81(c);

            // second, ... letter
            while (((c = mem->peek(addr)) & 0xc0) == 0x00) {
                addr++;
                c &= 0x3f;
                c |= 0x20;
                var.name += decode_zx81(c);
            }

            // last letter
            c = mem->peek(addr);
            if ((c & 0xc0) != 0x80) {
                error("invalid multi-letter variable" + fmt_hex(c, 2));
                return;
            }
            else {
                addr++;
                c &= 0x3f;
                c |= 0x20;
                var.name += decode_zx81(c);
            }
            var.value = mem->fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0x80) {	// array of numbers
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ArrayNumbers;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dimensions = mem->peek(addr++);
            int num_elements = 1;
            for (int i = 0; i < num_dimensions; i++) {
                int dimension = mem->dpeek(addr);
                addr += 2;
                num_elements *= dimension;
                var.dimensions.push_back(dimension);
            }
            for (int i = 0; i < num_elements; i++) {
                double value = mem->fpeek(addr);
                addr += 5;
                var.values.push_back(value);
            }

            if (var.dimensions.empty()) {
                error("array of numbers has no dimensions");
            }
            if (INT(var.values.size()) != num_elements) {
                error("array of numbers has " + std::to_string(var.values.size())
                      + " values but " + std::to_string(num_elements) + " expected");
            }
            if (addr0 + size != addr) {
                error("array of numbers has size " + std::to_string(size)
                      + " but " + std::to_string(addr - addr0) + " bytes were read");
            }
        }
        else if ((c & 0xe0) == 0xe0) {	// for-next loop
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ForNextLoop;
            var.name = decode_zx81(c);

            var.value = mem->fpeek(addr);
            addr += 5;
            var.limit = mem->fpeek(addr);
            addr += 5;
            var.step = mem->fpeek(addr);
            addr += 5;
            var.line_num = mem->dpeek(addr);
            addr += 2;
        }
        else if ((c & 0xe0) == 0x40) {	// string
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::String;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            for (int i = 0; i < size; i++) {
                c = mem->peek(addr++);
                var.str += decode_zx81_str_char(c);
            }
        }
        else if ((c & 0xe0) == 0xc0) {	// array of strings
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ArrayStrings;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dimensions = mem->peek(addr++);
            int num_elements = 1;
            for (int i = 0; i < num_dimensions; i++) {
                int dimension = mem->dpeek(addr);
                addr += 2;
                num_elements *= dimension;
                var.dimensions.push_back(dimension);
            }

            int last_dimension = var.dimensions.back();		// size of each string
            num_elements /= last_dimension;

            for (int i = 0; i < num_elements; i++) {
                std::string str;
                for (int j = 0; j < last_dimension; j++) {
                    c = mem->peek(addr++);
                    str += decode_zx81_str_char(c);
                }
                var.strs.push_back(str);
            }

            if (var.dimensions.empty()) {
                error("array of numbers has no dimensions");
            }
            if (INT(var.strs.size()) != num_elements) {
                error("array of numbers has " + std::to_string(var.strs.size())
                      + " values but " + std::to_string(num_elements) + " expected");
            }
            if (addr0 + size != addr) {
                error("array of numbers has size " + std::to_string(size)
                      + " but " + std::to_string(addr - addr0) + " bytes were read");
            }
        }
        else {
            error("invalid variable marker" + fmt_hex(c, 2));
            break;
        }

        var.size = addr - addr0;

        basic->basic_vars.push_back(var);
    }
}

void Decompiler::disassemble() {
    delete basic_info;
    basic_info = new BasicInfo(basic);

    ctl_file->apply_elements(basic_info, disasm);
    find_gotos();
    find_usrs();
    return; // TODO

    // search all USR n in Basic
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            std::vector<Token>& tokens = line.basic_line.tokens;
            if (tokens.size() > 1) {
                for (size_t i = 0; i + 1 < tokens.size(); i++) {
                    if (tokens[i].code == C_USR && tokens[i + 1].code == T_float) {
                        int addr = INT(tokens[i + 1].fvalue);
                        if (addr >= PROG && addr < mem->dpeek(E_LINE)) {
                            std::string label = disasm->get_add_label(addr);
                            tokens[i + 1].code = T_line_addr_ref;
                            tokens[i + 1].ident = label;
                            disasm->set_code(addr);
                        }
                    }
                }
            }
        }
    }
}

// find all GOTO and GOSUB line numbers and replace them with labels
void Decompiler::find_gotos() {
    std::vector<ZX81char> functions = { C_GOTO, C_GOSUB };
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            std::vector<Token>& tokens = line.basic_line.tokens;
            size_t pos = 0;
            int target_line_num = 0;
            while (pos < tokens.size()
                    && find_function_arg(tokens, pos, functions, target_line_num)) {
                SourceLine* target_line = basic_info->get_source_line(target_line_num);
                if (target_line) {
                    std::string label = basic_info->get_label(target_line_num);
                    if (!label.empty()) {
                        tokens[pos].code = T_line_num_ref;
                        tokens[pos].ident = label;
                    }
                }
            }
        }
    }
}

void Decompiler::find_usrs() {
    std::vector<ZX81char> functions = { C_USR };
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            std::vector<Token>& tokens = line.basic_line.tokens;
            size_t pos = 0;
            int target_addr = 0;
            while (pos < tokens.size()
                    && find_function_arg(tokens, pos, functions, target_addr)) {
                if (target_addr >= PROG && target_addr < mem->dpeek(E_LINE)) {
                    std::string label = disasm->get_add_label(target_addr);
                    if (!label.empty()) {
                        tokens[pos].code = T_line_addr_ref;
                        tokens[pos].ident = label;
                    }
                    disasm->set_code(target_addr);
                }
            }
        }
    }
}

bool Decompiler::find_function_arg(const std::vector<Token>& tokens,
                                   size_t& pos,
                                   const std::vector<ZX81char>& functions, int& arg) {
    for (size_t i = pos; i < tokens.size(); i++) {
        if (std::find(functions.begin(), functions.end(),
                      tokens[i].code) == functions.end()) {
            continue;
        }

        // got one of the functions
        pos = i + 1;
        while (pos < tokens.size() && tokens[pos].code == C_space) {
            pos++;
        }

        if (pos < tokens.size() && tokens[pos].code == T_float) {
            arg = INT(tokens[pos].fvalue);
            return true;
        }
    }
    pos = tokens.size();
    return false;
}
