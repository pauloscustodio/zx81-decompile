//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "encode.h"
#include "errors.h"
#include "memory.h"
#include "parser.h"
#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

static Parser g_parser;

void Scanner::skip_spaces() {
	while (*p != '\0' && isspace(*p))
		p++;
}

bool Scanner::at_end(char comment_char) {
	skip_spaces();
	if (*p == '\0' || *p == comment_char)
		return true;
	else
		return false;
}

bool Scanner::parse_integer(int& value) {
	skip_spaces();
	const char* p0 = p;

	// $hhhh
	p = p0;
	if (*p == '$') {	
		if (!isxdigit(p[1]))
			return false;
		p++;
		while (isxdigit(*p))
			p++;
		value = INT(strtol(p0 + 1, NULL, 16));
		return true;
	}

	// dhhhH
	p = p0;
	if (isdigit(*p)) {
		while (isxdigit(*p))
			p++;
		if (toupper(*p) == 'H') {
			p++;
			value = INT(strtol(p0, NULL, 16));
			return true;
		}
	}

	// decimal
	p = p0;
	if (isdigit(*p)) {
		while (isdigit(*p))
			p++;
		value = atoi(p0);
		return true;
	}

	p = p0;
	return false;
}

bool Scanner::parse_number(double& value, string& value_text) {
	skip_spaces();
	const char* p0 = p;

	// collect mantissa
	int num_dots = 0;
	int num_digits = 0;
	while (*p != '\0') {
		if (*p == '.') {
			p++;
			num_dots++;
			if (num_dots > 1) {
				p = p0;
				return false;
			}
		}
		else if (isdigit(*p)) {
			p++;
			num_digits++;
		}
		else
			break;
	}
	if (num_digits == 0) {
		p = p0;
		return false;
	}

	// collect exponent
	if (toupper(*p) == 'E') {
		p++;
		if (*p == '-' || *p == '+')
			p++;
		int exp = 0;
		if (!parse_integer(exp)) {
			p = p0;
			return false;
		}
	}

	value = atof(p0);
	value_text = string(p0, p);
	return true;
}

bool Scanner::parse_string(string& str) {
	string out;
	skip_spaces();
	const char* p0 = p;

	if (*p != '"')
		return false;
	p++;
	while (*p != '\0' && *p != '"') {
		if (*p == '\\') {
			out.push_back(*p++);
			out.push_back(*p++);
		}
		else
			out.push_back(*p++);
	}
	if (*p != '"') {
		p = p0;
		return false;
	}
	else {
		p++;
		str = out;
		return true;
	}
}

bool Scanner::parse_ident(string& ident) {
	skip_spaces();
	if (!isalpha(*p) && *p != '_')
		return false;
	while (isalnum(*p) || *p == '_')
		ident.push_back(*p++);
	return true;
}

bool Scanner::match(const string& compare) {
	skip_spaces();
	for (size_t i = 0; i < compare.size(); i++) {
		if (p[i] == '\0')
			return false;
		if (toupper(p[i]) != toupper(compare[i]))
			return false;
	}
	p += compare.size();
	return true;
}

int Scanner::match_one_of(int not_found_result, vector<pair<string, int>> compare_list) {
	skip_spaces();
	for (auto& compare : compare_list) {
		if (match(compare.first))
			return compare.second;
	}
	return not_found_result;
}

void Parser::parse_b81_file(Basic& basic_, const string& filename) {
	basic = &basic_;
	basic->clear();
	in_asm = false;

	ifstream ifs(filename);
	if (!ifs.is_open()) {
		perror(filename.c_str());
		fatal_error("read file", filename);
	}

	err_set_filename(filename);

	string text;
	int line_num = 0;
	while (getline(ifs, text)) {
		line_num++;
		err_set_line_num(line_num);

		while (!text.empty() && text.back() == '\\') {
			text.pop_back();
			text.push_back(' ');
			string cont;
			if (!getline(ifs, cont))
				break;

			line_num++;
			err_set_line_num(line_num);
			text += cont;
		}

		p = text.c_str();
		parse_line();
	}

	err_clear();
	basic = nullptr;
}

bool Parser::parse_ref(const string& prefix, string& ident) {
	skip_spaces();
	const char* p0 = p;
	if (match(prefix) && parse_ident(ident))
		return true;
	else {
		p = p0;
		return false;
	}
}

