//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "getopt.h"
#include "utils.h"
#include "zfloat.h"
#include "zx81.h"
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;

static const char* zx81_chars[] = {
/* 0x00 */	" ",			
/* 0x01 */	"\\' ",		
/* 0x02 */	"\\ '",		
/* 0x03 */	"\\''",		
/* 0x04 */	"\\. ",		
/* 0x05 */	"\\: ",		
/* 0x06 */	"\\.'",		
/* 0x07 */	"\\:'",		
/* 0x08 */	"\\##",		
/* 0x09 */	"\\,,",		
/* 0x0a */	"\\~~",		
/* 0x0b */	"\"",			
/* 0x0c */	"£",		
/* 0x0d */	"$",			
/* 0x0e */	":",			
/* 0x0f */	"?",			
/* 0x10 */	"(",			
/* 0x11 */	")",			
/* 0x12 */	">",			
/* 0x13 */	"<",			
/* 0x14 */	"=",			
/* 0x15 */	"+",			
/* 0x16 */	"-",			
/* 0x17 */	"*",			
/* 0x18 */	"/",			
/* 0x19 */	";",			
/* 0x1a */	",",			
/* 0x1b */	".",			
/* 0x1c */	"0",			
/* 0x1d */	"1",			
/* 0x1e */	"2",			
/* 0x1f */	"3",			
/* 0x20 */	"4",			
/* 0x21 */	"5",			
/* 0x22 */	"6",			
/* 0x23 */	"7",			
/* 0x24 */	"8",			
/* 0x25 */	"9",			
/* 0x26 */	"A",			
/* 0x27 */	"B",			
/* 0x28 */	"C",			
/* 0x29 */	"D",			
/* 0x2a */	"E",			
/* 0x2b */	"F",			
/* 0x2c */	"G",			
/* 0x2d */	"H",			
/* 0x2e */	"I",			
/* 0x2f */	"J",			
/* 0x30 */	"K",			
/* 0x31 */	"L",			
/* 0x32 */	"M",			
/* 0x33 */	"N",			
/* 0x34 */	"O",			
/* 0x35 */	"P",			
/* 0x36 */	"Q",			
/* 0x37 */	"R",			
/* 0x38 */	"S",			
/* 0x39 */	"T",			
/* 0x3a */	"U",			
/* 0x3b */	"V",			
/* 0x3c */	"W",			
/* 0x3d */	"X",			
/* 0x3e */	"Y",			
/* 0x3f */	"Z",			
/* 0x40 */	"RND",			
/* 0x41 */	"INKEY$",		
/* 0x42 */	"PI",			
/* 0x43 */	"\\43",			
/* 0x44 */	"\\44",			
/* 0x45 */	"\\45",			
/* 0x46 */	"\\46",			
/* 0x47 */	"\\47",			
/* 0x48 */	"\\48",			
/* 0x49 */	"\\49",			
/* 0x4a */	"\\4a",			
/* 0x4b */	"\\4b",			
/* 0x4c */	"\\4c",			
/* 0x4d */	"\\4d",			
/* 0x4e */	"\\4e",			
/* 0x4f */	"\\4f",			
/* 0x50 */	"\\50",			
/* 0x51 */	"\\51",			
/* 0x52 */	"\\52",			
/* 0x53 */	"\\53",			
/* 0x54 */	"\\54",			
/* 0x55 */	"\\55",			
/* 0x56 */	"\\56",			
/* 0x57 */	"\\57",			
/* 0x58 */	"\\58",			
/* 0x59 */	"\\59",			
/* 0x5a */	"\\5a",			
/* 0x5b */	"\\5b",			
/* 0x5c */	"\\5c",			
/* 0x5d */	"\\5d",			
/* 0x5e */	"\\5e",			
/* 0x5f */	"\\5f",			
/* 0x60 */	"\\60",			
/* 0x61 */	"\\61",			
/* 0x62 */	"\\62",			
/* 0x63 */	"\\63",			
/* 0x64 */	"\\64",			
/* 0x65 */	"\\65",			
/* 0x66 */	"\\66",			
/* 0x67 */	"\\67",			
/* 0x68 */	"\\68",			
/* 0x69 */	"\\69",			
/* 0x6a */	"\\6a",			
/* 0x6b */	"\\6b",			
/* 0x6c */	"\\6c",			
/* 0x6d */	"\\6d",			
/* 0x6e */	"\\6e",			
/* 0x6f */	"\\6f",			
/* 0x70 */	"\\70",			
/* 0x71 */	"\\71",		
/* 0x72 */	"\\72",		
/* 0x73 */	"\\73",		
/* 0x74 */	"\\74",	
/* 0x75 */	"\\75",		
/* 0x76 */	"\n",			
/* 0x77 */	"\\77",		
/* 0x78 */	"\\78",	
/* 0x79 */	"\\79",	
/* 0x7a */	"\\7a",			
/* 0x7b */	"\\7b",			
/* 0x7c */	"\\7c",			
/* 0x7d */	"\\7d",			
/* 0x7e */	"\\7e",		
/* 0x7f */	"\\7f",		
/* 0x80 */	"\\::",		
/* 0x81 */	"\\.:",		
/* 0x82 */	"\\:.",		
/* 0x83 */	"\\..",		
/* 0x84 */	"\\':",		
/* 0x85 */	"\\ :",		
/* 0x86 */	"\\'.",		
/* 0x87 */	"\\ .",		
/* 0x88 */	"\\@@",		
/* 0x89 */	"\\;;",		
/* 0x8a */	"\\!!",		
/* 0x8b */	"%\"",			
/* 0x8c */	"%£",	
/* 0x8d */	"%$",			
/* 0x8e */	"%:",			
/* 0x8f */	"%?",			
/* 0x90 */	"%(",			
/* 0x91 */	"%)",			
/* 0x92 */	"%>",			
/* 0x93 */	"%<",			
/* 0x94 */	"%=",			
/* 0x95 */	"%+",			
/* 0x96 */	"%-",			
/* 0x97 */	"%*",			
/* 0x98 */	"%/",			
/* 0x99 */	"%;",			
/* 0x9a */	"%,",			
/* 0x9b */	"%.",			
/* 0x9c */	"%0",			
/* 0x9d */	"%1",			
/* 0x9e */	"%2",			
/* 0x9f */	"%3",			
/* 0xa0 */	"%4",			
/* 0xa1 */	"%5",			
/* 0xa2 */	"%6",			
/* 0xa3 */	"%7",			
/* 0xa4 */	"%8",			
/* 0xa5 */	"%9",			
/* 0xa6 */	"%A",			
/* 0xa7 */	"%w",			
/* 0xa8 */	"%C",			
/* 0xa9 */	"%D",			
/* 0xaa */	"%E",			
/* 0xab */	"%F",			
/* 0xac */	"%G",			
/* 0xad */	"%H",			
/* 0xae */	"%I",			
/* 0xaf */	"%J",			
/* 0xb0 */	"%K",			
/* 0xb1 */	"%L",			
/* 0xb2 */	"%M",			
/* 0xb3 */	"%N",			
/* 0xb4 */	"%O",			
/* 0xb5 */	"%P",			
/* 0xb6 */	"%Q",			
/* 0xb7 */	"%R",			
/* 0xb8 */	"%S",			
/* 0xb9 */	"%T",			
/* 0xba */	"%U",			
/* 0xbb */	"%V",			
/* 0xbc */	"%W",			
/* 0xbd */	"%X",			
/* 0xbe */	"%Y",			
/* 0xbf */	"%Z",			
/* 0xc0 */	"\\\"",
/* 0xc1 */	"AT",			
/* 0xc2 */	"TAB",			
/* 0xc3 */	"\\c3",			
/* 0xc4 */	"CODE",			
/* 0xc5 */	"VAL",			
/* 0xc6 */	"LEN",			
/* 0xc7 */	"SIN",			
/* 0xc8 */	"COS",			
/* 0xc9 */	"TAN",			
/* 0xca */	"ASN",			
/* 0xcb */	"ACS",			
/* 0xcc */	"ATN",			
/* 0xcd */	"LN",			
/* 0xce */	"EXP",			
/* 0xcf */	"INT",			
/* 0xd0 */	"SQR",			
/* 0xd1 */	"SGN",			
/* 0xd2 */	"ABS",			
/* 0xd3 */	"PEEK",			
/* 0xd4 */	"USR",			
/* 0xd5 */	"STR$",			
/* 0xd6 */	"CHR$",			
/* 0xd7 */	"NOT",			
/* 0xd8 */	"**",			
/* 0xd9 */	"OR",			
/* 0xda */	"AND",			
/* 0xdb */	"<=",			
/* 0xdc */	">=",			
/* 0xdd */	"<>",			
/* 0xde */	"THEN",			
/* 0xdf */	"TO",			
/* 0xe0 */	"STEP",			
/* 0xe1 */	"LPRINT",		
/* 0xe2 */	"LLIST",		
/* 0xe3 */	"STOP",			
/* 0xe4 */	"SLOW",			
/* 0xe5 */	"FAST",			
/* 0xe6 */	"NEW",			
/* 0xe7 */	"SCROLL",		
/* 0xe8 */	"CONT",			
/* 0xe9 */	"DIM",			
/* 0xea */	"REM",			
/* 0xeb */	"FOR",			
/* 0xec */	"GOTO",			
/* 0xed */	"GOSUB",		
/* 0xee */	"INPUT",		
/* 0xef */	"LOAD",			
/* 0xf0 */	"LIST",			
/* 0xf1 */	"LET",			
/* 0xf2 */	"PAUSE",		
/* 0xf3 */	"NEXT",			
/* 0xf4 */	"POKE",			
/* 0xf5 */	"PRINT",		
/* 0xf6 */	"PLOT",			
/* 0xf7 */	"RUN",			
/* 0xf8 */	"SAVE",			
/* 0xf9 */	"RAND",			
/* 0xfa */	"IF",			
/* 0xfb */	"CLS",			
/* 0xfc */	"UNPLOT",		
/* 0xfd */	"CLEAR",		
/* 0xfe */	"RETURN",		
/* 0xff */	"COPY",			
};

