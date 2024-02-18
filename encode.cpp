//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "encode.h"
#include "errors.h"
#include "utils.h"
#include <cstring>
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

Bytes encode_zx81(const string& str) {
	const char* p = str.c_str();
	return encode_zx81(p);
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
				error("cannot encode", p);
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

