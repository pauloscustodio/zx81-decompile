//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "consts.h"
#include "basic.h"
#include "encode.h"
#include "errors.h"
#include "memory.h"
#include "zfloat.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
using namespace std;

static Memory g_mem;

//-----------------------------------------------------------------------------
// init memory
//-----------------------------------------------------------------------------

Memory::Memory() {
	clear();
}

void Memory::clear() {
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

int Memory::write_empty_d_file(int addr) {
	Bytes d_file = get_empty_d_file();
	addr = poke_bytes(addr, d_file);
	return addr;
}

int Memory::write_empty_vars(int addr) {
	poke(addr++, 0x80);
	return addr;
}

Bytes Memory::get_empty_d_file() {
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

Bytes Memory::get_empty_e_line() {
	Bytes out;
	return out;
}

void Memory::init_video_to_stkend(int addr, const Bytes& d_file_bytes, const Bytes& e_line_bytes) {
	dpoke(D_FILE, addr);
	dpoke(NXTLIN, addr);
	if (dpeek(DF_CC) < addr + 1 || dpeek(DF_CC) >= addr + 1 + NumRows * (NumCols + 1))
		dpoke(DF_CC, addr + 1);
	addr = poke_bytes(addr, d_file_bytes);

	// vars
	dpoke(VARS, addr);
	poke(addr++, 0x80);

	init_e_line_to_stkend(addr, e_line_bytes);
}

void Memory::init_e_line_to_stkend(int addr, const Bytes& e_line_bytes) {
	// edit line
	dpoke(E_LINE, addr);
	addr = poke_bytes(addr, e_line_bytes);

	// calculator stack
	dpoke(STKBOT, addr);
	dpoke(STKEND, addr);
}

//-----------------------------------------------------------------------------
// read/write binary files
//-----------------------------------------------------------------------------

void Memory::read_p_file(const string& filename) {
	// open file
	ifstream ifs(filename, ios::binary);
	if (!ifs.is_open()) {
		perror(filename.c_str());
		fatal_error("open file", filename);
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
		fatal_error("read " + to_string(size) + " bytes from file " + filename);
	}
}

void Memory::write_p_file(const string& filename) {
	// open file
	ofstream ofs(filename, ios::binary);
	if (!ofs.is_open()) {
		perror(filename.c_str());
		fatal_error("open file", filename);
	}

	size_t size = dpeek(E_LINE) - SAVE_ADDR;
	ofs.write(reinterpret_cast<const char*>(&mem[SAVE_ADDR]), size);
	size_t written = ofs.tellp();
	if (written != size) {
		perror(filename.c_str());
		fatal_error("write " + to_string(size) + " bytes to file " + filename);
	}
}

//-----------------------------------------------------------------------------
// read memory
//-----------------------------------------------------------------------------

int Memory::peek(int addr) {
	return mem[addr & 0xffff];
}

int Memory::dpeek(int addr) {
	return peek(addr) + (peek(addr + 1) << 8);
}

int Memory::speek(int addr) {
	int n = peek(addr);
	if (n >= 0x80)
		n -= 0x100;
	return n;
}

int Memory::dpeek_be(int addr) {
	return (peek(addr) << 8) + peek(addr + 1);
}

double Memory::fpeek(int addr) {
	array<Byte, 5> bytes{ 0 };
	for (int i = 0; i < 5; i++)
		bytes[i] = peek(addr + i);
	return zx81_to_float(bytes);
}

Bytes Memory::peek_bytes(int addr, int size) {
	Bytes out;
	out.insert(out.end(), mem.begin() + addr, mem.begin() + addr + size);
	return out;
}

string Memory::peek_hex(int addr, int len) {
	Bytes out;
	for (int i = 0; i < len; i++)
		out.push_back(peek(addr + i));
	return encode_hex(out);
}

int Memory::get_line_addr(int line_num) {
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
// write memory
//-----------------------------------------------------------------------------

void Memory::poke(int addr, int value) {
	mem[addr & 0xffff] = value & 0xff;
}

void Memory::dpoke(int addr, int value) {
	poke(addr, value);
	poke(addr + 1, value >> 8);
}

void Memory::dpoke_be(int addr, int value) {
	poke(addr, value >> 8);
	poke(addr + 1, value);
}

void Memory::fpoke(int addr, double value) {
	array<Byte, 5> bytes = float_to_zx81(value);
	for (int i = 0; i < 5; i++)
		poke(addr + i, bytes[i]);
}

int Memory::poke_bytes(int addr, const Bytes& bytes) {
	return poke_bytes(addr, bytes.data(), INT(bytes.size()));
}

int Memory::poke_bytes(int addr, const Byte* data, int size) {
	while (size-- > 0)
		poke(addr++, *(data++));
	return addr;
}

void clear_memory() {
	g_mem.clear();
}

void read_p_file(const string& filename) {
	g_mem.read_p_file(filename);
}

void write_p_file(const string& filename) {
	g_mem.write_p_file(filename);
}

int peek(int addr) {
	return g_mem.peek(addr);
}

int dpeek(int addr) {
	return g_mem.dpeek(addr);
}

int speek(int addr) {
	return g_mem.speek(addr);
}

int dpeek_be(int addr) {
	return g_mem.dpeek_be(addr);
}

double fpeek(int addr) {
	return g_mem.fpeek(addr);
}

Bytes peek_bytes(int addr, int size) {
	return g_mem.peek_bytes(addr, size);
}

string peek_hex(int addr, int len) {
	return g_mem.peek_hex(addr, len);
}

int get_line_addr(int line_num) {
	return g_mem.get_line_addr(line_num);
}

void poke(int addr, int value) {
	g_mem.poke(addr, value);
}

void dpoke(int addr, int value) {
	g_mem.dpoke(addr, value);
}

void dpoke_be(int addr, int value) {
	g_mem.dpoke_be(addr, value);
}

void fpoke(int addr, double value) {
	g_mem.fpoke(addr, value);
}

int poke_bytes(int addr, const Bytes& bytes) {
	return g_mem.poke_bytes(addr, bytes);
}

int poke_bytes(int addr, const Byte* data, int size) {
	return g_mem.poke_bytes(addr, data, size);
}

Bytes get_empty_d_file() {
	return Memory::get_empty_d_file();
}

Bytes get_empty_e_line() {
	return Memory::get_empty_e_line();
}

void init_video_to_stkend(int addr, const Bytes& d_file_bytes, const Bytes& e_line_bytes) {
	g_mem.init_video_to_stkend(addr, d_file_bytes, e_line_bytes);
}

void init_e_line_to_stkend(int addr, const Bytes& e_line_bytes) {
	g_mem.init_e_line_to_stkend(addr, e_line_bytes);
}