bool Parser::parse_line_num_ref(string& ident) {
	return parse_ref("@", ident);
}

bool Parser::parse_line_addr_ref(string& ident) {
	return parse_ref("&", ident);
}

bool Parser::parse_label(string& ident) {
	skip_spaces();
	const char* p0 = p;
	if (parse_line_num_ref(ident) && match(":"))
		return true;
	else {
		p = p0;
		return false;
	}
}

void Parser::parse_label_line_num(BasicLine& basic_line) {
	// get label and/or line number
	bool got_line_num = false;
	bool got_label = false;
	bool found_some = false;
	do {
		found_some = false;
		if (!got_line_num && parse_integer(basic_line.line_num)) {
			got_line_num = true;
			found_some = true;
		}
		if (!got_label && parse_label(basic_line.label)) {
			got_label = true;
			found_some = true;
		}
	} while (found_some);
}

bool Parser::parse_end_basic() {
	return at_end('#');
}

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
	if (parse_expr(in_asm, rpn))
		return true;
	else {
		p = p0;
		rpn.clear();
		return false;
	}
}

bool Parser::parse_expr(bool in_asm, Expr& rpn) {
	if (!parse_term(in_asm, rpn))
		return false;
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
	if (!parse_factor(in_asm, rpn))
		return false;
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
	else if (parse_number(token.fvalue, token.svalue) && floor(token.fvalue) == token.fvalue) {
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

bool Parser::parse_basic_line(BasicLine& basic_line) {
	static vector<pair<string, int>> keywords = {
	make_pair("RND", C_RND),
	make_pair("INKEY$", C_INKEY_dollar),
	make_pair("PI", C_PI),
	make_pair("AT", C_AT),
	make_pair("TAB", C_TAB),
	make_pair("CODE", C_CODE),
	make_pair("VAL", C_VAL),
	make_pair("LEN", C_LEN),
	make_pair("SIN", C_SIN),
	make_pair("COS", C_COS),
	make_pair("TAN", C_TAN),
	make_pair("ASN", C_ASN),
	make_pair("ACS", C_ACS),
	make_pair("ATN", C_ATN),
	make_pair("LN", C_LN),
	make_pair("EXP", C_EXP),
	make_pair("INT", C_INT),
	make_pair("SQR", C_SQR),
	make_pair("SGN", C_SGN),
	make_pair("ABS", C_ABS),
	make_pair("PEEK", C_PEEK),
	make_pair("USR", C_USR),
	make_pair("STR$", C_STR_dollar),
	make_pair("CHR$", C_CHR_dollar),
	make_pair("NOT", C_NOT),
	make_pair("**", C_power),
	make_pair("OR", C_OR),
	make_pair("AND", C_AND),
	make_pair("<=", C_le),
	make_pair(">=", C_ge),
	make_pair("<>", C_ne),
	make_pair("THEN", C_THEN),
	make_pair("TO", C_TO),
	make_pair("STEP", C_STEP),
	make_pair("LPRINT", C_LPRINT),
	make_pair("LLIST", C_LLIST),
	make_pair("STOP", C_STOP),
	make_pair("SLOW", C_SLOW),
	make_pair("FAST", C_FAST),
	make_pair("NEW", C_NEW),
	make_pair("SCROLL", C_SCROLL),
	make_pair("CONT", C_CONT),
	make_pair("DIM", C_DIM),
	make_pair("FOR", C_FOR),
	make_pair("GOTO", C_GOTO),
	make_pair("GOSUB", C_GOSUB),
	make_pair("INPUT", C_INPUT),
	make_pair("LOAD", C_LOAD),
	make_pair("LIST", C_LIST),
	make_pair("LET", C_LET),
	make_pair("PAUSE", C_PAUSE),
	make_pair("NEXT", C_NEXT),
	make_pair("POKE", C_POKE),
	make_pair("PRINT", C_PRINT),
	make_pair("PLOT", C_PLOT),
	make_pair("RUN", C_RUN),
	make_pair("SAVE", C_SAVE),
	make_pair("RAND", C_RAND),
	make_pair("IF", C_IF),
	make_pair("CLS", C_CLS),
	make_pair("UNPLOT", C_UNPLOT),
	make_pair("CLEAR", C_CLEAR),
	make_pair("RETURN", C_RETURN),
	make_pair("COPY", C_COPY),
	make_pair("\\0c", C_pound),
	make_pair("$", C_dollar),
	make_pair(":", C_colon),
	make_pair("?", C_quest),
	make_pair("(", C_lparens),
	make_pair(")", C_rparens),
	make_pair(">", C_gt),
	make_pair("<", C_lt),
	make_pair("=", C_eq),
	make_pair("+", C_plus),
	make_pair("-", C_minus),
	make_pair("*", C_mult),
	make_pair("/", C_div),
	make_pair(";", C_semicolon),
	make_pair(",", C_comma),
	make_pair("_", C_space),
	};

	basic_line = BasicLine();

	// get label and/or line number
	parse_label_line_num(basic_line);

	// get tokens
	skip_spaces();
	while (!parse_end_basic()) {
		Token token;
		int n = match_one_of(-1, keywords);
		if (n != -1)
			token.code = static_cast<ZX81char>(n);
		else if (match("REM")) {
			token.code = C_REM;
			basic_line.tokens.push_back(token);

			skip_spaces();
			token.code = T_rem_code;
			token.bytes = encode_zx81(p);
		}
		else if (parse_basic_expr(token.rpn))
			token.code = T_const_expr;
		else if (parse_number(token.fvalue, token.svalue))
			token.code = T_float;
		else if (parse_string(token.svalue))
			token.code = T_string;
		else if (parse_ident(token.ident))
			token.code = T_ident;
		else {
			error("cannot parse", p);
			break;
		}

		basic_line.tokens.push_back(token);
		skip_spaces();
	}

	Token token;
	token.code = C_newline;
	basic_line.tokens.push_back(token);

	return true;
}

bool Parser::parse_basic_var(BasicVar& var) {
	var = BasicVar();

	bool is_string = false;
	bool is_array = false;
	int num_elements = 0;

	// get name
	if (!parse_ident(var.name))
		goto error;

	// get string marker
	if (match("$")) {
		is_string = true;
		if (var.name.size() > 1) {
			error("name too long", var.name);
			return false;
		}
	}

	// get array marker and dimensions
	if (match("(")) {
		is_array = true;
		if (var.name.size() > 1) {
			error("name too long", var.name);
			return false;
		}

		num_elements = 1;
		do {
			int dimension = 0;
			if (!parse_integer(dimension))
				goto error;
			num_elements *= dimension;
			var.dimensions.push_back(dimension);
		} while (match(","));

		if (!match(")"))
			goto error;
	}

	// get =
	if (!match("="))
		goto error;

	// get value
	if (is_string == false && is_array == false) {
		string str;

		var.type = BasicVar::Type::Number;
		if (!parse_number(var.value, str))
			goto error;

		if (match(",")) {
			var.type = BasicVar::Type::ForNextLoop;
			if (var.name.size() > 1) {
				error("name too long", var.name);
				return false;
			}

			if (!parse_number(var.limit, str))
				goto error;
			if (!match(","))
				goto error;
			if (!parse_number(var.step, str))
				goto error;
			if (!match(","))
				goto error;
			if (!parse_integer(var.line_num))
				goto error;
			if (!parse_end_basic())
				goto error;
		}
	}
	else if (is_string == false && is_array == true) {
		double value = 0.0;
		string str;

		var.type = BasicVar::Type::ArrayNumbers;

		for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
			if (i > 0) {
				if (!match(","))
					goto error;
			}
			if (!parse_number(value, str))
				goto error;
			var.values.push_back(value);
		}
		if (!parse_end_basic())
			goto error;
	}
	else if (is_string == true && is_array == false) {
		var.type = BasicVar::Type::String;

		if (!parse_string(var.str))
			goto error;
		if (!parse_end_basic())
			goto error;
	}
	else if (is_string == true && is_array == true) {
		string str;

		var.type = BasicVar::Type::ArrayStrings;

		int last_dimension = var.dimensions.back();
		num_elements /= last_dimension;

		for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
			if (i > 0) {
				if (!match(","))
					goto error;
			}
			if (!parse_string(str))
				goto error;
			if (str.size() != static_cast<size_t>(last_dimension)) {
				error("string length should be " + to_string(last_dimension));
				return false;
			}
			var.strs.push_back(str);
		}
		if (!parse_end_basic())
			goto error;
	}

	return true;

error:
	error("cannot parse", p);
	return false;
}

