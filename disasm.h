//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class AsmLabels {
public:
	AsmLabels();

	void add(const string& name, int value);
	int get(const string& name);

	bool find(const string& name, int& value) const;
	bool find(int value, string& name) const;

	void clear();

	auto begin() { return by_name.begin(); }
	auto end() { return by_name.end(); }
	auto cbegin() const { return by_name.cbegin(); }
	auto cend() const { return by_name.cend(); }
	auto size() const { return by_name.size(); }

private:
	unordered_map<string, int> by_name;
	unordered_map<int, string> by_value;
};

extern AsmLabels g_asm_labels;

struct Opcode {
	enum class Type { Undef, Unknown, Asm, AsmData, Defb, DefbData, Defw, DefwData, Defm, DefmData };
	Type type{ Type::Unknown };
	int addr{ 0 };
	int size{ 0 };
	string opcode;			// normalized form, e.g. "jp NN"
	string refer_to;		// label to refer to instead of nn
	int n{ 0 };
	int nn{ 0 };
	int dis{ 0 };
	bool is_jump{ false };
	bool ends_flow{ false };
	vector<int> values;		// for defb, defw
	string str;				// for defm

	Opcode(Type type_ = Type::Unknown, int addr_ = 0, int size_ = 1);
	string to_string();

private:
	string decode_opcode();
	string decode_undef();
	string decode_defb();
	string decode_defw();
	string decode_defm();
	string decode_label(int value);
};

class DisasmCode {
public:
	DisasmCode();
	virtual ~DisasmCode();
	DisasmCode(const DisasmCode& other) = delete;
	DisasmCode& operator=(const DisasmCode& other) = delete;

	Opcode::Type get_type(int addr);
	Opcode* get(int addr);
	string get_label(int addr);
	string get_header(int addr);
	string get_comment(int addr);

	void set_unknown(int addr, int size);
	void set_defb(int addr, int count);
	void set_defw(int addr, int count);
	void set_defm(int addr, int len);
	void set_code(int addr);
	void add_header(int addr, const string& line);
	void set_comment(int addr, const string& text);
	
private:
	array<Opcode*, MEM_SIZE> opcodes{ 0 };
	array<string*, MEM_SIZE> headers{ 0 };
	array<string*, MEM_SIZE> comments{ 0 };

	bool check_range_unknown(int addr, int size);
};

extern DisasmCode g_disasm_code;

class Disasm {
public:
	Opcode disasm(int addr_);

private:
	int addr{ 0 };
	Opcode opcode;

	static string dd1(int n, int x);
	static string dd2(int n, int x);
	static string r1(int n, int x);
	static string x1(int x);
	static string flags1(int n);
	static string flags2(int n);
	static string alu1(int n);
	static string rot1(int n);
	static string bit1(int n);

	void collect_nn();
	void collect_n();
	void collect_dis();
	void collect_jr();
};
