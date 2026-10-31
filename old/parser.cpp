//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "memory.h"
#include "parser.h"
#include "utils.h"
#include "zx81chars.h"
#include <cmath>
#include <fstream>
#include <iostream>

static Parser g_parser;

bool Parser::parse_asm_expr(Expr& rpn) {
    return parse_const_expr(true, rpn);
}

bool Parser::parse_basic_expr(Expr& rpn) {
    return parse_const_expr(false, rpn);
}

bool Parser::parse_const_expr(bool in_asm, Expr& rpn) {
    skip_spaces();
    const char* p0 = p;
    rpn.clear();
    if (parse_expr(in_asm, rpn)) {
        return true;
    }
    else {
        p = p0;
        rpn.clear();
        return false;
    }
}

bool Parser::parse_expr(bool in_asm, Expr& rpn) {
    if (!parse_term(in_asm, rpn)) {
        return false;
    }
    skip_spaces();
    const char* p0 = p;
    while (*p0 == '+' || *p0 == '-') {
        Token token;
        if (match("+") && parse_term(in_asm, rpn)) {
            token.code = C_plus;
            rpn.push_back(token);
        }
        else if (match("-") && parse_term(in_asm, rpn)) {
            token.code = C_minus;
            rpn.push_back(token);
        }
        else {
            return false;
        }
        skip_spaces();
        p0 = p;
    }
    return true;
}

bool Parser::parse_term(bool in_asm, Expr& rpn) {
    if (!parse_factor(in_asm, rpn)) {
        return false;
    }
    skip_spaces();
    const char* p0 = p;
    while (*p0 == '*' || *p0 == '/') {
        Token token;
        if (match("*") && parse_factor(in_asm, rpn)) {
            token.code = C_mult;
            rpn.push_back(token);
        }
        else if (match("/") && parse_factor(in_asm, rpn)) {
            token.code = C_div;
            rpn.push_back(token);
        }
        else {
            return false;
        }
        skip_spaces();
        p0 = p;
    }
    return true;
}

