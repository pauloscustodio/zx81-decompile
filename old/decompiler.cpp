//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "decompiler.h"
#include "disasm.h"
#include "errors.h"
#include "memory.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81chars.h"
#include <fstream>

static Decompiler g_decompiler;

void Decompiler::decompile(Basic& basic_) {
    basic = &basic_;

    disassemble();

    basic = nullptr;
}

void Decompiler::decompile(Basic& basic, const std::string& ctl_filename) {
    parse_ctl_file(ctl_filename);
    decompile(basic);
}

void Decompiler::disassemble() {
    // apply MemElement
    for (auto& elem : mem_elements) {
        if (!elem.header.empty()) {
            g_disasm_code.add_header(elem.addr, elem.header);
        }

        if (!elem.comment.empty()) {
            g_disasm_code.set_comment(elem.addr, elem.comment);
        }

        switch (elem.type) {
        case MemElement::Type::Defb:
            g_disasm_code.set_defb(elem.addr, elem.size);
            break;
        case MemElement::Type::Defw:
            g_disasm_code.set_defw(elem.addr, elem.size);
            break;
        case MemElement::Type::Defm:
            g_disasm_code.set_defm(elem.addr, elem.size);
            break;
        case MemElement::Type::Code:
            g_disasm_code.set_code(elem.addr);
            break;
        case MemElement::Type::Comment:
            break;
        default:
            release_assert(0);
        }

        if (!elem.label.empty()) {
            g_asm_labels.add(elem.label, elem.addr);
        }
    }

    // search all USR n in Basic
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            std::vector<Token>& tokens = line.basic_line.tokens;
            if (tokens.size() > 1) {
                for (size_t i = 0; i < tokens.size() - 1; i++) {
                    if (tokens[i].code == C_USR && tokens[i + 1].code == T_float) {
                        int addr = INT(tokens[i + 1].fvalue);
                        if ((addr >= 0 && addr < 0x2000)
                                || (addr >= PROG + 5 && addr < dpeek(E_LINE))) {
                            std::string label = g_disasm_code.get_label(addr);
                            tokens[i + 1].code = T_line_addr_ref;
                            tokens[i + 1].ident = label;
                            g_disasm_code.set_code(addr);
                        }
                    }
                }
            }
        }
    }
}

void Decompiler::parse_ctl_file(const std::string& ctl_filename) {
    std::ifstream ifs(ctl_filename);
    if (!ifs.is_open()) {
        perror(ctl_filename.c_str());
        fatal_error("read file", ctl_filename);
    }

    err_set_filename(ctl_filename);

    std::string text;
    int line_num = 0;
    while (getline(ifs, text)) {
        line_num++;
        err_set_line_num(line_num);
        p = text.c_str();
        parse_ctl_line();
    }

    err_clear();
}

void Decompiler::parse_ctl_line() {
    MemElement elem;

    if (at_end('#')) {
        return;
    }

    // get address
    if (!parse_integer(elem.addr)) {
        error("address expected");
        return;
    }

    // get type
    if (match("B")) {
        elem.type = MemElement::Type::Defb;
    }
    else if (match("W")) {
        elem.type = MemElement::Type::Defw;
    }
    else if (match("M")) {
        elem.type = MemElement::Type::Defm;
    }
    else if (match("C")) {
        elem.type = MemElement::Type::Code;
    }
    else if (match(";")) {
        skip_spaces();
        elem.type = MemElement::Type::Comment;
        elem.comment = p;
        mem_elements.push_back(elem);
        return;
    }
    else if (match("#")) {
        skip_spaces();
        elem.type = MemElement::Type::Comment;
        elem.header += str_chomp(p) + "\n";
        mem_elements.push_back(elem);
        return;
    }
    else {
        error("expected B,W,M,C,;,#");
        return;
    }

    if (at_end('#')) {
        mem_elements.push_back(elem);
        return;
    }

    // get size
    if (match("(")) {
        if (!(parse_integer(elem.size) && match(")"))) {
            error("expected (size)");
            return;
        }
    }

    if (at_end('#')) {
        mem_elements.push_back(elem);
        return;
    }

    // get label
    if (parse_ident(elem.label)) {
    }

    // get comment
    if (match("#")) {
        skip_spaces();
        elem.header += str_chomp(p) + "\n";
    }
    else if (match(";")) {
        skip_spaces();
        elem.comment = p;
    }

    mem_elements.push_back(elem);
}

void decompile(Basic& basic) {
    g_decompiler.decompile(basic);
}

void decompile(Basic& basic, const std::string& ctl_filename) {
    g_decompiler.decompile(basic, ctl_filename);
}