void Parser::parse_meta_line() {
	int n = 0;
	if (match("VARS")) {
		BasicVar basic_var;
		if (parse_basic_var(basic_var))
			basic->basic_vars.push_back(basic_var);
	}
	else if (match("SYSVARS") && match("=")) {
		poke_bytes(VERSN, encode_zx81(p));
	}
	else if (match("D_FILE") && match("=")) {
		basic->d_file_bytes = encode_zx81(p);
	}
	else if (match("WORKSPACE") && match("=")) {
		basic->e_line_bytes = encode_zx81(p);
	}
	else if (match("AUTOSTART") && match("=") && parse_integer(n) && parse_end_basic()) {
		basic->autostart = n;
	}
	else if (match("FAST") && match("=") && parse_integer(n) && parse_end_basic()) {
		basic->fast = n ? true : false;
	}
	else if (match("INCREMENT") && match("=") && parse_integer(n) && parse_end_basic()) {
		basic->auto_increment = n;
	}
	else if (match("ASM")) {
		in_asm = true;
	}
	else if (match("ENDASM")) {
		in_asm = false;
	}
	else {
		// ignore, consider a comment
	}
}

bool Parser::parse_asm_line(AsmLine& asm_line) {
	if (parse_asm_label(asm_line.label)) {
		// is it EQU?
		skip_spaces();
		const char* p0 = p;
		if (match("EQU")) {
			if (parse_asm_expr(asm_line.equ_rpn) && parse_end_asm())
				return true;
			else
				return false;
		}
		p = p0;
	}
	
	if (parse_defb(asm_line) && parse_end_asm())
		return true;
	else if (parse_defw(asm_line) && parse_end_asm())
		return true;
	else if (parse_defm(asm_line) && parse_end_asm())
		return true;
	else if (parse_defs(asm_line) && parse_end_asm())
		return true;
	else if (parse_z80_opcode(asm_line))
		return true;
	else if (parse_end_asm())
		return true;
	else {
		error("cannot parse", p);
		return false;
	}
}

