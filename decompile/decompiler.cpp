//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "ctl_file.h"
#include "decompiler.h"
#include "errors.h"
#include "release_assert.h"
#include "token.h"
#include "types.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

void Decompiler::decompile() {
    decompile_sysvars();
    init_ctl_mem();

    decompile_dfile();
    decompile_vars();
    decompile_basic();

    map_source_lines();
    analyse_ctl_file();
    analyse_goto();
    analyse_usr();
}

void Decompiler::decompile_sysvars() {
    // get sysvars from memory
    basic->sysvars = mem->peek_bytes(SAVE_ADDR, PROG - SAVE_ADDR);

    // get autostart
    int d_file = mem->dpeek(D_FILE);
    int nxtlin = mem->dpeek(NXTLIN);
    if (nxtlin >= d_file) {
        basic->autostart = false;
        basic->autostart_line = 0;
    }
    else {
        basic->autostart = true;
        basic->autostart_line = mem->dpeek_be(nxtlin);
    }

    // get fast
    if ((mem->peek(CDFLAG) & FlagSlow) == FlagSlow) {
        basic->fast = false;
    }
    else {
        basic->fast = true;
    }
}

void Decompiler::decompile_dfile() {
    int d_file = mem->dpeek(D_FILE);
    int vars = mem->dpeek(VARS);

    // dfile collapsed
    if (d_file + 1 + NumRows * (NumCols + 1) > vars) {
        basic->dfile_collapsed = true;
    }
    else {
        basic->dfile_collapsed = false;
    }

    // dfile lines
    if (mem->peek(d_file) != C_newline) {
        fatal_error("malformed dfile at address " + fmt_hex(d_file, 4)
                    + ": expected newline");
    }

    std::vector<std::string> dfile_lines;
    for (int addr = d_file + 1, row = 0; addr < vars; row++) {
        std::string dfile_line;
        for (int col = 0; ; col++) {
            int c = mem->peek(addr++);
            if (c == C_newline) {
                break;
            }
            if (col >= NumCols) {
                fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                            + ": line too long");
            }
            if ((c & 0x40) != 0) {
                fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                            + ": invalid character " + fmt_hex(c, 2));
            }

            dfile_line += decode_zx81(c);
        }
        if (row >= NumRows) {
            fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                        + ": too many lines");
        }

        if (!basic->dfile_collapsed) {
            dfile_line = str_trim(dfile_line);
        }
        dfile_lines.push_back(dfile_line);
    }

    // trim empty lines at the end
    while (!dfile_lines.empty() && dfile_lines.back().empty()) {
        dfile_lines.pop_back();
    }

    basic->dfile_lines = dfile_lines;
}

void Decompiler::decompile_vars() {
    int addr = mem->dpeek(VARS);
    int c = 0;
    while ((c = mem->peek(addr)) != 0x80) {
        BasicVar var(BasicVar::Type::Number, SourceLoc());

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
                var.name += decode_zx81(c);
            }

            // last letter
            c = mem->peek(addr);
            if ((c & 0xc0) != 0x80) {
                error("invalid multi-letter variable " + fmt_hex(c, 2)
                      + " at address " + fmt_hex(addr, 4));
                return;
            }
            else {
                addr++;
                c &= 0x7f;
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

            int num_dims = mem->peek(addr++);
            int num_elems = 1;
            for (int i = 0; i < num_dims; i++) {
                int dim = mem->dpeek(addr);
                addr += 2;
                num_elems *= dim;
                var.dims.push_back(dim);
            }
            for (int i = 0; i < num_elems; i++) {
                double value = mem->fpeek(addr);
                addr += 5;
                var.values.push_back(value);
            }

            release_assert(addr0 + size == addr);
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
                var.str += decode_zx81(c);
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

            int num_dims = mem->peek(addr++);
            int num_elems = 1;
            for (int i = 0; i < num_dims; i++) {
                int dim = mem->dpeek(addr);
                addr += 2;
                num_elems *= dim;
                var.dims.push_back(dim);
            }

            int last_dim = var.dims.back();		// size of each string
            num_elems /= last_dim;

            for (int i = 0; i < num_elems; i++) {
                std::string str;
                for (int j = 0; j < last_dim; j++) {
                    c = mem->peek(addr++);
                    str += decode_zx81(c);
                }
                var.strs.push_back(str);
            }

            release_assert(addr0 + size == addr);
        }
        else {
            error("invalid variable marker " + fmt_hex(c, 2)
                  + " at address " + fmt_hex(addr, 4));
            break;
        }

        basic->basic_vars.push_back(var);
    }
}

