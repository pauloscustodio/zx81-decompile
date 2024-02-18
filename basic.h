//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include <string>
#include <unordered_map>
using namespace std;

enum ZX81char {
	T_none = 0x100,
	T_integer,
	T_float,
	T_string,
	T_ident,
	T_rem_code,
	T_line_addr_ref,
	T_line_num_ref,
	T_const_expr,
	T_unary_minus,
	T_ASMPC,

	C_space = 0x00,
	C_gr_wwww = 0x00,
	C_gr_bwww = 0x01,
	C_gr_wbww = 0x02,
	C_gr_bbww = 0x03,
	C_gr_wwbw = 0x04,
	C_gr_bwbw = 0x05,
	C_gr_wbbw = 0x06,
	C_gr_bbbw = 0x07,
	C_gr_gggg = 0x08,
	C_gr_wwgg = 0x09,
	C_gr_ggww = 0x0a,
	C_dquote = 0x0b,
	C_pound = 0x0c,
	C_dollar = 0x0d,
	C_colon = 0x0e,
	C_quest = 0x0f,
	C_lparens = 0x10,
	C_rparens = 0x11,
	C_gt = 0x12,
	C_lt = 0x13,
	C_eq = 0x14,
	C_plus = 0x15,
	C_minus = 0x16,
	C_mult = 0x17,
	C_div = 0x18,
	C_semicolon = 0x19,
	C_comma = 0x1a,
	C_dot = 0x1b,
	C_0 = 0x1c,
	C_1 = 0x1d,
	C_2 = 0x1e,
	C_3 = 0x1f,
	C_4 = 0x20,
	C_5 = 0x21,
	C_6 = 0x22,
	C_7 = 0x23,
	C_8 = 0x24,
	C_9 = 0x25,
	C_A = 0x26,
	C_B = 0x27,
	C_C = 0x28,
	C_D = 0x29,
	C_E = 0x2a,
	C_F = 0x2b,
	C_G = 0x2c,
	C_H = 0x2d,
	C_I = 0x2e,
	C_J = 0x2f,
	C_K = 0x30,
	C_L = 0x31,
	C_M = 0x32,
	C_N = 0x33,
	C_O = 0x34,
	C_P = 0x35,
	C_Q = 0x36,
	C_R = 0x37,
	C_S = 0x38,
	C_T = 0x39,
	C_U = 0x3a,
	C_V = 0x3b,
	C_W = 0x3c,
	C_X = 0x3d,
	C_Y = 0x3e,
	C_Z = 0x3f,
	C_RND = 0x40,
	C_INKEY_dollar = 0x41,
	C_PI = 0x42,
	C_up = 0x70,
	C_down = 0x71,
	C_left = 0x72,
	C_right = 0x73,
	C_graphics = 0x74,
	C_edit = 0x75,
	C_newline = 0x76,
	C_rubout = 0x77,
	C_KL_mode = 0x78,
	C_function = 0x79,
	C_number = 0x7e,
	C_cursor = 0x7f,
	C_inv_space = 0x80,
	C_gr_bbbb = 0x80,
	C_grwbbb = 0x81,
	C_gr_bwbb = 0x82,
	C_gr_wwbb = 0x83,
	C_gr_bbwb = 0x84,
	C_gr_wbwb = 0x85,
	C_gr_bwwb = 0x86,
	C_gr_wwwb = 0x87,
	c_gr_GGGG = 0x88,
	C_gr_bbGG = 0x89,
	C_gr_GGbb = 0x8a,
	C_inv_dquote = 0x8b,
	C_inv_pound = 0x8c,
	C_inv_dollar = 0x8d,
	C_inv_colon = 0x8e,
	C_inv_quest = 0x8f,
	C_inv_lparen = 0x90,
	C_inv_rparen = 0x91,
	C_inv_gt = 0x92,
	C_inv_lt = 0x93,
	C_inv_eq = 0x94,
	C_inv_plus = 0x95,
	C_inv_minus = 0x96,
	C_inv_mult = 0x97,
	C_inv_div = 0x98,
	C_inv_semicolon = 0x99,
	C_inv_comma = 0x9a,
	C_inv_dot = 0x9b,
	C_inv_0 = 0x9c,
	C_inv_1 = 0x9d,
	C_inv_2 = 0x9e,
	C_inv_3 = 0x9f,
	C_inv_4 = 0xa0,
	C_inv_5 = 0xa1,
	C_inv_6 = 0xa2,
	C_inv_7 = 0xa3,
	C_inv_8 = 0xa4,
	C_inv_9 = 0xa5,
	C_inv_A = 0xa6,
	C_inv_B = 0xa7,
	C_inv_C = 0xa8,
	C_inv_D = 0xa9,
	C_inv_E = 0xaa,
	C_inv_F = 0xab,
	C_inv_G = 0xac,
	C_inv_H = 0xad,
	C_inv_I = 0xae,
	C_inv_J = 0xaf,
	C_inv_K = 0xb0,
	C_inv_L = 0xb1,
	C_inv_M = 0xb2,
	C_inv_N = 0xb3,
	C_inv_O = 0xb4,
	C_inv_P = 0xb5,
	C_inv_Q = 0xb6,
	C_inv_R = 0xb7,
	C_inv_S = 0xb8,
	C_inv_T = 0xb9,
	C_inv_U = 0xba,
	C_inv_V = 0xbb,
	C_inv_W = 0xbc,
	C_inv_X = 0xbd,
	C_inv_Y = 0xbe,
	C_inv_Z = 0xbf,
	C_ddquote = 0xc0,
	C_AT = 0xc1,
	C_TAB = 0xc2,
	C_CODE = 0xc4,
	C_VAL = 0xc5,
	C_LEN = 0xc6,
	C_SIN = 0xc7,
	C_COS = 0xc8,
	C_TAN = 0xc9,
	C_ASN = 0xca,
	C_ACS = 0xcb,
	C_ATN = 0xcc,
	C_LN = 0xcd,
	C_EXP = 0xce,
	C_INT = 0xcf,
	C_SQR = 0xd0,
	C_SGN = 0xd1,
	C_ABS = 0xd2,
	C_PEEK = 0xd3,
	C_USR = 0xd4,
	C_STR_dollar = 0xd5,
	C_CHR_dollar = 0xd6,
	C_NOT = 0xd7,
	C_power = 0xd8,
	C_OR = 0xd9,
	C_AND = 0xda,
	C_le = 0xdb,
	C_ge = 0xdc,
	C_ne = 0xdd,
	C_THEN = 0xde,
	C_TO = 0xdf,
	C_STEP = 0xe0,
	C_LPRINT = 0xe1,
	C_LLIST = 0xe2,
	C_STOP = 0xe3,
	C_SLOW = 0xe4,
	C_FAST = 0xe5,
	C_NEW = 0xe6,
	C_SCROLL = 0xe7,
	C_CONT = 0xe8,
	C_DIM = 0xe9,
	C_REM = 0xea,
	C_FOR = 0xeb,
	C_GOTO = 0xec,
	C_GOSUB = 0xed,
	C_INPUT = 0xee,
	C_LOAD = 0xef,
	C_LIST = 0xf0,
	C_LET = 0xf1,
	C_PAUSE = 0xf2,
	C_NEXT = 0xf3,
	C_POKE = 0xf4,
	C_PRINT = 0xf5,
	C_PLOT = 0xf6,
	C_RUN = 0xf7,
	C_SAVE = 0xf8,
	C_RAND = 0xf9,
	C_IF = 0xfa,
	C_CLS = 0xfb,
	C_UNPLOT = 0xfc,
	C_CLEAR = 0xfd,
	C_RETURN = 0xfe,
	C_COPY = 0xff,
};