bool Parser::parse_asm_label(string& ident) {
	if (isalpha(*p) || *p == '_') {		// identifier at column 0
		parse_ident(ident);
		skip_spaces();
		match(":");						// optional colon
		return true;
	}
	else
		return false;
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
			if (!parse_asm_expr(rpn))
				return false;
			add_byte_patch(asm_line, rpn);

			if (match(","))
				continue;
			else
				break;
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
			if (!parse_asm_expr(rpn))
				return false;
			add_word_patch(asm_line, rpn);

			if (match(","))
				continue;
			else
				break;
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
			string str;
			if (!parse_string(str))
				return false;

			Bytes bytes = encode_zx81(str);
			for (auto& b : bytes)
				asm_line.bytes.push_back(b);

			if (match(","))
				continue;
			else
				break;
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
			for (int i = 0; i < space; i++)
				asm_line.bytes.push_back(0);
			return true;
		}
	}

	p = p0;
	return false;
}

bool Parser::parse_z80_opcode(AsmLine& asm_line) {
	if (parse_end_asm())
		return true;
	else if (parse_single_opcode(asm_line))
		return true;
	else if (parse_ld(asm_line))
		return true;
	else if (parse_alu(asm_line))
		return true;
	else if (parse_ex(asm_line))
		return true;
	else if (parse_bit(asm_line))
		return true;
	else if (parse_call_jp(asm_line))
		return true;
	else if (parse_rst(asm_line))
		return true;
	else if (parse_ret(asm_line))
		return true;
	else if (parse_jr(asm_line))
		return true;
	else if (parse_inc_dec(asm_line))
		return true;
	else if (parse_in(asm_line))
		return true;
	else if (parse_out(asm_line))
		return true;
	else if (parse_im(asm_line))
		return true;
	else if (parse_push_pop(asm_line))
		return true;
	else if (parse_rotate(asm_line))
		return true;
	else
		return false;
}