void Decompiler::decompile_basic() {
    basic->source_lines.clear();
    for (addr = PROG; addr < mem->dpeek(D_FILE); ) {
        SourceLine line(SourceLine::Type::Basic, SourceLoc());
        line.addr = addr;
        line.line_num = mem->dpeek_be(addr);
        int size = mem->dpeek(addr + 2);

        addr += 4;
        end = addr + size;

        decompile_basic_line(line);
        basic->source_lines.push_back(line);
        addr = end;
    }
}

void Decompiler::decompile_basic_line(SourceLine& line) {
    if (decompile_rem_code(line)) {
    }
    else {
        while (addr < end - 1) {
            if (decompile_number(line)) {
            }
            else if (decompile_ident(line)) {
            }
            else if (decompile_string(line)) {
            }
            else {
                Token token;
                token.code = static_cast<ZX81char>(mem->peek(addr++));
                line.tokens.push_back(token);
            }
        }
        decompile_newline(line);
    }
}

bool Decompiler::decompile_rem_code(SourceLine& line) {
    if (mem->peek(addr) == C_REM) {
        bool is_code = false;
        for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
            if ((mem->peek(p) & 0x40) == 0x40) {    // special char
                is_code = true;
                break;
            }
        }
        if (is_code) {
            Token rem;
            rem.code = C_REM;
            line.tokens.push_back(rem);

            Token bytes;
            bytes.code = T_rem_code;
            bytes.bytes = mem->peek_bytes(addr + 1,
                                          end - addr - 2);	// all bytes except final newline)
            line.tokens.push_back(bytes);

            // mark rem code in memory map
            disasm_code->set_mem_type_no_overlap_check(addr + 1, MemType::Unknown,
                    INT(bytes.bytes.size()));

            addr = end - 1;
            decompile_newline(line);
            return true;
        }
    }
    return false;
}

bool Decompiler::decompile_number(SourceLine& line) {
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

    if (fabs(value1 - value2) > 1e-10) {
        error("number " + std::to_string(value1) + " != " + std::to_string(value2)
              + " at address " + fmt_hex(addr, 4));
    }

    token.code = T_float;
    token.svalue = num;
    token.fvalue = value1;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

bool Decompiler::decompile_ident(SourceLine& line) {
    Token token;
    int p = addr;

    // get first letter
    int c = mem->peek(p);
    if (c < C_A || c > C_Z) {
        return false;
    }
    token.ident.push_back(c - C_A + 'A');
    p++;

    // get second, ... letter or digit
    while (true) {
        int c = mem->peek(p);
        if (c >= C_A && c <= C_Z) {
            token.ident.push_back(c - C_A + 'A');
            p++;
            continue;
        }
        if (c >= C_0 && c <= C_9) {
            token.ident.push_back(c - C_0 + '0');
            p++;
            continue;
        }
        break;
    }

    token.code = T_ident;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

bool Decompiler::decompile_string(SourceLine& line) {
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
        token.svalue += decode_zx81(c);
        p++;
    }
    p++;		// skip end dquote

    token.code = T_string;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

void Decompiler::decompile_newline(SourceLine& line) {
    Token token;
    token.code = static_cast<ZX81char>(mem->peek(addr++));
    if (token.code != C_newline) {
        error("missing newline at address " + std::to_string(addr - 1));
    }

    line.tokens.push_back(token);
}

void Decompiler::init_ctl_mem() {
    int d_file = mem->dpeek(D_FILE);
    int vars = mem->dpeek(VARS);
    int e_line = mem->dpeek(E_LINE);

    for (int addr = ERR_NO; addr < PROG; addr++) {
        disasm_code->set_mem_type_no_overlap_check(addr, MemType::Sysvars);
    }
    for (int addr = PROG; addr < d_file; addr++) {
        disasm_code->set_mem_type_no_overlap_check(addr, MemType::Basic);
    }
    for (int addr = d_file; addr < vars; addr++) {
        disasm_code->set_mem_type_no_overlap_check(addr, MemType::Dfile);
    }
    for (int addr = vars; addr < e_line; addr++) {
        disasm_code->set_mem_type_no_overlap_check(addr, MemType::Vars);
    }
}

void Decompiler::map_source_lines() {
    // map all source lines by line number
    source_lines.clear();
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            source_lines[line.line_num] = &line;
        }
    }
}

