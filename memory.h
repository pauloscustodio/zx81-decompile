//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include <array>
#include <string>
using namespace std;

class Memory {
public:
	Memory();

	void clear();

	// read/write .p file
	void read_p_file(const string& filename);
	void write_p_file(const string& filename);

	// read memory
	int peek(int addr);
	int dpeek(int addr);
	int speek(int addr);
	int dpeek_be(int addr);
	double fpeek(int addr);
	Bytes peek_bytes(int addr, int size);
	string peek_hex(int addr, int len);
	int get_line_addr(int line_num);

	// write memory
	void poke(int addr, int value);
	void dpoke(int addr, int value);
	void dpoke_be(int addr, int value);
	void fpoke(int addr, double value);
	int poke_bytes(int addr, const Bytes& bytes);
	int poke_bytes(int addr, const Byte* data, int size);

	static Bytes get_empty_d_file();
	static Bytes get_empty_e_line();

	void init_video_to_stkend(int addr, const Bytes& d_file_bytes, const Bytes& e_line_bytes);
	void init_e_line_to_stkend(int addr, const Bytes& e_line_bytes);

private:
	array<Byte, MEM_SIZE> mem{ 0 };

	int write_empty_d_file(int addr);
	int write_empty_vars(int addr);
};

void clear_memory();
void read_p_file(const string& filename);
void write_p_file(const string& filename);
int peek(int addr);
int dpeek(int addr);
int speek(int addr);
int dpeek_be(int addr);
double fpeek(int addr);
Bytes peek_bytes(int addr, int size);
string peek_hex(int addr, int len);
int get_line_addr(int line_num);
void poke(int addr, int value);
void dpoke(int addr, int value);
void dpoke_be(int addr, int value);
void fpoke(int addr, double value);
int poke_bytes(int addr, const Bytes& bytes);
int poke_bytes(int addr, const Byte* data, int size);
Bytes get_empty_d_file();
Bytes get_empty_e_line();
void init_video_to_stkend(int addr, const Bytes& d_file_bytes, const Bytes& e_line_bytes);
void init_e_line_to_stkend(int addr, const Bytes& e_line_bytes);