bool Parser::parse_factor(bool in_asm, Expr& rpn) {
    Token token;
    if (match("-") && parse_factor(in_asm, rpn)) {
        token.code = T_unary_minus;
        rpn.push_back(token);
        return true;
    }
    if (match("+") && parse_factor(in_asm, rpn)) {
        return true;
    }
    else if (parse_integer(token.ivalue)) {
        token.code = T_integer;
        rpn.push_back(token);
        return true;
    }
    else if (parse_number(token.fvalue, token.svalue)
             && floor(token.fvalue) == token.fvalue) {
        token.code = T_integer;
        token.ivalue = INT(token.fvalue);
        rpn.push_back(token);
        return true;
    }
    else if (parse_line_num_ref(token.ident)) {
        token.code = T_line_num_ref;
        rpn.push_back(token);
        return true;
    }
    else if (parse_line_addr_ref(token.ident)) {
        token.code = T_line_addr_ref;
        rpn.push_back(token);
        return true;
    }
    else if (in_asm && parse_ident(token.ident)) {
        token.code = T_line_addr_ref;
        rpn.push_back(token);
        return true;
    }
    else if (match("(") && parse_expr(in_asm, rpn) && match(")")) {
        return true;
    }
    else if (match("$")) {
        token.code = T_ASMPC;
        rpn.push_back(token);
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_asm_line(AsmLine& asm_line) {
    if (parse_asm_label(asm_line.label)) {
        // is it EQU?
        skip_spaces();
        const char* p0 = p;
        if (match("EQU")) {
            if (parse_asm_expr(asm_line.equ_rpn) && parse_end_asm()) {
                return true;
            }
            else {
                return false;
            }
        }
        p = p0;
    }

    if (parse_defb(asm_line) && parse_end_asm()) {
        return true;
    }
    else if (parse_defw(asm_line) && parse_end_asm()) {
        return true;
    }
    else if (parse_defm(asm_line) && parse_end_asm()) {
        return true;
    }
    else if (parse_defs(asm_line) && parse_end_asm()) {
        return true;
    }
    else if (parse_z80_opcode(asm_line)) {
        return true;
    }
    else if (parse_end_asm()) {
        return true;
    }
    else {
        error("cannot parse", p);
        return false;
    }
}

bool Parser::parse_asm_label(std::string& ident) {
    if (isalpha_safe(*p) || *p == '_') {		// identifier at column 0
        parse_ident(ident);
        skip_spaces();
        match(":");						// optional colon
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_end_asm() {
    return at_end(';');
}

bool Parser::parse_defb(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    if (match("DEFB")) {
        while (true) {
            Expr rpn;
            if (!parse_asm_expr(rpn)) {
                return false;
            }
            add_byte_patch(asm_line, rpn);

            if (match(",")) {
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }
    p = p0;
    return false;
}

bool Parser::parse_defw(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    if (match("DEFW")) {
        while (true) {
            Expr rpn;
            if (!parse_asm_expr(rpn)) {
                return false;
            }
            add_word_patch(asm_line, rpn);

            if (match(",")) {
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }
    p = p0;
    return false;
}

bool Parser::parse_defm(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    if (match("DEFM")) {
        while (true) {
            std::string str;
            if (!parse_string(str)) {
                return false;
            }

            Bytes bytes = encode_zx81(str);
            for (auto& b : bytes) {
                asm_line.bytes.push_back(b);
            }

            if (match(",")) {
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }

    p = p0;
    return false;
}

bool Parser::parse_defs(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    if (match("DEFS")) {
        Expr expr;
        if (parse_asm_expr(expr) && parse_end_asm()) {
            int space = basic->eval_const_expr(expr);
            for (int i = 0; i < space; i++) {
                asm_line.bytes.push_back(0);
            }
            return true;
        }
    }

    p = p0;
    return false;
}

bool Parser::parse_z80_opcode(AsmLine& asm_line) {
    if (parse_end_asm()) {
        return true;
    }
    else if (parse_single_opcode(asm_line)) {
        return true;
    }
    else if (parse_ld(asm_line)) {
        return true;
    }
    else if (parse_alu(asm_line)) {
        return true;
    }
    else if (parse_ex(asm_line)) {
        return true;
    }
    else if (parse_bit(asm_line)) {
        return true;
    }
    else if (parse_call_jp(asm_line)) {
        return true;
    }
    else if (parse_rst(asm_line)) {
        return true;
    }
    else if (parse_ret(asm_line)) {
        return true;
    }
    else if (parse_jr(asm_line)) {
        return true;
    }
    else if (parse_inc_dec(asm_line)) {
        return true;
    }
    else if (parse_in(asm_line)) {
        return true;
    }
    else if (parse_out(asm_line)) {
        return true;
    }
    else if (parse_im(asm_line)) {
        return true;
    }
    else if (parse_push_pop(asm_line)) {
        return true;
    }
    else if (parse_rotate(asm_line)) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_single_opcode(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> single_opcode = {
        std::make_pair("NOP", 0x00),
        std::make_pair("RLCA", 0x07),
        std::make_pair("RRCA", 0x0f),
        std::make_pair("RLA", 0x17),
        std::make_pair("RRA", 0x1f),
        std::make_pair("DAA", 0x27),
        std::make_pair("CPL", 0x2f),
        std::make_pair("SCF", 0x37),
        std::make_pair("CCF", 0x3f),
        std::make_pair("HALT", 0x76),
        std::make_pair("RETN", 0xed45),
        std::make_pair("RETI", 0xed4d),
        std::make_pair("RET", 0xc9),
        std::make_pair("EXX", 0xd9),
        std::make_pair("DI", 0xf3),
        std::make_pair("EI", 0xfb),
        std::make_pair("NEG", 0xed44),
        std::make_pair("RRD", 0xed67),
        std::make_pair("RLD", 0xed6f),
        std::make_pair("LDIR", 0xedb0),
        std::make_pair("CPIR", 0xedb1),
        std::make_pair("INIR", 0xedb2),
        std::make_pair("OTIR", 0xedb3),
        std::make_pair("LDDR", 0xedb8),
        std::make_pair("CPDR", 0xedb9),
        std::make_pair("INDR", 0xedba),
        std::make_pair("OTDR", 0xedbb),
        std::make_pair("LDI", 0xeda0),
        std::make_pair("CPI", 0xeda1),
        std::make_pair("INI", 0xeda2),
        std::make_pair("OUTI", 0xeda3),
        std::make_pair("LDD", 0xeda8),
        std::make_pair("CPD", 0xeda9),
        std::make_pair("IND", 0xedaa),
        std::make_pair("OUTD", 0xedab),
    };

    skip_spaces();
    const char* p0 = p;

    int n = match_one_of(-1, single_opcode);
    if (n != -1 && parse_end_asm()) {
        if ((n & 0xff00) != 0) {
            asm_line.bytes.push_back((n >> 8) & 0xff);
        }
        asm_line.bytes.push_back(n & 0xff);
        return true;
    }

    p = p0;
    return false;
}

bool Parser::parse_ld(AsmLine& asm_line) {
    if (!match("LD")) {
        return false;
    }

    skip_spaces();
    const char* p0 = p;

    int r1, r2, pfx;
    Expr rpn1, rpn2;

    // ld (bc|de), a
    p = p0;
    if (match("(") && parse_bc_de(r1) && match(")")
            && match(",") && match("A") && parse_end_asm()) {
        asm_line.bytes.push_back(0x02 + 16 * r1);
        return true;
    }

    // ld a, (bc|de)
    p = p0;
    if (match("A") && match(",")
            && match("(") && parse_bc_de(r1) && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0x0a + 16 * r1);
        return true;
    }

    // ld r1, r2
    p = p0;
    if (parse_r1(r1) && match(",") && parse_r1(r2) && parse_end_asm()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld (hl), r
    p = p0;
    if (match("(") && match("HL") && match(")") && match(",") && parse_r1(r1)
            && parse_end_asm()) {
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        return true;
    }

    // ld r, (hl)
    p = p0;
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match(")")
            && parse_end_asm()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        return true;
    }

    // ld i, a
    p = p0;
    if (match("I") && match(",") && match("A") && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x47);
        return true;
    }

    // ld r, a
    p = p0;
    if (match("R") && match(",") && match("A") && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4f);
        return true;
    }

    // ld a, i
    p = p0;
    if (match("A") && match(",") && match("I") && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x57);
        return true;
    }

    // ld a, r
    p = p0;
    if (match("A") && match(",") && match("R") && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x5f);
        return true;
    }

    // ld (hl), N
    p = p0;
    if (match("(") && match("HL") && match(")")
            && match(",") && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

    // ld (x+d), r
    p = p0;
    if (parse_x_plus_ind(pfx, rpn1) && match(",") && parse_r1(r1)
            && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        add_sbyte_patch(asm_line, rpn1);
        return true;
    }

    // ld r, (x+d)
    p = p0;
    if (parse_r1(r1) && match(",") && parse_x_plus_ind(pfx, rpn1)
            && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

    // ld (x+d), N
    p = p0;
    if (parse_x_plus_ind(pfx, rpn1)
            && match(",") && parse_asm_expr(rpn2) && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_sbyte_patch(asm_line, rpn1);
        add_byte_patch(asm_line, rpn2);
        return true;
    }

    // ld sp, x
    p = p0;
    if (match("SP") && match(",") && parse_hl_ix_iy(pfx) && parse_end_asm()) {
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xf9);
        return true;
    }

    // ld dd, (NN)
    p = p0;
    if (parse_dd_sp(r1) && match(",")
            && match("(") && parse_asm_expr(rpn1) && match(")") && parse_end_asm()) {
        if (r1 == 2) {
            asm_line.bytes.push_back(0x2a);
        }
        else if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
            asm_line.bytes.push_back(0x2a);
        }
        else {
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x4b + 16 * (r1 & 0xff));
        }
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // ld dd, NN
    p = p0;
    if (parse_dd_sp(r1) && match(",") && parse_asm_expr(rpn1) && parse_end_asm()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
        }
        asm_line.bytes.push_back(0x01 + 16 * (r1 & 0xff));
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // ld (NN), dd
    p = p0;
    if (match("(") && parse_asm_expr(rpn1) && match(")")
            && match(",") && parse_dd_sp(r1) && parse_end_asm()) {
        if (r1 == 2) {
            asm_line.bytes.push_back(0x22);
        }
        else if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
            asm_line.bytes.push_back(0x22);
        }
        else {
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x43 + 16 * (r1 & 0xff));
        }
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // ld a, (NN)
    p = p0;
    if (match("A") && match(",")
            && match("(") && parse_asm_expr(rpn1) && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0x3a);
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // ld (NN), a
    p = p0;
    if (match("(") && parse_asm_expr(rpn1) && match(")")
            && match(",") && match("A") && parse_end_asm()) {
        asm_line.bytes.push_back(0x32);
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // ld r1, N
    p = p0;
    if (parse_r1(r1) && match(",") && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x06 + 8 * r1);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

    p = p0;
    return false;
}

bool Parser::parse_alu(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> alu1 = {
        std::make_pair("ADD", 0),
        std::make_pair("ADC", 1),
        std::make_pair("SUB", 2),
        std::make_pair("SBC", 3),
        std::make_pair("AND", 4),
        std::make_pair("XOR", 5),
        std::make_pair("OR", 6),
        std::make_pair("CP", 7),
    };

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1, r2, pfx;
    Expr rpn1;

    int op = match_one_of(-1, alu1);
    if (op == -1) {
        goto error;
    }

    p1 = p;

    // add hl|ix|iy, dd
    p = p1;
    if (op == 0 && parse_dd_sp(r1) && (r1 & 0xff) == 2 && match(",")
            && parse_dd_sp(r2) && parse_end_asm()) {
        if ((r2 & 0xff) == 2 && (r1 & 0xff00) != (r2 & 0xff00)) {
            goto error;
        }
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x09 + 16 * (r2 & 0xff));
        return true;
    }

    // adc hl, dd
    p = p1;
    if (op == 1 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4a + 16 * (r2 & 0xff));
        return true;
    }

    // sbc hl, dd
    p = p1;
    if (op == 3 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x42 + 16 * (r2 & 0xff));
        return true;
    }

    // {alu} a, r
    p = p1;
    if (match("A") && match(",") && parse_r1(r1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} r
    p = p1;
    if (parse_r1(r1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} a, (hl)
    p = p1;
    if (match("A") && match(",") && match("(") && match("HL") && match(")")
            && parse_end_asm()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} (hl)
    p = p1;
    if (match("(") && match("HL") && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} a, (x+DIS)
    p = p1;
    if (match("A") && match(",") && parse_x_plus_ind(pfx, rpn1)
            && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, rpn1);
        return true;
    }

    // {alu} (x+DIS)
    p = p1;
    if (parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, rpn1);
        return true;
    }

    // {alu} a, N
    p = p1;
    if (match("A") && match(",") && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

    // {alu} N
    p = p1;
    if (parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_ex(AsmLine& asm_line) {
    int pfx;

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;

    if (!match("EX")) {
        goto error;
    }

    p1 = p;

    // ex (sp), x
    p = p1;
    if (match("(") && match("SP") && match(")") && match(",")
            && parse_hl_ix_iy(pfx) && parse_end_asm()) {
        if (pfx) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe3);
        return true;
    }

    // ex af, af'
    p = p1;
    if (match("AF") && match(",") && match("AF") && match("'") && parse_end_asm()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex af, af
    p = p1;
    if (match("AF") && match(",") && match("AF") && parse_end_asm()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex de, hl
    p = p1;
    if (match("DE") && match(",") && match("HL") && parse_end_asm()) {
        asm_line.bytes.push_back(0xeb);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_bit(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> bit1 = {
        std::make_pair("BIT", 0),
        std::make_pair("RES", 1),
        std::make_pair("SET", 2),
    };

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1, pfx;
    Expr bit_num, rpn1;

    int op = match_one_of(-1, bit1);
    if (op == -1) {
        goto error;
    }

    p1 = p;

    // {bit} N, r
    p = p1;
    if (parse_asm_expr(bit_num) && match(",") && parse_r1(r1) && parse_end_asm()) {
        int bit = basic->eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            goto error;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + r1);
        return true;
    }

    // {bit} N, (hl)
    p = p1;
    if (parse_asm_expr(bit_num) && match(",") && match("(") && match("HL")
            && match(")") && parse_end_asm()) {
        int bit = basic->eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            goto error;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

    // {bit} N, (x+DIS)
    p = p1;
    if (parse_asm_expr(bit_num) && match(",")
            && parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
        int bit = basic->eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            goto error;
        }
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, rpn1);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_call_jp(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> jpcall1 = {
        std::make_pair("JP", 0),
        std::make_pair("CALL", 1),
    };

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int f1, pfx;
    Expr rpn1;

    int op = match_one_of(-1, jpcall1);
    if (op == -1) {
        goto error;
    }

    p1 = p;

    // jp (hl|ix|iy)
    p = p1;
    if (op == 0 && match("(") && parse_hl_ix_iy(pfx) && match(")")
            && parse_end_asm()) {
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe9);
        return true;
    }

    // jp|call flag, NN
    p = p1;
    if (parse_flags_jp(f1) && match(",") && parse_asm_expr(rpn1)
            && parse_end_asm()) {
        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * f1);
        add_word_patch(asm_line, rpn1);
        return true;
    }

    // jp|call NN
    p = p1;
    if (parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xc3 + 10 * op);
        add_word_patch(asm_line, rpn1);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_rst(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    Expr rpn1;

    if (match("RST") && parse_asm_expr(rpn1) && parse_end_asm()) {
        int n = basic->eval_const_expr(rpn1);
        switch (n) {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            asm_line.bytes.push_back(0xc7 + n);
            return true;
        }
    }

    p = p0;
    return false;
}

bool Parser::parse_ret(AsmLine& asm_line) {
    int f1;

    skip_spaces();
    const char* p0 = p;

    if (match("RET") && parse_flags_jp(f1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xc0 + 8 * f1);
        return true;
    }

    p = p0;
    return false;
}

bool Parser::parse_jr(AsmLine& asm_line) {
    Expr rpn1;
    int f1;

    skip_spaces();
    const char* p0 = p;

    // djnz NN
    p = p0;
    if (match("DJNZ") && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x10);
        add_jr_patch(asm_line, rpn1);
        return true;
    }

    // jr f, NN
    p = p0;
    if (match("JR") && parse_flags_jr(f1) && match(",")
            && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x20 + 8 * f1);
        add_jr_patch(asm_line, rpn1);
        return true;
    }

    // jr NN
    p = p0;
    if (match("JR") && parse_asm_expr(rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x18);
        add_jr_patch(asm_line, rpn1);
        return true;
    }

    return false;
}

bool Parser::parse_inc_dec(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> incdec1 = {
        std::make_pair("INC", 0),
        std::make_pair("DEC", 1),
    };

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1, pfx;
    Expr rpn1;

    int op = match_one_of(-1, incdec1);
    if (op == -1) {
        goto error;
    }

    p1 = p;

    // inc|dec dd
    p = p1;
    if (parse_dd_sp(r1) && parse_end_asm()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x03 + 8 * op + 16 * (r1 & 0xff));
        return true;
    }

    // inc|dec r
    p = p1;
    if (parse_r1(r1) && parse_end_asm()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * r1);
        return true;
    }

    // inc|dec (hl)
    p = p1;
    if (match("(") && match("HL") && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        return true;
    }

    // inc|dec (x+DIS)
    p = p1;
    if (parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        add_sbyte_patch(asm_line, rpn1);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_in(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1;
    Expr rpn1;

    if (!match("IN")) {
        goto error;
    }

    p1 = p;

    // in r, (c)
    p = p1;
    if (parse_r1(r1) && match(",")
            && match("(") && match("C") && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x40 + 8 * r1);
        return true;
    }

    // in a, (n)
    p = p1;
    if (match("A") && match(",")
            && match("(") && parse_asm_expr(rpn1) && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0xdb);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_out(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1;
    Expr rpn1;

    if (!match("OUT")) {
        goto error;
    }

    p1 = p;

    // out (c), r
    p = p1;
    if (match("(") && match("C") && match(")") &&
            match(",") && parse_r1(r1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x41 + 8 * r1);
        return true;
    }

    // out (n), a
    p = p1;
    if (match("(") && parse_asm_expr(rpn1) && match(")")
            && match(",") && match("A") && parse_end_asm()) {
        asm_line.bytes.push_back(0xd3);
        add_byte_patch(asm_line, rpn1);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_im(AsmLine& asm_line) {
    Expr rpn;

    skip_spaces();
    const char* p0 = p;

    if (match("IM") && parse_asm_expr(rpn) && parse_end_asm()) {
        int n = basic->eval_const_expr(rpn);
        switch (n) {
        case 0:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x46);
            return true;
        case 1:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x56);
            return true;
        case 2:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x5e);
            return true;
        default:
            ;
        }
    }

    p = p0;
    return false;
}

bool Parser::parse_push_pop(AsmLine& asm_line) {
    int r1;

    skip_spaces();
    const char* p0 = p;

    // push dd
    p = p0;
    if (match("PUSH") && parse_dd_af(r1) && parse_end_asm()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc5 + 16 * (r1 & 0xff));
        return true;
    }

    // pop dd
    p = p0;
    if (match("POP") && parse_dd_af(r1) && parse_end_asm()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc1 + 16 * (r1 & 0xff));
        return true;
    }

    return false;
}

bool Parser::parse_rotate(AsmLine& asm_line) {
    static std::vector<std::pair<std::string, int>> rot1 = {
        std::make_pair("RLC", 0),
        std::make_pair("RRC", 1),
        std::make_pair("RL", 2),
        std::make_pair("RR", 3),
        std::make_pair("SLA", 4),
        std::make_pair("SRA", 5),
        std::make_pair("SLL", 6),
        std::make_pair("SRL", 7),
    };

    skip_spaces();
    const char* p0 = p;
    const char* p1 = p;
    int r1, pfx;
    Expr rpn1;

    int op = match_one_of(-1, rot1);
    if (op == -1) {
        goto error;
    }

    p1 = p;

    // {rot} r
    p = p1;
    if (parse_r1(r1) && parse_end_asm()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + r1);
        return true;
    }

    // {rot} (hl)
    p = p1;
    if (match("(") && match("HL") && match(")") && parse_end_asm()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

    // {rot} (x+DIS)
    p = p1;
    if (parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, rpn1);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

error:
    p = p0;
    return false;
}

bool Parser::parse_dd_sp(int& n) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
        std::make_pair("HL", 2),
        std::make_pair("SP", 3),
        std::make_pair("IX", 0xdd02),
        std::make_pair("IY", 0xfd02),
    };
    n = match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_dd_af(int& n) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
        std::make_pair("HL", 2),
        std::make_pair("AF", 3),
        std::make_pair("IX", 0xdd02),
        std::make_pair("IY", 0xfd02),
    };
    n = match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_bc_de(int& n) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
    };
    n = match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_hl_ix_iy(int& pfx) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("HL", 0),
        std::make_pair("IX", 0xdd),
        std::make_pair("IY", 0xfd),
    };
    pfx = match_one_of(-1, regs);
    if (pfx != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_r1(int& n) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("B", 0),
        std::make_pair("C", 1),
        std::make_pair("D", 2),
        std::make_pair("E", 3),
        std::make_pair("H", 4),
        std::make_pair("L", 5),
        std::make_pair("A", 7),
    };
    n = match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_flags_jr(int& n) {
    static std::vector<std::pair<std::string, int>> flags = {
        std::make_pair("NZ", 0),
        std::make_pair("Z", 1),
        std::make_pair("NC", 2),
        std::make_pair("C", 3),
    };
    n = match_one_of(-1, flags);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_flags_jp(int& n) {
    static std::vector<std::pair<std::string, int>> flags = {
        std::make_pair("NZ", 0),
        std::make_pair("Z", 1),
        std::make_pair("NC", 2),
        std::make_pair("C", 3),
        std::make_pair("PO", 4),
        std::make_pair("PE", 5),
        std::make_pair("P", 6),
        std::make_pair("M", 7),
    };
    n = match_one_of(-1, flags);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_x_plus_ind(int& pfx, Expr& offset) {
    static std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("IX", 0xdd),
        std::make_pair("IY", 0xfd),
    };

    skip_spaces();
    const char* p0 = p;
    if (!match("(")) {
        p = p0;
        return false;
    }

    pfx = match_one_of(-1, regs);
    if (pfx == -1) {
        p = p0;
        return false;
    }

    if (match(")")) {			// no offset
        Token token;
        token.code = T_integer;
        token.ivalue = 0;
        offset.push_back(token);
        return true;
    }
    else if (parse_asm_expr(offset) && match(")")) {
        return true;
    }
    else {
        p = p0;
        return false;
    }
}

void Parser::add_sbyte_patch(AsmLine& asm_line, const Expr& rpn) {
    Patch patch;
    patch.rpn = rpn;
    patch.type = Patch::Type::SByte;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}

void Parser::add_byte_patch(AsmLine& asm_line, const Expr& rpn) {
    Patch patch;
    patch.rpn = rpn;
    patch.type = Patch::Type::Byte;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}

void Parser::add_word_patch(AsmLine& asm_line, const Expr& rpn) {
    Patch patch;
    patch.rpn = rpn;
    patch.type = Patch::Type::Word;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
    asm_line.bytes.push_back(0);
}

void Parser::add_jr_patch(AsmLine& asm_line, const Expr& rpn) {
    Patch patch;
    patch.rpn = rpn;
    patch.type = Patch::Type::JrOffset;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}

void parse_b81_file(Basic& basic, const std::string& filename) {
    g_parser.parse_b81_file(basic, filename);
}