void Decompiler::analyse_ctl_file() {
    auto& basic_labels = disasm_code->get_basic_labels();
    auto& asm_labels = disasm_code->get_asm_labels();

    for (auto& elem : ctl_file->elems) {
        if (elem.type == CtlElement::Type::BasicCode) {
            // Basic line number
            if (!elem.label.empty()) {
                // create new label
                basic_labels.add(elem.label, elem.addr);
            }

            auto it = source_lines.find(elem.addr);
            if (it == source_lines.end()) {
                error(elem.loc, "undefined line number: " + std::to_string(elem.addr));
            }
            else {
                // get created label or automatically generated label
                std::string label = basic_labels.add_auto_gen(elem.addr);
                SourceLine* target_line = it->second;
                target_line->label = label;
            }
        }
        else if (elem.type == CtlElement::Type::AsmCode) {
            // Asm code
            basic->has_asm_code = true;

            if (!elem.label.empty()) {
                // create new label
                asm_labels.add(elem.label, elem.addr);
            }

            if (elem.addr < SAVE_ADDR || elem.addr >= mem->dpeek(E_LINE)) {
                error(elem.loc, "invalid Asm code address: " + fmt_hex(elem.addr, 4));
            }
            else if (disasm_code->get_mem_type(elem.addr) != MemType::Asm &&
                     disasm_code->get_mem_type(elem.addr) != MemType::Unknown) {
                error(elem.loc, "invalid Asm code address: " + fmt_hex(elem.addr, 4));
            }
            else {
                disasm_code->set_code(elem.addr);
            }
        }
        else if (elem.type == CtlElement::Type::Byte) {
            basic->has_asm_code = true;

            if (!elem.label.empty()) {
                // create new label
                asm_labels.add(elem.label, elem.addr);
            }

            disasm_code->set_byte(elem.addr, elem.count);
        }
        else if (elem.type == CtlElement::Type::Word) {
            basic->has_asm_code = true;

            if (!elem.label.empty()) {
                // create new label
                asm_labels.add(elem.label, elem.addr);
            }

            disasm_code->set_word(elem.addr, elem.count);
        }
        else if (elem.type == CtlElement::Type::Text) {
            basic->has_asm_code = true;

            if (!elem.label.empty()) {
                // create new label
                asm_labels.add(elem.label, elem.addr);
            }

            disasm_code->set_text(elem.addr, elem.count);
        }
        else if (elem.type == CtlElement::Type::Header) {
            disasm_code->add_header(elem.addr, elem.text);
        }
        else if (elem.type == CtlElement::Type::Comment) {
            disasm_code->add_comment(elem.addr, elem.text);
        }
        else {
            error(elem.loc, "invalid element type");
        }
    }
}

void Decompiler::analyse_goto() {
    auto& basic_labels = disasm_code->get_basic_labels();

    // search for GOTO, GOSUB, RUN and line number references
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            for (auto& token : line.tokens) {
                if (token.code == C_GOTO || token.code == C_GOSUB || token.code == C_RUN) {
                    size_t index = &token - &line.tokens[0];
                    if (index + 1 < line.tokens.size()) {
                        auto& next_token = line.tokens[index + 1];
                        if (next_token.code == T_float) {
                            int target_line_num = static_cast<int>(next_token.fvalue);
                            auto it = source_lines.find(target_line_num);
                            if (it == source_lines.end()) {
                                std::cerr << "undefined line number: " + std::to_string(
                                              target_line_num) << std::endl;
                            }
                            else {
                                // create new label/use existing one
                                std::string label = basic_labels.add_auto_gen(target_line_num);
                                SourceLine* target_line = it->second;
                                target_line->label = label;

                                // replace token with label
                                next_token.code = T_line_num_ref;
                                next_token.ident = label;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Decompiler::analyse_usr() {
    auto& asm_labels = disasm_code->get_asm_labels();

    // search for USR and routine addresses
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            for (auto& token : line.tokens) {
                if (token.code == C_USR) {
                    size_t index = &token - &line.tokens[0];
                    if (index + 1 < line.tokens.size()) {
                        auto& next_token = line.tokens[index + 1];
                        if (next_token.code == T_float) {
                            int target_addr = static_cast<int>(next_token.fvalue);
                            if (target_addr >= mem->dpeek(E_LINE)) {
                                std::cerr << "invalid USR address: "
                                          << fmt_hex(target_addr, 4) << std::endl;
                            }
                            else if (target_addr < RAM_ADDR) {
                                // call into ROM
                            }
                            else {
                                // create new label/use existing one
                                std::string label = asm_labels.add_auto_gen(target_addr);

                                // replace token with label
                                next_token.code = T_line_addr_ref;
                                next_token.ident = label;

                                // mark address as Asm code
                                basic->has_asm_code = true;
                                disasm_code->set_code(target_addr);
                            }
                        }
                    }
                }
            }
        }
    }
}
