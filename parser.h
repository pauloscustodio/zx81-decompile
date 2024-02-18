//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include <string>
using namespace std;

struct Parser {
	void parse_b81_file(Basic& basic, const string& filename);

private:
	Basic* basic{ nullptr };
	bool in_asm{ false };
	const char* p{ nullptr };

	// scanner
	void skip_spaces();
	bool at_end(char comment_char = '\0');
	bool parse_integer(int& value);
	bool parse_number(double& value, string& value_text);
	bool parse_string(string& str);
	bool parse_ident(string& ident);
	bool match(const string& compare);
	int match_one_of(int not_found_result, vector<pair<string, int>> compare_list);

	// basic parser
	bool parse_ref(const string& prefix, string& ident);
	bool parse_line_num_ref(string& ident);
	bool parse_line_addr_ref(string& ident);
	bool parse_label(string& ident);
	void parse_label_line_num(BasicLine& basic_line);
	bool parse_end_basic();
	bool parse_asm_expr(Expr& rpn);
	bool parse_basic_expr(Expr& rpn);
	bool parse_const_expr(bool in_asm, Expr& rpn);
	bool parse_expr(bool in_asm, Expr& rpn);
	bool parse_term(bool in_asm, Expr& rpn);
	bool parse_factor(bool in_asm, Expr& rpn);
	bool parse_basic_line(BasicLine& basic_line);
	bool parse_basic_var(BasicVar& var);

	// meta parser
	void parse_meta_line();

	// assembler parser
	bool parse_asm_line(AsmLine& asm_line);
	bool parse_asm_label(string& ident);
	bool parse_end_asm();
	bool parse_defb(AsmLine& asm_line);
	bool parse_defw(AsmLine& asm_line);
	bool parse_defm(AsmLine& asm_line);
	bool parse_defs(AsmLine& asm_line);
	bool parse_z80_opcode(AsmLine& asm_line);
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
	void add_sbyte_patch(AsmLine& asm_line, const Expr& rpn);
	void add_byte_patch(AsmLine& asm_line, const Expr& rpn);
	void add_word_patch(AsmLine& asm_line, const Expr& rpn);
	void add_jr_patch(AsmLine& asm_line, const Expr& rpn);

	// parser
	void parse_line();
};

void parse_b81_file(Basic& basic, const string& filename);