static void skip_spaces(const char*& p) {
	while (*p != '\0' && isspace(*p))
		p++;
}

static bool match(const char*& p, const string& compare) {
	skip_spaces(p);
	for (size_t i = 0; i < compare.size(); i++) {
		if (p[i] == '\0')
			return false;
		if (toupper(p[i]) != toupper(compare[i]))
			return false;
	}
	p += compare.size();
	return true;
}

static bool parse_integer(const char*& p, int& value) {
	skip_spaces(p);
	const char* p0 = p;
	if (*p == '$') {	// hex
		if (!isxdigit(p[1]))
			return false;
		p++;
		while (isxdigit(*p))
			p++;
		value = static_cast<int>(strtol(p0 + 1, NULL, 16));
		return true;
	}
	else {				// decimal
		if (!isdigit(*p))
			return false;
		while (isdigit(*p))
			p++;
		value = atoi(p0);
		return true;
	}
}

static bool parse_number(const char*& p, double& value, string& value_text) {
	skip_spaces(p);
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
		if (!parse_integer(p, exp)) {
			p = p0;
			return false;
		}
	}

	value = atof(p0);
	value_text = string(p0, p);
	return true;
}

static bool parse_string(const char*& p, string& str) {
	string out;
	skip_spaces(p);
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
	if (*p != '"')
		return false;
	else {
		p++;
		str = out;
		return true;
	}
}

static bool parse_ident(const char*& p, string& ident) {
	skip_spaces(p);
	if (!isalpha(*p))
		return false;
	while (isalnum(*p)) 
		ident.push_back(*p++);
	return true;
}

static bool parse_end(const char*& p) {
	skip_spaces(p);
	if (*p == '\0' || *p == '#')
		return true;
	else
		return false;
}

string decode_zx81(char c) {
	string code = zx81_chars[c & 0xff];
	if (code.size() > 1 && isalnum(code.front()))
		code = string(" ") + code + " ";
	return code;
}

vector<uint8_t> encode_zx81(const char*& p) {
	vector<uint8_t> bytes;
	while (*p != '\0') {
		if (p[0] == '\\' && isxdigit(p[1]) && isxdigit(p[2])) {
			bytes.push_back(strtol(p + 1, NULL, 16) & 0xff);
			p += 3;
		}
		else {
			bool encoded = false;
			for (size_t i = 0; i < NUM_ELEMS(zx81_chars); i++) {
				bool found = true;
				for (size_t j = 0; j < strlen(zx81_chars[i]); j++) {
					if (toupper(p[j]) != toupper(zx81_chars[i][j])) {
						found = false;
						break;
					}
				}
				if (found) {
					bytes.push_back(i & 0xff);
					encoded = true;
					p += strlen(zx81_chars[i]);
					break;
				}
			}
			if (!encoded) {
				ERROR("cannot encode: " << p);
				break;
			}
		}
	}
	return bytes;
}