bool Parser::parse_single_opcode(AsmLine& asm_line) {
	static vector<pair<string, int>> single_opcode = {
		make_pair("NOP", 0x00),
		make_pair("RLCA", 0x07),
		make_pair("RRCA", 0x0f),
		make_pair("RLA", 0x17),
		make_pair("RRA", 0x1f),
		make_pair("DAA", 0x27),
		make_pair("CPL", 0x2f),
		make_pair("SCF", 0x37),
		make_pair("CCF", 0x3f),
		make_pair("HALT", 0x76),
		make_pair("RETN", 0xed45),
		make_pair("RETI", 0xed4d),
		make_pair("RET", 0xc9),
		make_pair("EXX", 0xd9),
		make_pair("DI", 0xf3),
		make_pair("EI", 0xfb),
		make_pair("NEG", 0xed44),
		make_pair("RRD", 0xed67),
		make_pair("RLD", 0xed6f),
		make_pair("LDIR", 0xedb0),
		make_pair("CPIR", 0xedb1),
		make_pair("INIR", 0xedb2),
		make_pair("OTIR", 0xedb3),
		make_pair("LDDR", 0xedb8),
		make_pair("CPDR", 0xedb9),
		make_pair("INDR", 0xedba),
		make_pair("OTDR", 0xedbb),
		make_pair("LDI", 0xeda0),
		make_pair("CPI", 0xeda1),
		make_pair("INI", 0xeda2),
		make_pair("OUTI", 0xeda3),
		make_pair("LDD", 0xeda8),
		make_pair("CPD", 0xeda9),
		make_pair("IND", 0xedaa),
		make_pair("OUTD", 0xedab),
	};

	skip_spaces();
	const char* p0 = p;

	int n = match_one_of(-1, single_opcode);
	if (n != -1 && parse_end_asm()) {
		if ((n & 0xff00) != 0)
			asm_line.bytes.push_back((n >> 8) & 0xff);
		asm_line.bytes.push_back(n & 0xff);
		return true;
	}

	p = p0;
	return false;
}

