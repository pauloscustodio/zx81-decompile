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
#include <sstream>
using namespace std;

//-----------------------------------------------------------------------------
// encode/decode zx81 character set
//-----------------------------------------------------------------------------

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
/* 0x0c */	"\\0c",		
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
/* 0x76 */	"\\76",			
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
/* 0x8c */	"\\8c",	
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
/* 0xa7 */	"%B",			
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

string decode_zx81(char c) {
	string code = zx81_chars[c & 0xff];
	if (code.size() > 1 && isalnum(code.front()))
		code = string(" ") + code + " ";
	return code;
}

Bytes encode_zx81(const char*& p) {
	Bytes bytes;
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
				g_errors.error("cannot encode", p);
				break;
			}
		}
	}
	return bytes;
}

string encode_hex(const Bytes& bytes) {
	ostringstream oss;
	for (auto& b : bytes) 
		oss << "\\" << fmt_hex(b, 2);
	return oss.str();
}

//-----------------------------------------------------------------------------
// virtual machine
//-----------------------------------------------------------------------------

ZX81vm::ZX81vm() {
	int addr = PROG;
	dpoke(D_FILE, addr);
	dpoke(NXTLIN, addr);
	addr = write_empty_d_file(addr);
	dpoke(VARS, addr);
	addr = write_empty_vars(addr);
	dpoke(E_LINE, addr);
	dpoke(STKBOT, addr);
	dpoke(STKEND, addr);

	// print position
	dpoke(DF_CC, dpeek(D_FILE) + 1);
	poke(DF_SZ, 2);
	poke(S_POSN_ROW, NumRows + 1);
	poke(S_POSN_COL, NumCols + 1);
	poke(PR_CC, PRBUFF >> 8);

	// system variables
	dpoke(MEM, MEMBOT);
	dpoke(LAST_K, 0xffff);
	poke(DEBOUNCE, 0xff);
	poke(MARGIN, 55);
	dpoke(FRAMES, 0xffff);
	poke(CDFLAG, FlagSlow);
	poke(PRBUFF + 32, C_newline);
}

int ZX81vm::write_empty_d_file(int addr) {
	Bytes d_file = get_empty_d_file();
	poke_bytes(addr, d_file);
	return addr + static_cast<int>(d_file.size());
}

int ZX81vm::write_empty_vars(int addr) {
	poke(addr++, 0x80);
	return addr;
}

void ZX81vm::init_video_to_stkend(int addr, Bytes& d_file_bytes, Bytes& e_line_bytes) {
	dpoke(D_FILE, addr);
	dpoke(NXTLIN, addr);
	addr += poke_bytes(addr, d_file_bytes);

	// vars
	dpoke(VARS, addr);
	poke(addr++, 0x80);

	init_e_line_to_stkend(addr, e_line_bytes);
}

void ZX81vm::init_e_line_to_stkend(int addr, Bytes& e_line_bytes) {
	// edit line
	dpoke(E_LINE, addr);
	addr += poke_bytes(addr, e_line_bytes);

	// calculator stack
	dpoke(STKBOT, addr);
	dpoke(STKEND, addr);
}

Bytes ZX81vm::get_empty_d_file() {
	Bytes out;
	out.push_back(C_newline);
	for (int row = 0; row < NumRows; row++) {
		for (int col = 0; col < NumCols; col++) {
			out.push_back(C_space);
		}
		out.push_back(C_newline);
	}
	return out;
}

Bytes ZX81vm::get_empty_e_line() {
	Bytes out;
	out.push_back(0x80);
	return out;
}

//-----------------------------------------------------------------------------
// read/write binary files
//-----------------------------------------------------------------------------

void ZX81vm::read_p_file(const string& filename) {
	// open file
	ifstream ifs(filename, ios::binary);
	if (!ifs.is_open()) {
		perror(filename.c_str());
		g_errors.fatal_error("open file", filename);
	}

	// get size
	ifs.seekg(0, ios::end);
	size_t size = ifs.tellg();
	ifs.seekg(0, ios::beg);

	// read bytes
	ifs.read(reinterpret_cast<char*>(&mem[SAVE_ADDR]), size);
	size_t readed = ifs.gcount();
	if (readed != size) {
		perror(filename.c_str());
		g_errors.fatal_error("read " + to_string(size) + " bytes from file " + filename);
	}
}

void ZX81vm::write_p_file(const string& filename) const {
	// open file
	ofstream ofs(filename, ios::binary);
	if (!ofs.is_open()) {
		perror(filename.c_str());
		g_errors.fatal_error("open file", filename);
	}

	size_t size = dpeek(E_LINE) - SAVE_ADDR;
	ofs.write(reinterpret_cast<const char*>(&mem[SAVE_ADDR]), size);
	size_t written = ofs.tellp();
	if (written != size) {
		perror(filename.c_str());
		g_errors.fatal_error("write " + to_string(size) + " bytes to file " + filename);
	}
}

//-----------------------------------------------------------------------------
// manipulate memory
//-----------------------------------------------------------------------------

int ZX81vm::peek(int addr) const {
	return mem[addr & 0xffff];
}

int ZX81vm::dpeek(int addr) const {
	return peek(addr) + (peek(addr + 1) << 8);
}

int ZX81vm::dpeek_be(int addr) const {
	return (peek(addr) << 8) + peek(addr + 1);
}