ZX81::ZX81() {
	d_file.resize(NumLines);
	for (auto& line : d_file) {
		line = string(NumCols, ' ');
		line.push_back('\n');
	}
	for (size_t i = 0; i < sv_prbuff.size() - 1; i++)
		sv_prbuff[i] = C_space;
	sv_prbuff.back() = C_newline;
}

void ZX81::read_p(const string& p_filename) {
	// open file
	ifstream ifs(p_filename, ios::binary);
	if (!ifs.is_open()) {
		perror(p_filename.c_str());
		FATAL_ERROR("open file " << p_filename);
	}

	// get size
	ifs.seekg(0, ios::end);
	size_t size = ifs.tellg();
	ifs.seekg(0, ios::beg);

	// resize buffer
	mem_bytes.resize(size);

	// read bytes
	ifs.read(reinterpret_cast<char*>(mem_bytes.data()), size);
	size_t readed = ifs.gcount();
	if (readed != size) {
		perror(p_filename.c_str());
		FATAL_ERROR("read " << size << " bytes from file " << p_filename);
	}
}

void ZX81::write_p(const string& p_filename) {
	// open file
	ofstream ofs(p_filename, ios::binary);
	if (!ofs.is_open()) {
		perror(p_filename.c_str());
		FATAL_ERROR("open file " << p_filename);
	}

	size_t size = mem_bytes.size();
	ofs.write(reinterpret_cast<char*>(mem_bytes.data()), size);
	size_t written = ofs.tellp();
	if (written != size) {
		perror(p_filename.c_str());
		FATAL_ERROR("write " << size << " bytes to file " << p_filename);
	}
}

void ZX81::decompile() {
	decompile_sysvars();
	decompile_basic();
	decompile_video();
	decompile_vars();
}

void ZX81::compile() {
	mem_bytes.clear();
	mem_bytes.resize(BaseProg - BaseAddr);

	compile_basic();
	compile_video();
	compile_vars();
	compile_sysvars();
}

int ZX81::peek(int addr) {
	size_t idx = static_cast<size_t>(addr - BaseAddr);
	if (idx >= mem_bytes.size()) {
		ERROR("address out of bounds: $" << fmt_hex(addr, 4));
		return 0;
	}
	else
		return mem_bytes[idx] & 0xff;
}

int ZX81::dpeek(int addr) {
	return peek(addr) + (peek(addr + 1) << 8);
}

int ZX81::dpeek_be(int addr) {
	return (peek(addr) << 8) + peek(addr + 1);
}

void ZX81::poke(int addr, int value) {
	size_t idx = static_cast<size_t>(addr - BaseAddr);
	if (idx >= mem_bytes.size()) 
		ERROR("address out of bounds: $" << fmt_hex(addr, 4));
	mem_bytes[idx] = value & 0xff;
}

void ZX81::dpoke(int addr, int value) {
	poke(addr, value);
	poke(addr + 1, value >> 8);
}

void ZX81::dpoke_be(int addr, int value) {
	poke(addr, value >> 8);
	poke(addr + 1, value);
}

void ZX81::decompile_sysvars() {
	sv_versn = peek(VERSN);
	sv_e_ppc = dpeek(E_PPC);
	sv_d_file = dpeek(D_FILE);
	sv_df_cc = dpeek(DF_CC);
	sv_vars = dpeek(VARS);
	sv_dest = dpeek(DEST);
	sv_e_line = dpeek(E_LINE);
	sv_ch_add = dpeek(CH_ADD);
	sv_x_ptr = dpeek(X_PTR);
	sv_stkbot = dpeek(STKBOT);
	sv_stkend = dpeek(STKEND);
	sv_breg = peek(BREG);
	sv_mem = dpeek(MEM);
	sv_free1 = peek(FREE1);
	sv_df_sz = peek(DF_SZ);
	sv_s_top = dpeek(S_TOP);
	sv_last_k = dpeek(LAST_K);
	sv_debounce = peek(DEBOUNCE);
	sv_margin = peek(MARGIN);
	sv_nxtlin = dpeek(NXTLIN);
	sv_oldppc = dpeek(OLDPPC);
	sv_flagx = peek(FLAGX);
	sv_strlen = dpeek(STRLEN);
	sv_t_addr = dpeek(T_ADDR);
	sv_seed = dpeek(SEED);
	sv_frames = dpeek(FRAMES);
	sv_coords_x = peek(COORDS_X);
	sv_coords_y = peek(COORDS_Y);
	sv_pr_cc = peek(PR_CC);
	sv_s_posn_col = peek(S_POSN_COL);
	sv_s_posn_row = peek(S_POSN_ROW);
	sv_cdflag = peek(CDFLAG);
	for (size_t i = 0; i < sv_prbuff.size(); i++)
		sv_prbuff[i] = peek(PRBUFF + static_cast<int>(i));
	for (size_t i = 0; i < sv_membot.size(); i++)
		sv_membot[i] = peek(MEMBOT + static_cast<int>(i));
	sv_free2 = dpeek(FREE2);
}

void ZX81::decompile_basic() {
	basic_lines.clear();
	addr = BaseProg;
	while (addr < sv_d_file) {
		BasicLine line;
		line.addr = addr;
		line.line_num = dpeek_be(addr);
		line.size = dpeek(addr + 2);

		addr += 4;
		end = addr + line.size;
		
		decode_basic_line(line);
		basic_lines.push_back(line);
	}
}

void ZX81::decompile_video() {
	d_file.clear();
	d_file.resize(NumLines);

	addr = sv_d_file;
	if (peek(addr++) != C_newline)
		ERROR("corrupted video file");

	for (int line = 0; line < NumLines; line++) {
		while (addr < sv_vars && peek(addr) != C_newline)
			d_file[line] += decode_zx81(peek(addr++));
		if (peek(addr) == C_newline)
			d_file[line] += decode_zx81(peek(addr++));
	}
	
	if (addr != sv_vars)
		ERROR("corrupted video file");
}

