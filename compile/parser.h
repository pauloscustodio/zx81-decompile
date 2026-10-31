//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "errors.h"
#include "expr.h"
#include "scan.h"
#include "types.h"
#include <string>

class Parser {
public:
    explicit Parser(Basic* basic_)
        : basic(basic_) {};

    void parse_bas_file(const std::string& filename);

private:
    Basic* basic = nullptr;
    bool in_asm = false;
    Scanner scan;
    SourceLoc loc;
    int last_line_num = -1;

    // parser
    void parse_line();

    // meta parser
    void parse_pragma_line();
    bool parse_basic_var(BasicVar& var);
    Bytes parse_byte_list();

    // basic parser
    bool parse_basic_line(SourceLine& source_line);
    bool parse_ref(const std::string& prefix, std::string& ident);
    bool parse_line_num_ref(std::string& ident);
    bool parse_line_addr_ref(std::string& ident);
    bool parse_label(std::string& ident);
    void parse_label_line_num(SourceLine& source_line);

    // expression parser
    bool parse_primary_expr(Expr& expr);
    bool parse_unary_expr(Expr& expr);
    bool parse_mult_expr(Expr& expr);
    bool parse_add_expr(Expr& expr);
    bool parse_shift_expr(Expr& expr);
    bool parse_rel_expr(Expr& expr);
    bool parse_eq_expr(Expr& expr);
    bool parse_bin_and_expr(Expr& expr);
    bool parse_bin_xor_expr(Expr& expr);
    bool parse_bin_or_expr(Expr& expr);
    bool parse_log_and_expr(Expr& expr);
    bool parse_log_or_expr(Expr& expr);
    bool parse_conditional_expr(Expr& expr);
    bool parse_expr(Expr& expr);

    // assembler parser
    bool parse_asm_line(AsmLine& asm_line);
    bool parse_asm_label(std::string& ident);
    bool parse_defb(AsmLine& asm__line);
    bool parse_defw(AsmLine& asm__line);
    bool parse_defs(AsmLine& asm__line);
    bool parse_z80_opcode(AsmLine& asm__line);
    bool parse_single_opcode(AsmLine& asm_line);
    bool parse_ld(AsmLine& asm_line);
    bool parse_alu(AsmLine& asm_line);
    bool parse_ex(AsmLine& asm_line);
    bool parse_bit(AsmLine& asm_line);
    bool parse_call_jp(AsmLine& asm_line);
    bool parse_rst(AsmLine& asm_line);
    bool parse_ret(AsmLine& asm_line);
    bool parse_jr(AsmLine& asm_line);
    bool parse_inc_dec(AsmLine& asm_line);
    bool parse_in(AsmLine& asm_line);
    bool parse_out(AsmLine& asm_line);
    bool parse_im(AsmLine& asm_line);
    bool parse_push_pop(AsmLine& asm_line);
    bool parse_rotate(AsmLine& asm_line);
    bool parse_dd_sp(int& n);
    bool parse_dd_af(int& n);
    bool parse_bc_de(int& n);
    bool parse_hl_ix_iy(int& pfx);
    bool parse_r1(int& n);
    bool parse_flags_jr(int& n);
    bool parse_flags_jp(int& n);
    bool parse_x_plus_ind(int& pfx, Expr& offset);
    void add_sbyte_patch(AsmLine& asm_line, const Expr& expr);
    void add_byte_patch(AsmLine& asm_line, const Expr& expr);
    void add_word_patch(AsmLine& asm_line, const Expr& expr);
    void add_jr_patch(AsmLine& asm_line, const Expr& expr);
};