double ZX81vm::fpeek(int addr) const {
	array<uint8_t, 5> bytes{ 0 };
	for (int i = 0; i < 5; i++)
		bytes[i] = peek(addr + i);
	return zx81_to_float(bytes);
}

Bytes ZX81vm::peek_bytes(int addr, int size) const {
	Bytes out;
	out.insert(out.end(), mem.begin() + addr, mem.begin() + addr + size);
	return out;
}

string ZX81vm::peek_hex(int addr, int len) const {
	Bytes out;
	for (int i = 0; i < len; i++)
		out.push_back(peek(addr + i));
	return encode_hex(out);
}

void ZX81vm::poke(int addr, int value) {
	mem[addr & 0xffff] = value & 0xff;
}

void ZX81vm::dpoke(int addr, int value) {
	poke(addr, value);
	poke(addr + 1, value >> 8);
}

void ZX81vm::dpoke_be(int addr, int value) {
	poke(addr, value >> 8);
	poke(addr + 1, value);
}

void ZX81vm::fpoke(int addr, double value) {
	array<uint8_t, 5> bytes = float_to_zx81(value);
	for (int i = 0; i < 5; i++)
		poke(addr + i, bytes[i]);
}

int ZX81vm::get_line_addr(int line_num) {
	int addr = PROG;
	int d_file = dpeek(D_FILE);
	while (addr < d_file) {
		int cur_line = dpeek_be(addr);
		if (cur_line >= line_num)
			return addr;
		int size = dpeek(addr + 2);
		addr += 4 + size;
	}
	return d_file;
}

//-----------------------------------------------------------------------------
// BASIC structure
//-----------------------------------------------------------------------------

ZX81basic::ZX81basic() {
	d_file_bytes = ZX81vm::get_empty_d_file();
}

void ZX81basic::write_b81_file(const string& filename) const {
	ofstream ofs(filename);
	if (!ofs.is_open()) {
		perror(filename.c_str());
		g_errors.fatal_error("write file", filename);
	}

	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
		write_sysvars(ofs);
	write_basic_lines(ofs);
	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
		write_video(ofs);
	write_basic_vars(ofs);
	write_basic_system(ofs);
}

int ZX81basic::peek_sysvars(int addr) const {
	size_t idx = addr - VERSN;
	assert(idx < sysvars.size());
	return sysvars[idx];
}

int ZX81basic::dpeek_sysvars(int addr) const {
	return peek_sysvars(addr) + (peek_sysvars(addr + 1) << 8);
}

void ZX81basic::write_sysvars(ofstream& ofs) const {
	ofs << "# [VERSN     =  " << peek_sysvars(VERSN) << "]" << endl;
	ofs << "# [E_PPC     =  " << dpeek_sysvars(E_PPC) << "]" << endl;
	ofs << "# [D_FILE    = $" << fmt_hex(dpeek_sysvars(D_FILE), 4) << "]" << endl;
	ofs << "# [DF_CC     = $" << fmt_hex(dpeek_sysvars(DF_CC), 4) << "]" << endl;
	ofs << "# [VARS      = $" << fmt_hex(dpeek_sysvars(VARS), 4) << "]" << endl;
	ofs << "# [DEST      = $" << fmt_hex(dpeek_sysvars(DEST), 4) << "]" << endl;
	ofs << "# [E_LINE    = $" << fmt_hex(dpeek_sysvars(E_LINE), 4) << "]" << endl;
	ofs << "# [CH_ADD    = $" << fmt_hex(dpeek_sysvars(CH_ADD), 4) << "]" << endl;
	ofs << "# [X_PTR     = $" << fmt_hex(dpeek_sysvars(X_PTR), 4) << "]" << endl;
	ofs << "# [STKBOT    = $" << fmt_hex(dpeek_sysvars(STKBOT), 4) << "]" << endl;
	ofs << "# [STKEND    = $" << fmt_hex(dpeek_sysvars(STKEND), 4) << "]" << endl;
	ofs << "# [BREG      =  " << peek_sysvars(BREG) << "]" << endl;
	ofs << "# [MEM       = $" << fmt_hex(dpeek_sysvars(MEM), 4) << "]" << endl;
	ofs << "# [FREE1     =  " << peek_sysvars(FREE1) << "]" << endl;
	ofs << "# [DF_SZ     =  " << peek_sysvars(DF_SZ) << "]" << endl;
	ofs << "# [S_TOP     =  " << dpeek_sysvars(S_TOP) << "]" << endl;
	ofs << "# [LAST_K    = $" << fmt_hex(dpeek_sysvars(LAST_K), 4) << "]" << endl;
	ofs << "# [DEBOUNCE  =$" << fmt_hex(peek_sysvars(DEBOUNCE), 2) << "]" << endl;
	ofs << "# [MARGIN    =  " << peek_sysvars(MARGIN) << "]" << endl;
	ofs << "# [NXTLIN    = $" << fmt_hex(dpeek_sysvars(NXTLIN), 4) << "]" << endl;
	ofs << "# [OLDPPC    =  " << dpeek_sysvars(OLDPPC) << "]" << endl;
	ofs << "# [FLAGX     = $" << fmt_hex(peek_sysvars(FLAGX), 2) << "]" << endl;
	ofs << "# [STRLEN    =  " << dpeek_sysvars(STRLEN) << "]" << endl;
	ofs << "# [T_ADDR    = $" << fmt_hex(dpeek_sysvars(T_ADDR), 4) << "]" << endl;
	ofs << "# [SEED      = $" << fmt_hex(dpeek_sysvars(SEED), 4) << "]" << endl;
	ofs << "# [FRAMES    = $" << fmt_hex(dpeek_sysvars(FRAMES), 4) << "]" << endl;
	ofs << "# [COORDS_X  =  " << peek_sysvars(COORDS_X) << "]" << endl;
	ofs << "# [COORDS_Y  =  " << peek_sysvars(COORDS_Y) << "]" << endl;
	ofs << "# [PR_CC     = $" << fmt_hex(peek_sysvars(PR_CC), 2) << "]" << endl;
	ofs << "# [S_POSN_COL=  " << peek_sysvars(S_POSN_COL) << "]" << endl;
	ofs << "# [S_POSN_ROW=  " << peek_sysvars(S_POSN_ROW) << "]" << endl;
	ofs << "# [CDFLAG    = $" << fmt_hex(peek_sysvars(CDFLAG), 2) << "]" << endl;

	ofs << "# [PRBUFF=";
	for (int i = 0; i < 33; i++)
		ofs << "\\" << fmt_hex(peek_sysvars(PRBUFF + i), 2);
	ofs << "]" << endl;

	ofs << "# [MEMBOT=";
	for (int i = 0; i < 30; i++)
		ofs << "\\" << fmt_hex(peek_sysvars(MEMBOT + i), 2);
	ofs << "]" << endl;

	ofs << "# [FREE2     = $" << fmt_hex(dpeek_sysvars(FREE2), 4) << "]" << endl;
	ofs << endl;
}