void ZX81::decompile_vars() {
	addr = sv_vars;
	int c = 0;
	while ((c = peek(addr)) != 0x80) {
		BasicVar var;

		if ((c & 0xe0) == 0x60) {		// single letter variable
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::Number;
			var.name = decode_zx81(c);
			var.value = decode_fp_number(addr); addr += 5;
		}
		else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::Number;
			var.name = decode_zx81(c);
			while (((c = peek(addr)) & 0xe0) != 0x80) {		// not last char
				addr++;
				c &= 0x3f;
				c |= 0x20;
				var.name += decode_zx81(c);
			}
			var.value = decode_fp_number(addr); addr += 5;
		}
		else if ((c & 0xe0) == 0x80) {	// array of numbers
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::ArrayNumbers;
			var.name = decode_zx81(c);

			int size = dpeek(addr); addr += 2;
			int addr0 = addr;

			int num_dimensions = peek(addr++);
			int num_elements = 1;
			for (int i = 0; i < num_dimensions; i++) {
				int dimension = dpeek(addr); addr += 2;
				num_elements *= dimension;
				var.dimensions.push_back(dimension);
			}
			for (int i = 0; i < num_elements; i++) {
				double value = decode_fp_number(addr); addr += 5;
				var.values.push_back(value);
			}

			assert(addr0 + size == addr);
		}
		else if ((c & 0xe0) == 0xe0) {	// for-next loop
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::ForNextLoop;
			var.name = decode_zx81(c);

			var.value = decode_fp_number(addr); addr += 5;
			var.limit = decode_fp_number(addr); addr += 5;
			var.step = decode_fp_number(addr); addr += 5;
			var.line_num = dpeek(addr); addr += 2;
		}
		else if ((c & 0xe0) == 0x40) {	// string
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::String;
			var.name = decode_zx81(c);

			int size = dpeek(addr); addr += 2;
			for (int i = 0; i < size; i++) {
				c = peek(addr++);
				var.str += decode_zx81(c);
			}
		}
		else if ((c & 0xe0) == 0xc0) {	// array of strings
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::ArrayStrings;
			var.name = decode_zx81(c);

			int size = dpeek(addr); addr += 2;
			int addr0 = addr;

			int num_dimensions = peek(addr++);
			int num_elements = 1;
			for (int i = 0; i < num_dimensions; i++) {
				int dimension = dpeek(addr); addr += 2;
				num_elements *= dimension;
				var.dimensions.push_back(dimension);
			}

			int last_dimension = var.dimensions.back();		// size of each string
			num_elements /= last_dimension;

			for (int i = 0; i < num_elements; i++) {
				string str;
				for (int j = 0; j < last_dimension; j++) {
					c = peek(addr++);
					str += decode_zx81(c);
				}
				var.strs.push_back(str);
			}

			assert(addr0 + size == addr);
		}
		else {
			ERROR("invalid variable marker: $" << fmt_hex(c, 2));
			break;
		}

		basic_vars.push_back(var);
	}
}

void ZX81::decode_basic_line(BasicLine& line) {
	if (decode_rem_code(line)) {
	}
	else {
		while (addr < end - 1) {
			if (decode_number(line)) {
			}
			else if (decode_ident(line)) {
			}
			else if (decode_string(line)) {
			}
			else {
				Token token;
				token.code = peek(addr++);
				line.tokens.push_back(token);
			}
		}
		decode_newline(line);
	}
}

bool ZX81::decode_rem_code(BasicLine& line) {
	if (peek(addr) == C_REM) {
		bool is_code = false;
		for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
			if ((peek(p) & 0x40) == 0x40) {			// special char
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
			for (int p = addr + 1; p < end - 1; p++)
				bytes.bytes.push_back(peek(p));
			line.tokens.push_back(bytes);

			addr = end - 1;
			decode_newline(line);
			return true;
		}
	}
	return false;
}

double ZX81::decode_fp_number(int addr) {
	array<uint8_t, 5> bytes{ 0 };
	for (size_t i = 0; i < bytes.size(); i++)
		bytes[i] = peek(addr + static_cast<int>(i));

	return zx81_to_float(bytes);
}

bool ZX81::decode_number(BasicLine& line) {
	Token token;
	string num;

	// get mantissa
	int p = addr;
	int num_dots = 0;
	int num_digits = 0;
	while (true) {
		int c = peek(p);
		if (c == C_dot) {
			p++;
			num_dots++;
			num.push_back('.');
			if (num_dots > 1)
				return false;
		}
		else if (c >= C_0 && c <= C_9) {
			p++;
			num_digits++;
			num.push_back(c - C_0 + '0');
		}
		else
			break;
	}
	if (num_digits == 0)
		return false;

	// get exponent
	int c = peek(p);
	if (p == C_E) {
		p++;
		num.push_back('E');
		c = peek(p);
		if (c == C_plus || c == C_minus) {
			p++;
			num.push_back(c == C_plus ? '+' : '-');
		}
		c = peek(p);
		if (c < C_0 || c > C_9)
			return false;
		while (c >= C_0 && c <= C_9) {
			p++;
			num.push_back(c - C_0 + '0');
			c = peek(p);
		}
	}

	double value1 = atof(num.c_str());

	// get number marker
	c = peek(p);
	if (c != C_number)
		return false;
	p++;

	// get fp value
	double value2 = decode_fp_number(p); p += 5;

	if (abs(value1 - value2) > 1e-6) 
		ERROR("line " << line.line_num << " number " << value1 << " != " << value2);
	
	token.code = T_number;
	token.str = num;
	token.num = value1;
	line.tokens.push_back(token);
	addr = p;
	return true;
}

bool ZX81::decode_ident(BasicLine& line) {
	Token token;
	int p = addr;
	while (true) {
		int c = peek(p);
		if (c < C_A || c > C_Z)
			break;
		token.ident.push_back(c - C_A + 'A');
		p++;
	}
	if (p == addr)					// no digits found
		return false;
	else {
		token.code = T_ident;
		line.tokens.push_back(token);
		addr = p;
		return true;
	}
}

bool ZX81::decode_string(BasicLine& line) {
	Token token;
	int p = addr;
	if (peek(p) != C_dquote)
		return false;
	p++;
	while (true) {
		int c = peek(p);
		if (c == C_newline)
			return false;
		if (c == C_dquote)
			break;
		token.str += decode_zx81(c);
		p++;
	}
	p++;		// skip end dquote

	token.code = T_string;
	line.tokens.push_back(token);
	addr = p;
	return true;
}

void ZX81::decode_newline(BasicLine& line) {
	Token token;
	token.code = peek(addr++);
	if (token.code != C_newline)
		ERROR("line " << line.line_num << " has no newline");

	line.tokens.push_back(token);
}

void ZX81::parse_line(const char* p) {
	skip_spaces(p);
	if (*p == '\0') {
	}
	else if (*p == '#')
		parse_meta_line(p + 1);
	else
		parse_basic_line(p);
}

