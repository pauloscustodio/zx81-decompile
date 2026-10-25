//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "disasm.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "parser.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81chars.h"
#include <fstream>
#include <iostream>

//-----------------------------------------------------------------------------
// Symtab
//-----------------------------------------------------------------------------

Symbol::Symbol(Type type_, const std::string& name_, int value_)
    : type(type_), name(name_), value(value_) {
}

Symtab::Symtab() {
    init();
}

Symtab::~Symtab() {
    for (auto& it : symbols) {
        delete it.second;
    }
    symbols.clear();
}

void Symtab::init() {
    for (auto& it : symbols) {
        delete it.second;
    }
    symbols.clear();

#define X(name, value)		add(Symbol::Type::Const, #name, value);
#include "zx81consts.def"
}

void Symtab::add(Symbol::Type type, const std::string& name, int value) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        error("duplicate definition", name);
    }
    else {
        auto symbol = new Symbol(type, name, value);
        symbols[name] = symbol;
    }
}

void Symtab::update(Symbol::Type type, const std::string& name, int value) {
    auto symbol = get(name);
    if (symbol) {
        symbol->type = type;
        symbol->value = value;
    }
    else {
        add(type, name, value);
    }
}

Symbol* Symtab::get(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}

Symbol* Symtab::find(int value) {
    for (auto& it : symbols) {
        if (it.second->value == value) {
            return it.second;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
// Basic
//-----------------------------------------------------------------------------

Basic::Basic() {
    clear();
}

void Basic::clear() {
    source_lines.clear();
    basic_vars.clear();
    symtab.init();
    d_file_bytes = get_empty_d_file();
    autostart = 0;
    auto_increment = 10;
    fast = false;
}

int Basic::eval_const_expr(const Expr& rpn) {
    std::vector<int> stack;
    Symbol* symbol{ nullptr };
    int value = 0;

    for (auto& token : rpn) {
        switch (token.code) {
        case T_integer:
            stack.push_back(token.ivalue);
            break;
        case T_line_num_ref:
            symbol = symtab.get(token.ident);
            if (symbol && symbol->type == Symbol::Type::Const) {
                stack.push_back(dpeek_be(symbol->value));
            }
            else {
                error("const expression refers to non-const symbol", token.ident);
                return 0;
            }
            break;
        case T_line_addr_ref:
            symbol = symtab.get(token.ident);
            if (symbol && symbol->type == Symbol::Type::Const) {
                stack.push_back(symbol->value);
            }
            else {
                error("const expression refers to non-const symbol", token.ident);
                return 0;
            }
            break;
        case T_unary_minus:
            stack.back() = -stack.back();
            break;
        case C_plus:
            value = stack.back();
            stack.pop_back();
            stack.back() += value;
            break;
        case C_minus:
            value = stack.back();
            stack.pop_back();
            stack.back() -= value;
            break;
        case C_mult:
            value = stack.back();
            stack.pop_back();
            stack.back() *= value;
            break;
        case C_div:
            value = stack.back();
            stack.pop_back();
            stack.back() /= value;
            break;
        default:
            release_assert(0);
        }
    }

    release_assert(stack.size() == 1);
    return stack.back();
}

bool Basic::eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error) {
    std::vector<int> stack;
    Symbol* symbol{ nullptr };
    bool result = true;

    for (auto& token : rpn) {
        switch (token.code) {
        case T_integer:
            stack.push_back(token.ivalue);
            break;
        case T_line_num_ref:
            symbol = symtab.get(token.ident);
            if (symbol) {
                stack.push_back(dpeek_be(symbol->value));
            }
            else {
                result = false;
                if (do_error) {
                    error("undefined symbol", token.ident);
                }
                stack.push_back(0);
            }
            break;
        case T_line_addr_ref:
            symbol = symtab.get(token.ident);
            if (symbol) {
                stack.push_back(symbol->value);
            }
            else {
                result = false;
                if (do_error) {
                    error("undefined symbol", token.ident);
                }
                stack.push_back(0);
            }
            break;
        case T_ASMPC:
            stack.push_back(asmpc);
            break;
        case T_unary_minus:
            stack.back() = -stack.back();
            break;
        case C_plus:
            value = stack.back();
            stack.pop_back();
            stack.back() += value;
            break;
        case C_minus:
            value = stack.back();
            stack.pop_back();
            stack.back() -= value;
            break;
        case C_mult:
            value = stack.back();
            stack.pop_back();
            stack.back() *= value;
            break;
        case C_div:
            value = stack.back();
            stack.pop_back();
            stack.back() /= value;
            break;
        default:
            release_assert(0);
        }
    }

    release_assert(stack.size() == 1);
    value = stack.back();
    return result;
}

void Basic::write_b81_file(const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        fatal_error("write file", filename);
    }

    write_basic_lines(ofs);
    write_basic_memory_map(ofs);
}

void Basic::parse_b81_file(const std::string& filename) {
    ::parse_b81_file(*this, filename);
}

void Basic::write_basic_lines(std::ofstream& ofs) const {
    for (auto& line : source_lines) {
        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [" << fmt_hex(line.addr, 4) << "]" << std::endl;
        }
        if (line.type == SourceLine::Type::Basic) {
            if (!line.basic_line.label.empty()) {
                ofs << "@" << line.basic_line.label << ":" << std::endl;
            }

            ofs << fmt_line_number(line.basic_line.line_num) << " ";

            for (auto& token : line.basic_line.tokens) {
                switch (token.code) {
                case T_none:
                    break;
                case T_float:
                    ofs << token.svalue;
                    break;
                case T_string:
                    ofs << "\"" << token.svalue << "\"";
                    break;
                case T_ident:
                    ofs << token.ident;
                    break;
                case T_rem_code:
                    write_mem_info(ofs, line.addr + 5, INT(token.bytes.size()));
                    break;
                case T_line_addr_ref:
                    ofs << "&" << token.ident;
                    break;
                case T_line_num_ref:
                    ofs << "@" << token.ident;
                    break;
                case C_space:
                    ofs << "_";
                    break;
                case C_newline:
                    ofs << std::endl;
                    break;
                default:
                    release_assert(token.code < 0x100);
                    ofs << decode_zx81(token.code);
                }
            }
        }
    }

    if (!source_lines.empty()) {
        ofs << std::endl;
    }
}