void ZX81basic::write_basic_lines(ofstream& ofs) const {
	for (auto& line : lines) {
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
			case C_newline:
				ofs << endl;
				break;
			default:
				ofs << decode_zx81(token.code);
			}
		}
	}

	if (!lines.empty())
		ofs << endl;
}

void ZX81basic::write_video(ofstream& ofs) const {
	int addr;
	if (lines.empty())
		addr = PROG;
	else
		addr = lines.back().addr + 4 + lines.back().size;

	size_t p = 0;
	while (p < d_file_bytes.size()) {
		ofs << "# [$" << fmt_hex(addr,4)<<"] = \"";
		int c;
		do {
			if (p < d_file_bytes.size()) {
				c = d_file_bytes[p];
				p++;
				addr++;
			}
			else
				c = C_newline;
			ofs << decode_zx81(c);
		} while (c != C_newline);
		ofs << "\"" << endl;
	}

	ofs << endl;
}

void ZX81basic::write_basic_vars(ofstream& ofs) const {
	int num_elements = 0;
	int last_dimension = 0;

	int addr = 0;
	for (auto& var : vars) {
		addr = var.addr;

		if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
			ofs << "# [$" << fmt_hex(addr, 4) << "]" << endl;

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
		addr += var.size;
	}

	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
		ofs << "# [$" << fmt_hex(addr, 4) << "] = $80" << endl;

	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG || !vars.empty())
		ofs << endl;
}

void ZX81basic::write_basic_system(ofstream& ofs) const {
	ofs << "#SYSVARS=" << encode_hex(sysvars) << endl;
	ofs << "#D_FILE=" << encode_hex(d_file_bytes) << endl;
	ofs << "#WORKSPACE=" << encode_hex(e_line_bytes) << endl;
	ofs << endl;

	// autostart
	ofs << "#AUTOSTART=" << autostart << endl;

	// fast mode
	ofs << "#FAST=" << fast << endl;
}

//-----------------------------------------------------------------------------
// BASIC parser
//-----------------------------------------------------------------------------

ZX81parser::ZX81parser(const string& b81_filename, ZX81basic& result)
	: m_filename(b81_filename), m_ifs(b81_filename), m_basic(result) {

	if (!m_ifs.is_open()) {
		perror(b81_filename.c_str());
		g_errors.fatal_error("read file", b81_filename);
	}
}

void ZX81parser::parse() {
	g_errors.set_filename(m_filename);

	string text;
	while (getline(m_ifs, text)) {
		g_errors.set_line_num(g_errors.get_line_num() + 1);
		while (!text.empty() && text.back() == '\\') {
			text.pop_back();
			text.push_back(' ');
			string cont;
			if (!getline(m_ifs, cont))
				break;
			g_errors.set_line_num(g_errors.get_line_num() + 1);
			text += cont;
		}
		p = text.c_str();
		parse_line();
	}

	g_errors.clear();
}

void ZX81parser::skip_spaces() {
	while (*p != '\0' && isspace(*p))
		p++;
}