void ZX81::parse_meta_line(const char* p) {
	int n = 0;
	if (match(p, "VARS")) {
		parse_basic_var(p);
	}
	else if (match(p, "E_PPC") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_e_ppc = n;
	}
	else if (match(p, "DF_CC") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_df_cc = n;
	}
	else if (match(p, "DEST") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_dest = n;
	}
	else if (match(p, "CH_ADD") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_ch_add = n;
	}
	else if (match(p, "X_PTR") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_x_ptr = n;
	}
	else if (match(p, "BREG") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_breg = n;
	}
	else if (match(p, "DF_SZ") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_df_sz = n;
	}
	else if (match(p, "S_TOP") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_s_top = n;
	}
	else if (match(p, "LAST_K") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_last_k = n;
	}
	else if (match(p, "DEBOUNCE") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_debounce = n;
	}
	else if (match(p, "MARGIN") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_margin = n;
	}
	else if (match(p, "NXTLIN") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_nxtlin = n;
	}
	else if (match(p, "OLDPPC") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_oldppc = n;
	}
	else if (match(p, "FLAGX") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_flagx = n;
	}
	else if (match(p, "STRLEN") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_strlen = n;
	}
	else if (match(p, "T_ADDR") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_t_addr = n;
	}
	else if (match(p, "SEED") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_seed = n;
	}
	else if (match(p, "FRAMES") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_frames = n;
	}
	else if (match(p, "COORDS_X") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_coords_x = n;
	}
	else if (match(p, "COORDS_Y") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_coords_y = n;
	}
	else if (match(p, "PR_CC") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_pr_cc = n;
	}
	else if (match(p, "S_POSN_COL") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_s_posn_col = n;
	}
	else if (match(p, "S_POSN_ROW") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_s_posn_row = n;
	}
	else if (match(p, "CDFLAG") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_cdflag = n;
	}
	else if (match(p, "PRBUFF") && match(p, "=")) {
		vector<uint8_t> bytes = encode_zx81(p);
		for (size_t i = 0; i < sv_prbuff.size() && i < bytes.size(); i++)
			sv_prbuff[i] = bytes[i];
	}
	else if (match(p, "MEMBOT") && match(p, "=")) {
		vector<uint8_t> bytes = encode_zx81(p);
		for (size_t i = 0; i < sv_membot.size() && i < bytes.size(); i++)
			sv_membot[i] = bytes[i];
	}
	else if (match(p, "MEM") && match(p, "=") && parse_integer(p, n) && parse_end(p)) {
		sv_mem = n;
	}
	else
		ERROR("line " << input_line_num << ": cannot parse: " << p);
}

void ZX81::parse_basic_line(const char* p) {
	BasicLine line;
	
	parse_integer(p, line.line_num);		// if false, line number = 0
	skip_spaces(p);
	while (*p != '\0' && *p != '#') {
		Token token;
		if (match(p, "RND")) 
			token.code = C_RND;
		else if (match(p, "INKEY$")) 
			token.code = C_INKEY_dollar;
		else if (match(p, "PI")) 
			token.code = C_PI;
		else if (match(p, "AT")) 
			token.code = C_AT;
		else if (match(p, "TAB")) 
			token.code = C_TAB;
		else if (match(p, "CODE")) 
			token.code = C_CODE;
		else if (match(p, "VAL")) 
			token.code = C_VAL;
		else if (match(p, "LEN")) 
			token.code = C_LEN;
		else if (match(p, "SIN")) 
			token.code = C_SIN;
		else if (match(p, "COS")) 
			token.code = C_COS;
		else if (match(p, "TAN")) 
			token.code = C_TAN;
		else if (match(p, "ASN")) 
			token.code = C_ASN;
		else if (match(p, "ACS")) 
			token.code = C_ACS;
		else if (match(p, "ATN")) 
			token.code = C_ATN;
		else if (match(p, "LN")) 
			token.code = C_LN;
		else if (match(p, "EXP")) 
			token.code = C_EXP;
		else if (match(p, "INT")) 
			token.code = C_INT;
		else if (match(p, "SQR")) 
			token.code = C_SQR;
		else if (match(p, "SGN")) 
			token.code = C_SGN;
		else if (match(p, "ABS")) 
			token.code = C_ABS;
		else if (match(p, "PEEK")) 
			token.code = C_PEEK;
		else if (match(p, "USR")) 
			token.code = C_USR;
		else if (match(p, "STR$")) 
			token.code = C_STR_dollar;
		else if (match(p, "CHR$")) 
			token.code = C_CHR_dollar;
		else if (match(p, "NOT")) 
			token.code = C_NOT;
		else if (match(p, "**")) 
			token.code = C_power;
		else if (match(p, "OR")) 
			token.code = C_OR;
		else if (match(p, "AND")) 
			token.code = C_AND;
		else if (match(p, "<=")) 
			token.code = C_le;
		else if (match(p, ">=")) 
			token.code = C_ge;
		else if (match(p, "<>")) 
			token.code = C_ne;
		else if (match(p, "THEN")) 
			token.code = C_THEN;
		else if (match(p, "TO")) 
			token.code = C_TO;
		else if (match(p, "STEP")) 
			token.code = C_STEP;
		else if (match(p, "LPRINT")) 
			token.code = C_LPRINT;
		else if (match(p, "LLIST")) 
			token.code = C_LLIST;
		else if (match(p, "STOP")) 
			token.code = C_STOP;
		else if (match(p, "SLOW")) 
			token.code = C_SLOW;
		else if (match(p, "FAST")) 
			token.code = C_FAST;
		else if (match(p, "NEW")) 
			token.code = C_NEW;
		else if (match(p, "SCROLL")) 
			token.code = C_SCROLL;
		else if (match(p, "CONT")) 
			token.code = C_CONT;
		else if (match(p, "DIM")) 
			token.code = C_DIM;
		else if (match(p, "REM")) {
			token.code = C_REM;
			line.tokens.push_back(token);

			skip_spaces(p);
			token.code = T_rem_code;
			token.bytes = encode_zx81(p);
		}
		else if (match(p, "FOR"))
			token.code = C_FOR;
		else if (match(p, "GOTO")) 
			token.code = C_GOTO;
		else if (match(p, "GOSUB")) 
			token.code = C_GOSUB;
		else if (match(p, "INPUT")) 
			token.code = C_INPUT;
		else if (match(p, "LOAD")) 
			token.code = C_LOAD;
		else if (match(p, "LIST")) 
			token.code = C_LIST;
		else if (match(p, "LET")) 
			token.code = C_LET;
		else if (match(p, "PAUSE")) 
			token.code = C_PAUSE;
		else if (match(p, "NEXT")) 
			token.code = C_NEXT;
		else if (match(p, "POKE")) 
			token.code = C_POKE;
		else if (match(p, "PRINT")) 
			token.code = C_PRINT;
		else if (match(p, "PLOT")) 
			token.code = C_PLOT;
		else if (match(p, "RUN")) 
			token.code = C_RUN;
		else if (match(p, "SAVE")) 
			token.code = C_SAVE;
		else if (match(p, "RAND")) 
			token.code = C_RAND;
		else if (match(p, "IF")) 
			token.code = C_IF;
		else if (match(p, "CLS")) 
			token.code = C_CLS;
		else if (match(p, "UNPLOT")) 
			token.code = C_UNPLOT;
		else if (match(p, "CLEAR")) 
			token.code = C_CLEAR;
		else if (match(p, "RETURN")) 
			token.code = C_RETURN;
		else if (match(p, "COPY")) 
			token.code = C_COPY;
		else if (match(p, "£")) 
			token.code = C_pound;
		else if (match(p, "$")) 
			token.code = C_dollar;
		else if (match(p, ":")) 
			token.code = C_colon;
		else if (match(p, "?")) 
			token.code = C_quest;
		else if (match(p, "(")) 
			token.code = C_lparens;
		else if (match(p, ")")) 
			token.code = C_rparens;
		else if (match(p, ">")) 
			token.code = C_gt;
		else if (match(p, "<")) 
			token.code = C_lt;
		else if (match(p, "=")) 
			token.code = C_eq;
		else if (match(p, "+")) 
			token.code = C_plus;
		else if (match(p, "-")) 
			token.code = C_minus;
		else if (match(p, "*")) 
			token.code = C_mult;
		else if (match(p, "/")) 
			token.code = C_div;
		else if (match(p, ";")) 
			token.code = C_semicolon;
		else if (match(p, ",")) 
			token.code = C_comma;
		else if (match(p, "_")) 
			token.code = C_space;
		else if (parse_number(p, token.num, token.str)) 
			token.code = T_number;
		else if (parse_string(p, token.str)) 
			token.code = T_string;
		else if (parse_ident(p, token.ident)) 
			token.code = T_ident;
		else {
			ERROR("line " << input_line_num << ": cannot parse: " << p);
			break;
		}

		line.tokens.push_back(token);
		skip_spaces(p);
	}

	Token token;
	token.code = C_newline;
	line.tokens.push_back(token);

	basic_lines.push_back(line);
}