void Basic::write_basic_memory_map(std::ofstream& ofs) const {
    ofs << std::endl;
    int d_file = dpeek(D_FILE);
    int vars = dpeek(VARS);
    int e_line = dpeek(E_LINE);
    for (int addr = RAM_ADDR; addr < e_line; addr += 64) {
        ofs << "# [" << fmt_hex(addr, 4) << "] ";
        for (int p = addr; p < addr + 64; p++) {
            if (p < PROG) {
                ofs << "!";
            }
            else if (p < d_file) {
                switch (g_disasm_code.get_type(p)) {
                case Opcode::Type::Undef:
                    ofs << "#";
                    break;
                case Opcode::Type::Unknown:
                    ofs << "-";
                    break;
                case Opcode::Type::Asm:
                    ofs << "C";
                    break;
                case Opcode::Type::AsmData:
                    ofs << "C";
                    break;
                case Opcode::Type::Defb:
                    ofs << "B";
                    break;
                case Opcode::Type::DefbData:
                    ofs << "B";
                    break;
                case Opcode::Type::Defw:
                    ofs << "W";
                    break;
                case Opcode::Type::DefwData:
                    ofs << "W";
                    break;
                case Opcode::Type::Defm:
                    ofs << "T";
                    break;
                case Opcode::Type::DefmData:
                    ofs << "T";
                    break;
                default:
                    release_assert(0);
                }
            }
            else if (p < vars) {
                ofs << "*";
            }
            else {
                ofs << "$";
            }
        }
        ofs << std::endl;
    }
}

void Basic::write_mem_info(std::ofstream& ofs, int start_addr, int len) const {
    ofs << std::endl;			// newline after REM
    ofs << "#ASM" << std::endl;

    for (int addr = start_addr; addr < start_addr + len; ) {
        Opcode* opc = g_disasm_code.get(addr);
        ofs << opc->to_string();
        addr += opc->size;
    }

    ofs << "#ENDASM";	// followed by newline from REM
}


//-----------------------------------------------------------------------------
// Lines
//-----------------------------------------------------------------------------

AsmLine::AsmLine()
    : source_filename(err_get_filename()), source_line_num(err_get_line_num()) {
}

BasicLine::BasicLine()
    : source_filename(err_get_filename()), source_line_num(err_get_line_num()) {
}