bool ZX81parser::match(const string& compare) {
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

bool ZX81parser::parse_integer(int& value) {
	skip_spaces();
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

bool ZX81parser::parse_number(double& value, string& value_text) {
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

bool ZX81parser::parse_string(string& str) {
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

bool ZX81parser::parse_ident(string& ident) {
	skip_spaces();
	if (!isalpha(*p))
		return false;
	while (isalnum(*p))
		ident.push_back(*p++);
	return true;
}

bool ZX81parser::parse_label(string& ident) {
	skip_spaces();
	const char* p0 = p;
	if (!parse_line_num_ref(ident)) {
		p = p0;
		return false;
	}
	skip_spaces();
	if (*p != ':') {
		p = p0;
		return false;
	}
	p++;
	return true;
}

bool ZX81parser::parse_line_num_ref(string& ident) {
	skip_spaces();
	const char* p0 = p;
	if (*p != '@')
		return false;
	p++;
	if (!parse_ident(ident)) {
		p = p0;
		return false;
	}
	return true;
}

bool ZX81parser::parse_line_addr_ref(string& ident) {
	skip_spaces();
	const char* p0 = p;
	if (*p != '&')
		return false;
	p++;
	if (!parse_ident(ident)) {
		p = p0;
		return false;
	}
	return true;
}

bool ZX81parser::parse_end() {
	skip_spaces();
	if (*p == '\0' || *p == '#')
		return true;
	else
		return false;
}

void ZX81parser::parse_line() {
	skip_spaces();
	if (*p == '\0') {
	}
	else if (*p == '#') {
		p++;
		parse_meta_line();
	}
	else
		parse_basic_line();
}

void ZX81parser::parse_meta_line() {
	int n = 0;
	if (match("VARS")) {
		parse_basic_var();
	}
	else if (match("SYSVARS") && match("=")) {
		m_basic.sysvars = encode_zx81(p);
	}
	else if (match("D_FILE") && match("=")) {
		m_basic.d_file_bytes = encode_zx81(p);
	}
	else if (match("WORKSPACE") && match("=")) {
		m_basic.e_line_bytes = encode_zx81(p);
	}
	else if (match("AUTOSTART") && match("=") && parse_integer(n) && parse_end()) {
		m_basic.autostart = n;
	}
	else if (match("FAST") && match("=") && parse_integer(n) && parse_end()) {
		m_basic.fast = n ? true : false;
	}
	else if (match("INCREMENT") && match("=") && parse_integer(n) && parse_end()) {
		m_basic.auto_increment = n;
	}
	else {
		// ignore, consider a comment
	}
}

void ZX81parser::parse_basic_line() {
	BasicLine line;

	// get label and/or line number
	bool got_line_num = false;
	bool got_label = false;
	bool found_some = false;
	do {
		found_some = false;
		if (!got_line_num && parse_integer(line.line_num)) {
			got_line_num = true;
			found_some = true;
		}
		if (!got_label && parse_label(line.label)) {
			got_label = true;
			found_some = true;
		}
	} while (found_some);

	skip_spaces();
	while (*p != '\0' && *p != '#') {
		Token token;
		if (match("RND"))
			token.code = C_RND;
		else if (match("INKEY$"))
			token.code = C_INKEY_dollar;
		else if (match("PI"))
			token.code = C_PI;
		else if (match("AT"))
			token.code = C_AT;
		else if (match("TAB"))
			token.code = C_TAB;
		else if (match("CODE"))
			token.code = C_CODE;
		else if (match("VAL"))
			token.code = C_VAL;
		else if (match("LEN"))
			token.code = C_LEN;
		else if (match("SIN"))
			token.code = C_SIN;
		else if (match("COS"))
			token.code = C_COS;
		else if (match("TAN"))
			token.code = C_TAN;
		else if (match("ASN"))
			token.code = C_ASN;
		else if (match("ACS"))
			token.code = C_ACS;
		else if (match("ATN"))
			token.code = C_ATN;
		else if (match("LN"))
			token.code = C_LN;
		else if (match("EXP"))
			token.code = C_EXP;
		else if (match("INT"))
			token.code = C_INT;
		else if (match("SQR"))
			token.code = C_SQR;
		else if (match("SGN"))
			token.code = C_SGN;
		else if (match("ABS"))
			token.code = C_ABS;
		else if (match("PEEK"))
			token.code = C_PEEK;
		else if (match("USR"))
			token.code = C_USR;
		else if (match("STR$"))
			token.code = C_STR_dollar;
		else if (match("CHR$"))
			token.code = C_CHR_dollar;
		else if (match("NOT"))
			token.code = C_NOT;
		else if (match("**"))
			token.code = C_power;
		else if (match("OR"))
			token.code = C_OR;
		else if (match("AND"))
			token.code = C_AND;
		else if (match("<="))
			token.code = C_le;
		else if (match(">="))
			token.code = C_ge;
		else if (match("<>"))
			token.code = C_ne;
		else if (match("THEN"))
			token.code = C_THEN;
		else if (match("TO"))
			token.code = C_TO;
		else if (match("STEP"))
			token.code = C_STEP;
		else if (match("LPRINT"))
			token.code = C_LPRINT;
		else if (match("LLIST"))
			token.code = C_LLIST;
		else if (match("STOP"))
			token.code = C_STOP;
		else if (match("SLOW"))
			token.code = C_SLOW;
		else if (match("FAST"))
			token.code = C_FAST;
		else if (match("NEW"))
			token.code = C_NEW;
		else if (match("SCROLL"))
			token.code = C_SCROLL;
		else if (match("CONT"))
			token.code = C_CONT;
		else if (match("DIM"))
			token.code = C_DIM;
		else if (match("REM")) {
			token.code = C_REM;
			line.tokens.push_back(token);

			skip_spaces();
			token.code = T_rem_code;
			token.bytes = encode_zx81(p);
		}
		else if (match("FOR"))
			token.code = C_FOR;
		else if (match("GOTO"))
			token.code = C_GOTO;
		else if (match("GOSUB"))
			token.code = C_GOSUB;
		else if (match("INPUT"))
			token.code = C_INPUT;
		else if (match("LOAD"))
			token.code = C_LOAD;
		else if (match("LIST"))
			token.code = C_LIST;
		else if (match("LET"))
			token.code = C_LET;
		else if (match("PAUSE"))
			token.code = C_PAUSE;
		else if (match("NEXT"))
			token.code = C_NEXT;
		else if (match("POKE"))
			token.code = C_POKE;
		else if (match("PRINT"))
			token.code = C_PRINT;
		else if (match("PLOT"))
			token.code = C_PLOT;
		else if (match("RUN"))
			token.code = C_RUN;
		else if (match("SAVE"))
			token.code = C_SAVE;
		else if (match("RAND"))
			token.code = C_RAND;
		else if (match("IF"))
			token.code = C_IF;
		else if (match("CLS"))
			token.code = C_CLS;
		else if (match("UNPLOT"))
			token.code = C_UNPLOT;
		else if (match("CLEAR"))
			token.code = C_CLEAR;
		else if (match("RETURN"))
			token.code = C_RETURN;
		else if (match("COPY"))
			token.code = C_COPY;
		else if (match("\\0c"))
			token.code = C_pound;
		else if (match("$"))
			token.code = C_dollar;
		else if (match(":"))
			token.code = C_colon;
		else if (match("?"))
			token.code = C_quest;
		else if (match("("))
			token.code = C_lparens;
		else if (match(")"))
			token.code = C_rparens;
		else if (match(">"))
			token.code = C_gt;
		else if (match("<"))
			token.code = C_lt;
		else if (match("="))
			token.code = C_eq;
		else if (match("+"))
			token.code = C_plus;
		else if (match("-"))
			token.code = C_minus;
		else if (match("*"))
			token.code = C_mult;
		else if (match("/"))
			token.code = C_div;
		else if (match(";"))
			token.code = C_semicolon;
		else if (match(","))
			token.code = C_comma;
		else if (match("_"))
			token.code = C_space;
		else if (parse_number(token.num, token.str))
			token.code = T_number;
		else if (parse_string(token.str))
			token.code = T_string;
		else if (parse_ident(token.ident))
			token.code = T_ident;
		else if (parse_line_addr_ref(token.ident))
			token.code = T_line_addr_ref;
		else if (parse_line_num_ref(token.ident))
			token.code = T_line_num_ref;
		else {
			g_errors.error("cannot parse", p);
			break;
		}

		line.tokens.push_back(token);
		skip_spaces();
	}

	Token token;
	token.code = C_newline;
	line.tokens.push_back(token);

	m_basic.lines.push_back(line);
}

void ZX81parser::parse_basic_var() {
	BasicVar var;
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
			g_errors.error("name too long", var.name);
			return;
		}
	}

	// get array marker and dimensions
	if (match("(")) {
		is_array = true;
		if (var.name.size() > 1) {
			g_errors.error("name too long", var.name);
			return;
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
				g_errors.error("name too long", var.name);
				return;
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
			if (!parse_end())
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
		if (!parse_end())
			goto error;
	}
	else if (is_string == true && is_array == false) {
		var.type = BasicVar::Type::String;

		if (!parse_string(var.str))
			goto error;
		if (!parse_end())
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
				g_errors.error("string length should be " + to_string(last_dimension));
				return;
			}
			var.strs.push_back(str);
		}
		if (!parse_end())
			goto error;
	}

	m_basic.vars.push_back(var);
	return;

error:
	g_errors.error("cannot parse", p);
}

//-----------------------------------------------------------------------------
// BASIC compiler
//-----------------------------------------------------------------------------

ZX81compiler::ZX81compiler(ZX81basic& basic, ZX81vm& result)
	: m_basic(basic), m_vm(result) {
}

void ZX81compiler::compile() {
	delete_empty_lines();
	compute_line_numbers();

	// init system vars 
	m_vm.poke_bytes(SAVE_ADDR, m_basic.sysvars);

	m_basic.labels.clear();
	int last_end_addr = 0;
	pass = 1;
	int num_passes_ok = 0;
	while (true) {
		compile_basic();

		// next pass
		if (pass > 1 && addr == last_end_addr) {
			num_passes_ok++;
			if (num_passes_ok >= 2)		// must run a second pass after end addr is ok
				break;
		}
		last_end_addr = addr;
		pass++;
	}

	compile_vars();
}

void ZX81compiler::compute_line_numbers() {
	int line_num = m_basic.auto_increment;
	int last_line = 0;
	for (auto& line : m_basic.lines) {
		if (line.line_num == 0) {
			line.line_num = line_num;
			line_num += m_basic.auto_increment;
		}
		else {
			line_num = line.line_num + m_basic.auto_increment;
		}

		if (line.line_num <= last_line)
			g_errors.error("line " + to_string(line.line_num) + " follows line " + to_string(last_line));
		else if (line.line_num > MaxLineNum)
			g_errors.error("line " + to_string(line.line_num) + " above maximum of " + to_string(MaxLineNum));
	}
}

void ZX81compiler::delete_empty_lines() {
	if (!m_basic.lines.empty()) {
		for (size_t i = 0; i < m_basic.lines.size() - 1; i++) {
			if (m_basic.lines[i].tokens.size() == 1 && m_basic.lines[i].tokens[0].code == C_newline) {
				if (!m_basic.lines[i].label.empty() && !m_basic.lines[i + 1].label.empty())
					g_errors.error("two labels on same line: " + m_basic.lines[i].label + " and " + m_basic.lines[i + 1].label);
				m_basic.lines[i + 1].label = m_basic.lines[i].label;
				m_basic.lines[i + 1].line_num = m_basic.lines[i].line_num;
				m_basic.lines.erase(m_basic.lines.begin() + i);
				i--;
			}
		}
	}
}

void ZX81compiler::compile_basic() {
	addr = PROG;

	for (auto& line : m_basic.lines) {
		// define label
		if (pass == 1 && !line.label.empty()) {
			auto it = m_basic.labels.find(line.label);
			if (it != m_basic.labels.end())
				g_errors.error("label redefinition", line.label);
			m_basic.labels[line.label] = &line;
		}

		Bytes bytes;
		for (auto& token : line.tokens) {
			switch (token.code) {
			case T_none:
				break;
			case T_number:
				compile_number(bytes, token.num);
				break;
			case T_string:
				compile_string(bytes, token.str);
				break;
			case T_ident:
				compile_ident(bytes, token.ident);
				break;
			case T_rem_code:
				bytes.insert(bytes.end(), token.bytes.begin(), token.bytes.end());
				break;
			case T_line_num_ref:
				if (pass == 1)
					compile_number(bytes, 0);
				else {
					auto it = m_basic.labels.find(token.ident);
					if (it == m_basic.labels.end())
						g_errors.error("label undefined", token.ident);
					else
						compile_number(bytes, it->second->line_num);
				}
				break;
			case T_line_addr_ref:
				if (pass == 1)
					compile_number(bytes, 0);
				else {
					auto it = m_basic.labels.find(token.ident);
					if (it == m_basic.labels.end())
						g_errors.error("label undefined", token.ident);
					else
						compile_number(bytes, it->second->addr);
				}
				break;
			default:
				assert(token.code < 0x100);
				bytes.push_back(token.code);
			}
		}

		// define addr and size
		line.addr = addr;
		line.size = static_cast<int>(bytes.size());

		m_vm.dpoke_be(addr, line.line_num); addr += 2;
		m_vm.dpoke(addr, line.size); addr += 2;
		addr += m_vm.poke_bytes(addr, bytes);
		assert(addr == line.addr + 4 + line.size);
	}

	m_vm.init_video_to_stkend(addr, m_basic.d_file_bytes, m_basic.e_line_bytes);

	if (m_basic.autostart) {
		int line_addr = m_vm.get_line_addr(m_basic.autostart);
		m_vm.dpoke(NXTLIN, line_addr);
	}
	else
		m_vm.dpoke(NXTLIN, m_vm.dpeek(D_FILE));
}

void ZX81compiler::compile_vars() {
	addr = m_vm.dpeek(VARS);

	Bytes str_bytes;
	array<uint8_t, 5> fp_bytes{ 0 };
	int last_dimension = 0;
	int size = 0;

	for (auto& var : m_basic.vars) {
		const char* p = var.name.c_str();
		Bytes name_bytes = encode_zx81(p);
		assert(name_bytes.size() > 0);

		switch (var.type) {
		case BasicVar::Type::Number:
			// put name
			if (var.name.size() == 1) {
				m_vm.poke(addr++, (name_bytes[0] & 0x3f) | 0x60);			// 011-letter
			}
			else {
				m_vm.poke(addr++, (name_bytes[0] & 0x3f) | 0xa0);			// 101-letter
				for (size_t i = 1; i < name_bytes.size() - 1; i++)
					m_vm.poke(addr++, name_bytes[i] & 0x3f);				// 001-letter
				m_vm.poke(addr++, (name_bytes.back() & 0x3f) | 0x80);		// 101-letter
			}

			// put value
			fp_bytes = float_to_zx81(var.value);
			addr += m_vm.poke_bytes(addr, fp_bytes);
			break;

		case BasicVar::Type::ArrayNumbers:
			// put name
			m_vm.poke(addr++, (name_bytes[0] & 0x1f) | 0x80);				// 100-letter

			// put dimensions
			size = 1
				+ 2 * static_cast<int>(var.dimensions.size())
				+ 5 * static_cast<int>(var.values.size());
			m_vm.dpoke(addr, size); addr += 2;
			m_vm.poke(addr++, var.dimensions.size() & 0xff);

			for (auto& dimension : var.dimensions) {
				m_vm.dpoke(addr, dimension); addr += 2;
			}

			// put values
			for (auto& value : var.values) {
				fp_bytes = float_to_zx81(value);
				addr += m_vm.poke_bytes(addr, fp_bytes);
			}
			break;

		case BasicVar::Type::ForNextLoop:
			// put name
			m_vm.poke(addr++, (name_bytes[0] & 0x3f) | 0xe0);				// 111-letter

			// put values
			fp_bytes = float_to_zx81(var.value);
			addr += m_vm.poke_bytes(addr, fp_bytes);

			fp_bytes = float_to_zx81(var.limit);
			addr += m_vm.poke_bytes(addr, fp_bytes);

			fp_bytes = float_to_zx81(var.step);
			addr += m_vm.poke_bytes(addr, fp_bytes);

			m_vm.dpoke(addr, var.line_num); addr += 2;
			break;

		case BasicVar::Type::String:
			// put name
			m_vm.poke(addr++, (name_bytes[0] & 0x1f) | 0x40);				// 010-letter

			m_vm.dpoke(addr, static_cast<int>(var.str.size())); addr += 2;

			p = var.str.c_str();
			str_bytes = encode_zx81(p);
			addr += m_vm.poke_bytes(addr, str_bytes);
			break;

		case BasicVar::Type::ArrayStrings:
			// put name
			m_vm.poke(addr++, (name_bytes[0] & 0x1f) | 0xc0);				// 110-letter

			// put dimensions
			last_dimension = var.dimensions.back();
			size = 1
				+ 2 * static_cast<int>(var.dimensions.size())
				+ last_dimension * static_cast<int>(var.strs.size());
			m_vm.dpoke(addr, size); addr += 2;
			m_vm.poke(addr++, static_cast<int>(var.dimensions.size()));

			for (auto& dimension : var.dimensions) {
				m_vm.dpoke(addr, dimension); addr += 2;
			}

			// put strings
			for (auto& str : var.strs) {
				p = str.c_str();
				str_bytes = encode_zx81(p);
				addr += m_vm.poke_bytes(addr, str_bytes);
			}
			break;

		default:
			assert(0);
		}
	}

	m_vm.poke(addr++, 0x80);
	m_vm.init_e_line_to_stkend(addr, m_basic.e_line_bytes);
}

void ZX81compiler::compile_number(Bytes& bytes, double value) {
	ostringstream oss;

	oss << value;
	string value_str = oss.str();
	const char* p = value_str.c_str();
	Bytes str_bytes = encode_zx81(p);
	array<uint8_t, 5> fp_bytes = float_to_zx81(value);
	bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
	bytes.push_back(C_number);
	bytes.insert(bytes.end(), fp_bytes.begin(), fp_bytes.end());
}

void ZX81compiler::compile_string(Bytes& bytes, const string& str) {
	const char* p = str.c_str();
	Bytes str_bytes = encode_zx81(p);
	bytes.push_back(C_dquote);
	bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
	bytes.push_back(C_dquote);
}

void ZX81compiler::compile_ident(Bytes& bytes, const string& ident) {
	const char* p = ident.c_str();
	Bytes str_bytes = encode_zx81(p);
	bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
}

//-----------------------------------------------------------------------------
// BASIC decompiler
//-----------------------------------------------------------------------------

ZX81decompiler::ZX81decompiler(ZX81vm& vm, ZX81basic& result)
	: m_vm(vm), m_basic(result) {
}

void ZX81decompiler::decompile() {
	decompile_sysvars();
	decompile_d_file();
	decompile_e_line();
	decompile_basic();
	decompile_vars();
}

void ZX81decompiler::decompile_sysvars() {
	m_basic.sysvars = m_vm.peek_bytes(VERSN, PROG - VERSN);

	// autostart
	int d_file = m_vm.dpeek(D_FILE);
	int nxtlin = m_vm.dpeek(NXTLIN);
	if (nxtlin >= d_file)
		m_basic.autostart = 0;
	else
		m_basic.autostart = m_vm.dpeek_be(nxtlin);

	// fast
	if ((m_vm.peek(CDFLAG) & FlagSlow) == FlagSlow)
		m_basic.fast = false;
	else
		m_basic.fast = true;
}

void ZX81decompiler::decompile_d_file() {
	int d_file = m_vm.dpeek(D_FILE);
	int vars = m_vm.dpeek(VARS);
	m_basic.d_file_bytes = m_vm.peek_bytes(d_file, vars - d_file);
}

void ZX81decompiler::decompile_e_line() {
	int e_line= m_vm.dpeek(E_LINE);
	int stkbot = m_vm.dpeek(STKBOT);
	m_basic.e_line_bytes = m_vm.peek_bytes(e_line, stkbot - e_line);
}

void ZX81decompiler::decompile_basic() {
	m_basic.lines.clear();
	addr = PROG;
	while (addr < m_vm.dpeek(D_FILE)) {
		BasicLine line;
		line.addr = addr;
		line.line_num = m_vm.dpeek_be(addr);
		line.size = m_vm.dpeek(addr + 2);

		addr += 4;
		end = addr + line.size;

		decompile_basic_line(line);
		m_basic.lines.push_back(line);
	}
}

void ZX81decompiler::decompile_basic_line(BasicLine& line) {
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
				token.code = m_vm.peek(addr++);
				line.tokens.push_back(token);
			}
		}
		decompile_newline(line);
	}
}