void ZX81::parse_basic_var(const char* p) {
	BasicVar var;
	bool is_string = false;
	bool is_array = false;
	int num_elements = 0;

	// get name
	if (!parse_ident(p, var.name))
		goto error;

	// get string marker
	if (match(p, "$")) {
		is_string = true;
		if (var.name.size() > 1) {
			ERROR("line " << input_line_num << ": name " << var.name << "too long");
			return;
		}
	}

	// get array marker and dimensions
	if (match(p, "(")) {
		is_array = true;
		if (var.name.size() > 1) {
			ERROR("line " << input_line_num << ": name " << var.name << "too long");
			return;
		}

		num_elements = 1;
		do {
			int dimension = 0;
			if (!parse_integer(p, dimension))
				goto error;
			num_elements *= dimension;
			var.dimensions.push_back(dimension);
		} while (match(p, ","));

		if (!match(p, ")"))
			goto error;
	}

	// get =
	if (!match(p, "="))
		goto error;

	// get value
	if (is_string == false && is_array == false) {
		string str;
		
		var.type = BasicVar::Type::Number;
		if (!parse_number(p, var.value, str))
			goto error;

		if (match(p, ",")) {
			var.type = BasicVar::Type::ForNextLoop;
			if (var.name.size() > 1) {
				ERROR("line " << input_line_num << ": name " << var.name << "too long");
				return;
			}

			if (!parse_number(p, var.limit, str))
				goto error;
			if (!match(p, ","))
				goto error;
			if (!parse_number(p, var.step, str))
				goto error;
			if (!match(p, ","))
				goto error;
			if (!parse_integer(p, var.line_num))
				goto error;
			if (!parse_end(p))
				goto error;
		}
	}
	else if (is_string == false && is_array == true) {
		double value = 0.0;
		string str;

		var.type = BasicVar::Type::ArrayNumbers;

		for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
			if (i > 0) {
				if (!match(p, ","))
					goto error;
			}
			if (!parse_number(p, value, str))
				goto error;
			var.values.push_back(value);
		}
		if (!parse_end(p))
			goto error;
	}
	else if (is_string == true && is_array == false) {
		var.type = BasicVar::Type::String;

		if (!parse_string(p, var.str))
			goto error;
		if (!parse_end(p))
			goto error;
	}
	else if (is_string == true && is_array == true) {
		string str;

		var.type = BasicVar::Type::ArrayStrings;

		int last_dimension = var.dimensions.back();
		num_elements /= last_dimension;

		for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
			if (i > 0) {
				if (!match(p, ","))
					goto error;
			}
			if (!parse_string(p, str))
				goto error;
			if (str.size() != static_cast<size_t>(last_dimension)) {
				ERROR("line " << input_line_num << ": string length should be " << last_dimension);
				return;
			}
			var.strs.push_back(str);
		}
		if (!parse_end(p))
			goto error;
	}

	basic_vars.push_back(var);
	return;

error:
	ERROR("line " << input_line_num << ": cannot parse: " << p);
}

void ZX81::compile_sysvars() {
	poke(VERSN, sv_versn);
	dpoke(E_PPC, sv_e_ppc);
	dpoke(D_FILE, sv_d_file);
	dpoke(DF_CC, sv_df_cc);
	dpoke(VARS, sv_vars);
	dpoke(DEST, sv_dest);
	dpoke(E_LINE, sv_e_line);
	dpoke(CH_ADD, sv_ch_add);
	dpoke(X_PTR, sv_x_ptr);
	dpoke(STKBOT, sv_stkbot);
	dpoke(STKEND, sv_stkend);
	poke(BREG, sv_breg);
	dpoke(MEM, sv_mem);
	poke(FREE1, sv_free1);
	poke(DF_SZ, sv_df_sz);
	dpoke(S_TOP, sv_s_top);
	dpoke(LAST_K, sv_last_k);
	poke(DEBOUNCE, sv_debounce);
	poke(MARGIN, sv_margin);
	dpoke(NXTLIN, sv_nxtlin);
	dpoke(OLDPPC, sv_oldppc);
	poke(FLAGX, sv_flagx);
	dpoke(STRLEN, sv_strlen);
	dpoke(T_ADDR, sv_t_addr);
	dpoke(SEED, sv_seed);
	dpoke(FRAMES, sv_frames);
	poke(COORDS_X, sv_coords_x);
	poke(COORDS_Y, sv_coords_y);
	poke(PR_CC, sv_pr_cc);
	poke(S_POSN_COL, sv_s_posn_col);
	poke(S_POSN_ROW, sv_s_posn_row);
	poke(CDFLAG, sv_cdflag);
	for (size_t i = 0; i < sv_prbuff.size(); i++)
		poke(PRBUFF + static_cast<int>(i), sv_prbuff[i]);
	for (size_t i = 0; i < sv_membot.size(); i++)
		poke(MEMBOT + static_cast<int>(i), sv_membot[i]);
	dpoke(FREE2, sv_free2);
}