bool Parser::parse_ld(AsmLine& asm_line) {
	if (!match("LD"))
		return false;

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
	if (match("(") && match("HL") && match(")") && match(",") && parse_r1(r1) && parse_end_asm()) {
		asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
		return true;
	}

	// ld r, (hl)
	p = p0;
	if (parse_r1(r1) && match(",") && match("(") && match("HL") && match(")") && parse_end_asm()) {
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
	if (parse_x_plus_ind(pfx, rpn1) && match(",") && parse_r1(r1) && parse_end_asm()) {
		asm_line.bytes.push_back(pfx);
		asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
		add_sbyte_patch(asm_line, rpn1);
		return true;
	}

	// ld r, (x+d)
	p = p0;
	if (parse_r1(r1) && match(",") && parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
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
		if (pfx != 0)
			asm_line.bytes.push_back(pfx);
		asm_line.bytes.push_back(0xf9);
		return true;
	}

	// ld dd, (NN)
	p = p0;
	if (parse_dd_sp(r1) && match(",")
		&& match("(") && parse_asm_expr(rpn1) && match(")") && parse_end_asm()) {
		if (r1 == 2)
			asm_line.bytes.push_back(0x2a);
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
		if ((r1 & 0xff00) != 0)
			asm_line.bytes.push_back(r1 >> 8);
		asm_line.bytes.push_back(0x01 + 16 * (r1 & 0xff));
		add_word_patch(asm_line, rpn1);
		return true;
	}

	// ld (NN), dd
	p = p0;
	if (match("(") && parse_asm_expr(rpn1) && match(")")
		&& match(",") && parse_dd_sp(r1) && parse_end_asm()) {
		if (r1 == 2)
			asm_line.bytes.push_back(0x22);
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
	static vector<pair<string, int>> alu1 = {
		make_pair("ADD", 0),
		make_pair("ADC", 1),
		make_pair("SUB", 2),
		make_pair("SBC", 3),
		make_pair("AND", 4),
		make_pair("XOR", 5),
		make_pair("OR", 6),
		make_pair("CP", 7),
	};

	skip_spaces();
	const char* p0 = p;
	const char* p1 = p;
	int r1, r2, pfx;
	Expr rpn1;

	int op = match_one_of(-1, alu1);
	if (op == -1)
		goto error;

	p1 = p;

	// add hl|ix|iy, dd
	p = p1;
	if (op == 0 && parse_dd_sp(r1) && (r1 & 0xff) == 2 && match(",") && parse_dd_sp(r2) && parse_end_asm()) {
		if ((r2 & 0xff) == 2 && (r1 & 0xff00) != (r2 & 0xff00))
			goto error;
		if ((r1 & 0xff00) != 0)
			asm_line.bytes.push_back((r1 >> 8) & 0xff);
		asm_line.bytes.push_back(0x09 + 16 * (r2 & 0xff));
		return true;
	}

	// adc hl, dd
	p = p1;
	if (op == 1 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2) && (r2 & 0xff00) == 0 && parse_end_asm()) {
		asm_line.bytes.push_back(0xed);
		asm_line.bytes.push_back(0x4a + 16 * (r2 & 0xff));
		return true;
	}

	// sbc hl, dd
	p = p1;
	if (op == 3 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2) && (r2 & 0xff00) == 0 && parse_end_asm()) {
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
	if (match("A") && match(",") && match("(") && match("HL") && match(")") && parse_end_asm()) {
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
	if (match("A") && match(",") && parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
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

	if (!match("EX"))
		goto error;

	p1 = p;

	// ex (sp), x
	p = p1;
	if (match("(") && match("SP") && match(")") && match(",") 
		&& parse_hl_ix_iy(pfx) && parse_end_asm()) {
		if (pfx)
			asm_line.bytes.push_back(pfx);
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
	static vector<pair<string, int>> bit1 = {
		make_pair("BIT", 0),
		make_pair("RES", 1),
		make_pair("SET", 2),
	};

	skip_spaces();
	const char* p0 = p;
	const char* p1 = p;
	int r1, pfx;
	Expr bit_num, rpn1;

	int op = match_one_of(-1, bit1);
	if (op == -1)
		goto error;

	p1 = p;

	// {bit} N, r
	p = p1;
	if (parse_asm_expr(bit_num) && match(",") && parse_r1(r1) && parse_end_asm()) {
		int bit = basic->eval_const_expr(bit_num);
		if (bit < 0 || bit > 7)
			goto error;
		asm_line.bytes.push_back(0xcb);
		asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + r1);
		return true;
	}

	// {bit} N, (hl)
	p = p1;
	if (parse_asm_expr(bit_num) && match(",") && match("(") && match("HL") && match(")") && parse_end_asm()) {
		int bit = basic->eval_const_expr(bit_num);
		if (bit < 0 || bit > 7)
			goto error;
		asm_line.bytes.push_back(0xcb);
		asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
		return true;
	}

	// {bit} N, (x+DIS)
	p = p1;
	if (parse_asm_expr(bit_num) && match(",") 
		&& parse_x_plus_ind(pfx, rpn1) && parse_end_asm()) {
		int bit = basic->eval_const_expr(bit_num);
		if (bit < 0 || bit > 7)
			goto error;
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
	static vector<pair<string, int>> jpcall1 = {
	make_pair("JP", 0),
	make_pair("CALL", 1),
	};

	skip_spaces();
	const char* p0 = p;
	const char* p1 = p;
	int f1, pfx;
	Expr rpn1;

	int op = match_one_of(-1, jpcall1);
	if (op == -1)
		goto error;

	p1 = p;

	// jp (hl|ix|iy)
	p = p1;
	if (op == 0 && match("(") && parse_hl_ix_iy(pfx) && match(")") && parse_end_asm()) {
		if (pfx != 0)
			asm_line.bytes.push_back(pfx);
		asm_line.bytes.push_back(0xe9);
		return true;
	}

	// jp|call flag, NN
	p = p1;
	if (parse_flags_jp(f1) && match(",") && parse_asm_expr(rpn1) && parse_end_asm()) {
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
		case 0x00: case 0x08: case 0x10: case 0x18: case 0x20: case 0x28: case 0x30: case 0x38:
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
	static vector<pair<string, int>> incdec1 = {
	make_pair("INC", 0),
	make_pair("DEC", 1),
	};

	skip_spaces();
	const char* p0 = p;
	const char* p1 = p;
	int r1, pfx;
	Expr rpn1;

	int op = match_one_of(-1, incdec1);
	if (op == -1)
		goto error;

	p1 = p;

	// inc|dec dd
	p = p1;
	if (parse_dd_sp(r1) && parse_end_asm()) {
		if ((r1 & 0xff00) != 0)
			asm_line.bytes.push_back((r1 >> 8) & 0xff);
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

	if (!match("IN"))
		goto error;

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

	if (!match("OUT"))
		goto error;

	p1 = p;

	// out (c), r
	p = p1;
	if (match("(") && match("C") && match(")") && 
		match(",")&& parse_r1(r1) && parse_end_asm()) {
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
		default:;
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
		if ((r1 & 0xff00) != 0)
			asm_line.bytes.push_back((r1 >> 8) & 0xff);
		asm_line.bytes.push_back(0xc5 + 16 * (r1 & 0xff));
		return true;
	}

	// pop dd
	p = p0;
	if (match("POP") && parse_dd_af(r1) && parse_end_asm()) {
		if ((r1 & 0xff00) != 0)
			asm_line.bytes.push_back((r1 >> 8) & 0xff);
		asm_line.bytes.push_back(0xc1 + 16 * (r1 & 0xff));
		return true;
	}

	return false;
}

bool Parser::parse_rotate(AsmLine& asm_line) {
	static vector<pair<string, int>> rot1 = {
		make_pair("RLC", 0),
		make_pair("RRC", 1),
		make_pair("RL", 2),
		make_pair("RR", 3),
		make_pair("SLA", 4),
		make_pair("SRA", 5),
		make_pair("SLL", 6),
		make_pair("SRL", 7),
	};

	skip_spaces();
	const char* p0 = p;
	const char* p1 = p;
	int r1, pfx;
	Expr rpn1;

	int op = match_one_of(-1, rot1);
	if (op == -1)
		goto error;

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
	static vector<pair<string, int>> regs = {
		make_pair("BC", 0),
		make_pair("DE", 1),
		make_pair("HL", 2),
		make_pair("SP", 3),
		make_pair("IX", 0xdd02),
		make_pair("IY", 0xfd02),
	};
	n = match_one_of(-1, regs);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_dd_af(int& n) {
	static vector<pair<string, int>> regs = {
		make_pair("BC", 0),
		make_pair("DE", 1),
		make_pair("HL", 2),
		make_pair("AF", 3),
		make_pair("IX", 0xdd02),
		make_pair("IY", 0xfd02),
	};
	n = match_one_of(-1, regs);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_bc_de(int& n) {
	static vector<pair<string, int>> regs = {
		make_pair("BC", 0),
		make_pair("DE", 1),
	};
	n = match_one_of(-1, regs);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_hl_ix_iy(int& pfx) {
	static vector<pair<string, int>> regs = {
		make_pair("HL", 0),
		make_pair("IX", 0xdd),
		make_pair("IY", 0xfd),
	};
	pfx = match_one_of(-1, regs);
	if (pfx != -1)
		return true;
	else
		return false;
}

bool Parser::parse_r1(int& n) {
	static vector<pair<string, int>> regs = {
		make_pair("B", 0),
		make_pair("C", 1),
		make_pair("D", 2),
		make_pair("E", 3),
		make_pair("H", 4),
		make_pair("L", 5),
		make_pair("A", 7),
	};
	n = match_one_of(-1, regs);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_flags_jr(int& n) {
	static vector<pair<string, int>> flags = {
		make_pair("NZ", 0),
		make_pair("Z", 1),
		make_pair("NC", 2),
		make_pair("C", 3),
	};
	n = match_one_of(-1, flags);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_flags_jp(int& n) {
	static vector<pair<string, int>> flags = {
		make_pair("NZ", 0),
		make_pair("Z", 1),
		make_pair("NC", 2),
		make_pair("C", 3),
		make_pair("PO", 4),
		make_pair("PE", 5),
		make_pair("P", 6),
		make_pair("M", 7),
	};
	n = match_one_of(-1, flags);
	if (n != -1)
		return true;
	else
		return false;
}

bool Parser::parse_x_plus_ind(int& pfx, Expr& offset) {
	static vector<pair<string, int>> regs = {
		make_pair("IX", 0xdd),
		make_pair("IY", 0xfd),
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
	else if (parse_asm_expr(offset) && match(")"))
		return true;
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

void Parser::parse_line() {
	const char* p0 = p;
	skip_spaces();
	if (*p == '\0') {
	}
	else if (*p == '#') {
		p++;
		parse_meta_line();
	}
	else if (in_asm) {
		p = p0;

		SourceLine source_line(SourceLine::Type::Asm);
		if (parse_asm_line(source_line.asm_line))
			basic->source_lines.push_back(source_line);
	}
	else {
		SourceLine source_line(SourceLine::Type::Basic);
		if (parse_basic_line(source_line.basic_line)) 
			basic->source_lines.push_back(source_line);
	}
}

void parse_b81_file(Basic& basic, const string& filename) {
	g_parser.parse_b81_file(basic, filename);
}