bool ZX81decompiler::decompile_rem_code(BasicLine& line) {
	if (m_vm.peek(addr) == C_REM) {
		bool is_code = false;
		for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
			if ((m_vm.peek(p) & 0x40) == 0x40) {			// special char
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
				bytes.bytes.push_back(m_vm.peek(p));
			line.tokens.push_back(bytes);

			addr = end - 1;
			decompile_newline(line);
			return true;
		}
	}
	return false;
}

bool ZX81decompiler::decompile_number(BasicLine& line) {
	Token token;
	string num;

	// get mantissa
	int p = addr;
	int num_dots = 0;
	int num_digits = 0;
	while (true) {
		int c = m_vm.peek(p);
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
	int c = m_vm.peek(p);
	if (p == C_E) {
		p++;
		num.push_back('E');
		c = m_vm.peek(p);
		if (c == C_plus || c == C_minus) {
			p++;
			num.push_back(c == C_plus ? '+' : '-');
		}
		c = m_vm.peek(p);
		if (c < C_0 || c > C_9)
			return false;
		while (c >= C_0 && c <= C_9) {
			p++;
			num.push_back(c - C_0 + '0');
			c = m_vm.peek(p);
		}
	}

	double value1 = atof(num.c_str());

	// get number marker
	c = m_vm.peek(p);
	if (c != C_number)
		return false;
	p++;

	// get fp value
	double value2 = m_vm.fpeek(p); p += 5;

	if (abs(value1 - value2) > 1e-6)
		g_errors.error("number " + to_string(value1) + " != " + to_string(value2));

	token.code = T_number;
	token.str = num;
	token.num = value1;
	line.tokens.push_back(token);
	addr = p;
	return true;
}

bool ZX81decompiler::decompile_ident(BasicLine& line) {
	Token token;
	int p = addr;
	while (true) {
		int c = m_vm.peek(p);
		if (c < C_A || c > C_Z)
			break;
		token.ident.push_back(c - C_A + 'A');
		p++;
	}
	if (p == addr)					// no letters found
		return false;
	else {
		token.code = T_ident;
		line.tokens.push_back(token);
		addr = p;
		return true;
	}
}

bool ZX81decompiler::decompile_string(BasicLine& line) {
	Token token;
	int p = addr;
	if (m_vm.peek(p) != C_dquote)
		return false;
	p++;
	while (true) {
		int c = m_vm.peek(p);
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

void ZX81decompiler::decompile_newline(BasicLine& line) {
	Token token;
	token.code = m_vm.peek(addr++);
	if (token.code != C_newline)
		g_errors.error("missing newline");

	line.tokens.push_back(token);
}

void ZX81decompiler::decompile_vars() {
	addr = m_vm.dpeek(VARS);
	int c = 0;
	while ((c = m_vm.peek(addr)) != 0x80) {
		BasicVar var;
		var.addr = addr;
		int addr0 = addr;

		if ((c & 0xe0) == 0x60) {		// single letter variable
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::Number;
			var.name = decode_zx81(c);
			var.value = m_vm.fpeek(addr); addr += 5;
		}
		else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
			var.type = BasicVar::Type::Number;

			// first letter
			addr++;
			c &= 0x3f;
			c |= 0x20;
			var.name = decode_zx81(c);

			// second, ... letter
			while (((c = m_vm.peek(addr)) & 0xc0) == 0x00) {
				addr++;
				c &= 0x3f;
				c |= 0x20;
				var.name += decode_zx81(c);
			}

			// last letter
			c = m_vm.peek(addr);
			if ((c & 0xc0) != 0x80) {
				g_errors.error("invalid multi-letter variable", fmt_hex(c, 2));
				return;
			}
			else {
				addr++;
				c &= 0x3f;
				c |= 0x20;
				var.name += decode_zx81(c);
			}
			var.value = m_vm.fpeek(addr); addr += 5;
		}
		else if ((c & 0xe0) == 0x80) {	// array of numbers
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::ArrayNumbers;
			var.name = decode_zx81(c);

			int size = m_vm.dpeek(addr); addr += 2;
			int addr0 = addr;

			int num_dimensions = m_vm.peek(addr++);
			int num_elements = 1;
			for (int i = 0; i < num_dimensions; i++) {
				int dimension = m_vm.dpeek(addr); addr += 2;
				num_elements *= dimension;
				var.dimensions.push_back(dimension);
			}
			for (int i = 0; i < num_elements; i++) {
				double value = m_vm.fpeek(addr); addr += 5;
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

			var.value = m_vm.fpeek(addr); addr += 5;
			var.limit = m_vm.fpeek(addr); addr += 5;
			var.step = m_vm.fpeek(addr); addr += 5;
			var.line_num = m_vm.dpeek(addr); addr += 2;
		}
		else if ((c & 0xe0) == 0x40) {	// string
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::String;
			var.name = decode_zx81(c);

			int size = m_vm.dpeek(addr); addr += 2;
			for (int i = 0; i < size; i++) {
				c = m_vm.peek(addr++);
				var.str += decode_zx81(c);
			}
		}
		else if ((c & 0xe0) == 0xc0) {	// array of strings
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::ArrayStrings;
			var.name = decode_zx81(c);

			int size = m_vm.dpeek(addr); addr += 2;
			int addr0 = addr;

			int num_dimensions = m_vm.peek(addr++);
			int num_elements = 1;
			for (int i = 0; i < num_dimensions; i++) {
				int dimension = m_vm.dpeek(addr); addr += 2;
				num_elements *= dimension;
				var.dimensions.push_back(dimension);
			}

			int last_dimension = var.dimensions.back();		// size of each string
			num_elements /= last_dimension;

			for (int i = 0; i < num_elements; i++) {
				string str;
				for (int j = 0; j < last_dimension; j++) {
					c = m_vm.peek(addr++);
					str += decode_zx81(c);
				}
				var.strs.push_back(str);
			}

			assert(addr0 + size == addr);
		}
		else {
			g_errors.error("invalid variable marker", string("$") + fmt_hex(c, 2));
			break;
		}

		var.size = addr - addr0;

		m_basic.vars.push_back(var);
	}
}