void ZX81::compile_basic() {
	string str;
	const char* p = nullptr;
	array<uint8_t, 5> fp_bytes{ 0 };
	vector<uint8_t> str_bytes;

	for (auto& line : basic_lines) {
		vector<uint8_t> bytes;
		for (auto& token : line.tokens) {
			switch (token.code) {
			case T_none:
				break;
			case T_number:
				p = token.str.c_str();
				str_bytes = encode_zx81(p);
				fp_bytes = float_to_zx81(token.num);
				bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
				bytes.push_back(C_number);
				bytes.insert(bytes.end(), fp_bytes.begin(), fp_bytes.end());
				break;
			case T_string:
				p = token.str.c_str();
				str_bytes = encode_zx81(p);
				bytes.push_back(C_dquote);
				bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
				bytes.push_back(C_dquote);
				break;
			case T_ident:
				p = token.ident.c_str();
				str_bytes = encode_zx81(p);
				bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
				break;
			case T_rem_code:
				bytes.insert(bytes.end(), token.bytes.begin(), token.bytes.end());
				break;
			default:
				bytes.push_back(token.code);
			}
		}

		// add line number and size
		mem_bytes.push_back(line.line_num >> 8);
		mem_bytes.push_back(line.line_num);

		mem_bytes.push_back(bytes.size() & 0xff);
		mem_bytes.push_back((bytes.size() >> 8) & 0xff);

		mem_bytes.insert(mem_bytes.end(), bytes.begin(), bytes.end());
	}
}

void ZX81::compile_video() {
	addr = static_cast<int>(mem_bytes.size()) + BaseAddr;
	sv_d_file = addr;
	mem_bytes.push_back(C_newline);
	for (auto& line : d_file) {
		const char* p = line.c_str();
		vector<uint8_t> bytes = encode_zx81(p);
		mem_bytes.insert(mem_bytes.end(), bytes.begin(), bytes.end());
	}
}

void ZX81::compile_vars() {
	addr = static_cast<int>(mem_bytes.size()) + BaseAddr;
	sv_vars = addr;

	vector<uint8_t> str_bytes;
	array<uint8_t, 5> fp_bytes{ 0 };
	int last_dimension = 0;
	int size = 0;

	for (auto& var : basic_vars) {
		const char* p = var.name.c_str();
		vector<uint8_t> name_bytes = encode_zx81(p);
		assert(name_bytes.size() > 0);

		switch (var.type) {
		case BasicVar::Type::Number:
			// put name
			if (var.name.size() == 1) {
				mem_bytes.push_back((name_bytes[0] & 0x3f) | 0x60);		// 011-letter
			}
			else {
				mem_bytes.push_back((name_bytes[0] & 0x3f) | 0xa0);		// 101-letter
				for (size_t i = 1; i < name_bytes.size() - 1; i++)
					mem_bytes.push_back(name_bytes[i] & 0x3f);			// 001-letter
				mem_bytes.push_back((name_bytes.back() & 0x3f) | 0x80);	// 101-letter
			}

			// put value
			fp_bytes = float_to_zx81(var.value);
			mem_bytes.insert(mem_bytes.end(), fp_bytes.begin(), fp_bytes.end());
			break;

		case BasicVar::Type::ArrayNumbers:
			// put name
			mem_bytes.push_back((name_bytes[0] & 0x1f) | 0x80);			// 100-letter

			// put dimensions
			size = 1
				+ 2 * static_cast<int>(var.dimensions.size())
				+ 5 * static_cast<int>(var.values.size());
			mem_bytes.push_back(size);
			mem_bytes.push_back(size >> 8);

			mem_bytes.push_back(var.dimensions.size() & 0xff);

			for (auto& dimension : var.dimensions) {
				mem_bytes.push_back(dimension);
				mem_bytes.push_back(dimension >> 8);
			}

			// put values
			for (auto& value : var.values) {
				fp_bytes = float_to_zx81(value);
				mem_bytes.insert(mem_bytes.end(), fp_bytes.begin(), fp_bytes.end());
			}
			break;

		case BasicVar::Type::ForNextLoop:
			// put name
			mem_bytes.push_back((name_bytes[0] & 0x3f) | 0xe0);			// 111-letter

			// put values
			fp_bytes = float_to_zx81(var.value);
			mem_bytes.insert(mem_bytes.end(), fp_bytes.begin(), fp_bytes.end());

			fp_bytes = float_to_zx81(var.limit);
			mem_bytes.insert(mem_bytes.end(), fp_bytes.begin(), fp_bytes.end());

			fp_bytes = float_to_zx81(var.step);
			mem_bytes.insert(mem_bytes.end(), fp_bytes.begin(), fp_bytes.end());

			mem_bytes.push_back(var.line_num);
			mem_bytes.push_back(var.line_num >> 8);
			break;

		case BasicVar::Type::String:
			// put name
			mem_bytes.push_back((name_bytes[0] & 0x1f) | 0x40);			// 010-letter

			mem_bytes.push_back((var.str.size()) & 0xff);
			mem_bytes.push_back((var.str.size() >> 8) & 0xff);

			p = var.str.c_str();
			str_bytes = encode_zx81(p);
			mem_bytes.insert(mem_bytes.end(), str_bytes.begin(), str_bytes.end());
			break;

		case BasicVar::Type::ArrayStrings:
			// put name
			mem_bytes.push_back((name_bytes[0] & 0x1f) | 0xc0);			// 110-letter

			// put dimensions
			last_dimension = var.dimensions.back();
			size = 1
				+ 2 * static_cast<int>(var.dimensions.size())
				+ last_dimension * static_cast<int>(var.strs.size());
			mem_bytes.push_back(size);
			mem_bytes.push_back(size >> 8);

			mem_bytes.push_back(var.dimensions.size() & 0xff);

			for (auto& dimension : var.dimensions) {
				mem_bytes.push_back(dimension);
				mem_bytes.push_back(dimension >> 8);
			}

			// put strings
			for (auto& str : var.strs) {
				p = str.c_str();
				str_bytes = encode_zx81(p);
				mem_bytes.insert(mem_bytes.end(), str_bytes.begin(), str_bytes.end());
			}
			break;

		default:
			assert(0);
		}
	}
	
	mem_bytes.push_back(0x80);
	addr = static_cast<int>(mem_bytes.size()) + BaseAddr;
	sv_e_line = addr;
	sv_stkbot = addr;
	sv_stkend = addr;
}