struct Token {
	ZX81char code{ T_none };
	int		ivalue{ 0 };		// for T_integer
	double	fvalue{ 0.0 };		// for T_float
	string  svalue;				// for T_string
	string	ident;				// for T_ident, T_line_addr, T_line_num
	Bytes	bytes;				// for T_rem_code
	vector<Token> rpn;			// Expr for T_const_expr
};

typedef vector<Token> Expr;

struct Symbol {
	enum class Type { Const, Addr };
	Type type{ Type::Const };
	string name;
	int value;

	Symbol(Type type_ = Type::Const, const string& name_ = "", int value_ = 0);
};

class Symtab {
public:
	Symtab();
	virtual ~Symtab();
	Symtab(const Symtab& other) = delete;
	Symtab& operator=(const Symtab& other) = delete;

	void init();

	void add(Symbol::Type type, const string& name, int value);
	void update(Symbol::Type type, const string& name, int value);

	Symbol* get(const string& name);	// nullptr if not found
	Symbol* find(int value);			// last defined symbol with this value

	auto begin() { return symbols.begin(); }
	auto end() { return symbols.end(); }
	auto cbegin() const { return symbols.cbegin(); }
	auto cend() const { return symbols.cend(); }
	auto size() const { return symbols.size(); }

private:
	unordered_map<string, Symbol*> symbols;
};

struct Patch {
	enum class Type { Byte, Word, SByte, JrOffset };
	Type type{ Type::Byte };
	int offset{ 0 };
	Expr rpn;

	Patch(Type type_ = Type::Byte) : type(type_) {}
};

struct AsmLine {
	string source_filename;
	int source_line_num;
	string label;
	Expr equ_rpn;			// if equ
	Bytes bytes;
	vector<Patch> patches;

	AsmLine();
};

struct BasicLine {
	string source_filename;
	int source_line_num;
	string label;
	int line_num{ -1 };
	vector<Token> tokens;

	BasicLine();
};

struct SourceLine {
	enum class Type { Basic, Asm };
	Type type{ Type::Basic };

	int addr{ 0 };
	BasicLine basic_line;
	AsmLine asm_line;

	SourceLine(Type type_ = Type::Basic) : type(type_) {}
};

struct BasicVar {
	enum class Type { Number, ArrayNumbers, ForNextLoop, String, ArrayStrings };
	Type type{ Type::Number };
	int addr{ 0 };
	int size{ 0 };

	// Number
	string name;
	double value{ 0.0 };

	// ForNextLoop
	double limit{ 0.0 };
	double step{ 0.0 };
	int line_num{ 0 };

	// ArrayNumbers
	vector<int> dimensions;
	vector<double> values;

	// String
	string str;

	// ArrayStrings
	vector<string> strs;
};

struct Basic {
	Basic();
	void clear();

	vector<SourceLine> source_lines;
	vector<BasicVar> basic_vars;
	Symtab symtab;
	Bytes d_file_bytes;
	Bytes e_line_bytes;
	int autostart;
	int auto_increment;
	bool fast;

	int eval_const_expr(const Expr& rpn);
	bool eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error = true);
	void parse_b81_file(const string& filename);
	void write_b81_file(const string& filename);

private:
	void write_sysvars(ofstream& ofs) const;
	void write_basic_lines(ofstream& ofs) const;
	void write_video(ofstream& ofs) const;
	void write_basic_vars(ofstream& ofs) const;
	void write_basic_system(ofstream& ofs) const;
	void write_mem_info(ofstream& ofs, int start_addr, int len) const;
	void write_basic_memory_map(ofstream& ofs) const;
};
