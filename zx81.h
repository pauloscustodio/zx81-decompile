//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
using namespace std;

#define NUM_ELEMS(a)	(sizeof(a) / sizeof(a[0]))

string decode_zx81(char c);
vector<uint8_t> encode_zx81(const char*& p);

enum ZX81char {
	T_none = -1,
	T_number = -2,
	T_string = -3,
	T_ident = -4,
	T_rem_code = -5,
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
	int		code{ T_none };
	double	num{ 0.0 };			// for T_number
	string  str;				// for T_string
	string	ident;				// for T_ident
	vector<uint8_t> bytes;		// for T_code
};

struct BasicLine {
	int addr{ 0 };
	int line_num{ 0 };
	int size{ 0 };
	string label;
	vector<Token> tokens;
};

struct BasicVar {
	enum class Type { Number, ArrayNumbers, ForNextLoop, String, ArrayStrings };
	Type type{ Type::Number};

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

struct ZX81 {
	ZX81();

	static inline const int ERR_NO = 0x4000;
	static inline const int FLAGS = 0x4001;
	static inline const int ERR_SP = 0x4002;
	static inline const int RAMTOP = 0x4004;
	static inline const int MODE = 0x4006;
	static inline const int PPC = 0x4007;
	static inline const int VERSN = 0x4009;
	static inline const int E_PPC = 0x400a;
	static inline const int D_FILE = 0x400c;
	static inline const int DF_CC = 0x400e;
	static inline const int VARS = 0x4010;
	static inline const int DEST = 0x4012;
	static inline const int E_LINE = 0x4014;
	static inline const int CH_ADD = 0x4016;
	static inline const int X_PTR = 0x4018;
	static inline const int STKBOT = 0x401a;
	static inline const int STKEND = 0x401c;
	static inline const int BREG = 0x401e;
	static inline const int MEM = 0x401f;
	static inline const int FREE1 = 0x4021;
	static inline const int DF_SZ = 0x4022;
	static inline const int S_TOP = 0x4023;
	static inline const int LAST_K = 0x4025;
	static inline const int DEBOUNCE = 0x4027;
	static inline const int MARGIN = 0x4028;
	static inline const int NXTLIN = 0x4029;
	static inline const int OLDPPC = 0x402b;
	static inline const int FLAGX = 0x402d;
	static inline const int STRLEN = 0x402e;
	static inline const int T_ADDR = 0x4030;
	static inline const int SEED = 0x4032;
	static inline const int FRAMES = 0x4034;
	static inline const int COORDS_X = 0x4036;
	static inline const int COORDS_Y = 0x4037;
	static inline const int PR_CC = 0x4038;
	static inline const int S_POSN_COL = 0x4039;
	static inline const int S_POSN_ROW = 0x403a;
	static inline const int CDFLAG = 0x403b;
	static inline const int PRBUFF = 0x403c;
	static inline const int MEMBOT = 0x405d;
	static inline const int FREE2 = 0x407b;

	static inline const int BaseAddr = VERSN;
	static inline const int BaseProg = FREE2 + 2;
	static inline const int NumLines = 24;
	static inline const int NumCols = 32;

	vector<uint8_t> mem_bytes;

	int sv_versn{ 0 };
	int sv_e_ppc{ 0 };
	int sv_d_file{ 0 };
	int sv_df_cc{ 0 };
	int sv_vars{ 0 };
	int sv_dest{ 0 };
	int sv_e_line{ 0 };
	int sv_ch_add{ 0 };
	int sv_x_ptr{ 0 };
	int sv_stkbot{ 0 };
	int sv_stkend{ 0 };
	int sv_breg{ 0 };
	int sv_mem{ 0 };
	int sv_free1{ 0 };
	int sv_df_sz{ 0 };
	int sv_s_top{ 0 };
	int sv_last_k{ 0 };
	int sv_debounce{ 0 };
	int sv_margin{ 0 };
	int sv_nxtlin{ 0 };
	int sv_oldppc{ 0 };
	int sv_flagx{ 0 };
	int sv_strlen{ 0 };
	int sv_t_addr{ 0 };
	int sv_seed{ 0 };
	int sv_frames{ 0 };
	int sv_coords_x{ 0 };
	int sv_coords_y{ 0 };
	int sv_pr_cc{ 0 };
	int sv_s_posn_col{ 0 };
	int sv_s_posn_row{ 0 };
	int sv_cdflag{ 0 };
	array<uint8_t, 33> sv_prbuff{ 0 };
	array<uint8_t, 30> sv_membot{ 0 };
	int sv_free2{ 0 };

	vector<BasicLine> basic_lines;
	vector<string> d_file;
	vector<BasicVar> basic_vars;

	void read_p(const string& p_filename);
	void write_p(const string& p_filename);
	void parse_b81(const string& b81_filename);
	void write_b81(const string& b81_filename);

	void decompile();		// convert bytes to memory structure
	void compile();			// convert memory structure to bytes

	int peek(int addr);
	int dpeek(int addr);
	int dpeek_be(int addr);

	void poke(int addr, int value);
	void dpoke(int addr, int value);
	void dpoke_be(int addr, int value);

private:
	int addr{ 0 };
	int end{ 0 };
	int input_line_num{ 0 };
	int auto_increment{ 10 };

	void decompile_sysvars();
	void decompile_basic();
	void decompile_video();
	void decompile_vars();
	void decode_basic_line(BasicLine& line);
	bool decode_rem_code(BasicLine& line);
	double decode_fp_number(int addr);
	bool decode_number(BasicLine& line);
	bool decode_ident(BasicLine& line);
	bool decode_string(BasicLine& line);
	void decode_newline(BasicLine& line);

	void parse_line(const char* p);
	void parse_meta_line(const char* p);
	void parse_basic_line(const char* p);
	void parse_basic_var(const char* p);

	void compile_sysvars();
	void compile_basic();
	void compile_video();
	void compile_vars();

	void write_basic_lines(ofstream& ofs);
	void write_basic_vars(ofstream& ofs);
	void write_sysvars(ofstream& ofs);
};