void ZX81::write_basic_lines(ofstream& ofs) {
	for (auto& line : basic_lines) {
		if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
			ofs << "# [$" << fmt_hex(line.addr, 4) << "]" << endl;

		if (!line.label.empty())
			ofs << "@" << line.label << ":" << endl;

		ofs << fmt_line_number(line.line_num) << " ";

		for (auto& token : line.tokens) {
			switch (token.code) {
			case T_none:
				break;
			case T_number:
				ofs << token.str;
				break;
			case T_string:
				ofs << "\"" << token.str << "\"";
				break;
			case T_ident:
				ofs << token.ident;
				break;
			case T_rem_code:
				for (auto& c : token.bytes)
					ofs << "\\" << fmt_hex(c & 0xff, 2);
				break;
			case C_space:
				ofs << "_";
				break;
			default:
				ofs << decode_zx81(token.code);
			}
		}
	}

	if (!basic_lines.empty())
		ofs << endl;
}

void ZX81::write_basic_vars(ofstream& ofs) {
	int num_elements = 0;
	int last_dimension = 0;

	for (auto& var : basic_vars) {
		switch (var.type) {
		case BasicVar::Type::Number:
			ofs << "#VARS " << var.name << "=" << var.value << endl;
			break;
		case BasicVar::Type::ArrayNumbers:
			num_elements = 1;
			ofs << "#VARS " << var.name << "(";
			
			for (size_t i = 0; i < var.dimensions.size(); i++) {
				if (i > 0)
					ofs << ",";
				ofs << var.dimensions[i];
				num_elements *= var.dimensions[i];
			}
			
			ofs << ")=";
			
			for (int i = 0; i < num_elements; i++) {
				if (i > 0)
					ofs << ",";
				ofs << var.values[i];
			}
			
			ofs << endl;
			break;
		case BasicVar::Type::ForNextLoop:
			ofs << "#VARS " << var.name << "=" << var.value
				<< "," << var.limit << "," << var.step << "," << var.line_num << endl;
			break;
		case BasicVar::Type::String:
			ofs << "#VARS " << var.name << "$=\"" << var.str << "\"" << endl;
			break;
		case BasicVar::Type::ArrayStrings:
			num_elements = 1;
			ofs << "#VARS " << var.name << "$(";
			
			for (size_t i = 0; i < var.dimensions.size(); i++) {
				if (i > 0)
					ofs << ",";
				ofs << var.dimensions[i];
				num_elements *= var.dimensions[i];
			}

			ofs << ")=";
			
			last_dimension = var.dimensions.back();
			num_elements /= last_dimension;
			for (int i = 0; i < num_elements; i++) {
				if (i > 0)
					ofs << ",";
				ofs << "\"" << var.strs[i] << "\"";
			}
			
			ofs << endl;
			break;
		default:
			assert(0);
		}
	}

	if (!basic_vars.empty())
		ofs << endl;
}

void ZX81::write_sysvars(ofstream& ofs) {
	ofs << "#E_PPC=" << sv_e_ppc << endl;
	ofs << "#DF_CC=$" << fmt_hex(sv_df_cc, 4) << endl;
	ofs << "#DEST=$" << fmt_hex(sv_dest, 4) << endl;
	ofs << "#CH_ADD=$" << fmt_hex(sv_ch_add, 4) << endl;
	ofs << "#X_PTR=$" << fmt_hex(sv_x_ptr, 4) << endl;
	ofs << "#BREG=" << sv_breg << endl;
	ofs << "#MEM=$" << fmt_hex(sv_mem, 4) << endl;
	ofs << "#DF_SZ=" << sv_df_sz << endl;
	ofs << "#S_TOP=" << sv_s_top << endl;
	ofs << "#LAST_K=$" << fmt_hex(sv_last_k, 4) << endl;
	ofs << "#DEBOUNCE=$" << fmt_hex(sv_debounce, 2) << endl;
	ofs << "#MARGIN=" << sv_margin << endl;
	ofs << "#NXTLIN=$" << fmt_hex(sv_nxtlin, 4) << endl;
	ofs << "#OLDPPC=" << sv_oldppc << endl;
	ofs << "#FLAGX=$" << fmt_hex(sv_flagx, 2) << endl;
	ofs << "#STRLEN=" << sv_strlen << endl;
	ofs << "#T_ADDR=$" << fmt_hex(sv_t_addr, 4) << endl;
	ofs << "#SEED=$" << fmt_hex(sv_seed, 4) << endl;
	ofs << "#FRAMES=$" << fmt_hex(sv_frames, 4) << endl;
	ofs << "#COORDS_X=" << sv_coords_x << endl;
	ofs << "#COORDS_Y=" << sv_coords_y << endl;
	ofs << "#PR_CC=$" << fmt_hex(sv_pr_cc, 2) << endl;
	ofs << "#S_POSN_COL=" << sv_s_posn_col << endl;
	ofs << "#S_POSN_ROW=" << sv_s_posn_row << endl;
	ofs << "#CDFLAG=$" << fmt_hex(sv_cdflag, 2) << endl;

	ofs << "#PRBUFF=";
	for (size_t i = 0; i < sv_prbuff.size(); i++)
		ofs << "\\" << fmt_hex(sv_prbuff[i], 2);
	ofs << endl;

	ofs << "#MEMBOT=";
	for (size_t i = 0; i < sv_membot.size(); i++)
		ofs << "\\" << fmt_hex(sv_membot[i], 2);
	ofs << endl;

	ofs << endl;
}

void ZX81::parse_b81(const string& b81_filename) {
	ifstream ifs(b81_filename);
	if (!ifs.is_open()) {
		perror(b81_filename.c_str());
		FATAL_ERROR("read file " << b81_filename);
	}

	string text;
	input_line_num = 0;
	while (getline(ifs, text)) {
		input_line_num++;
		while (!text.empty() && text.back() == '\\') {
			text.pop_back();
			text.push_back(' ');
			string cont;
			if (!getline(ifs, cont))
				break;
			text += cont;
		}
		parse_line(text.c_str());
	}
}

void ZX81::write_b81(const string& b81_filename) {
	ofstream ofs(b81_filename);
	if (!ofs.is_open()) {
		perror(b81_filename.c_str());
		FATAL_ERROR("write file " << b81_filename);
	}

	write_basic_lines(ofs);
	write_basic_vars(ofs);
	write_sysvars(ofs);
}
