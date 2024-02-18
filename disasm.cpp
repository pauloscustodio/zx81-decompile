//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disasm.h"
#include "encode.h"
#include "errors.h"
#include "memory.h"
#include "utils.h"
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

//-----------------------------------------------------------------------------
// asm labels
//-----------------------------------------------------------------------------

AsmLabels g_asm_labels;

AsmLabels::AsmLabels() {
#define X(name, value)		add(#name, value);
#include "consts.def"
}

void AsmLabels::add(const string& name, int value) {
	auto it = by_name.find(name);
	if (it != by_name.end())
		error("label redefinition", name + " = $" + fmt_hex(value, 4));

	by_name[name] = value;
	by_value[value] = name;		// overwrite if tow labels with same value
	return;
}

int AsmLabels::get(const string& name) {
	int value = 0;
	if (find(name, value))
		return value;
	else {
		error("label undefined", name);
		return 0;
	}
}

bool AsmLabels::find(const string& name, int& value) const {
	auto it1 = by_name.find(name);
	if (it1 == by_name.end()) {
		return false;
	}
	else {
		value = it1->second;
		return true;
	}
}

bool AsmLabels::find(int value, string& name) const {
	auto it2 = by_value.find(value);
	if (it2 == by_value.end()) {
		return false;
	}
	else {
		name = it2->second;
		return true;
	}
}

void AsmLabels::clear() {
	by_name.clear();
	by_value.clear();
}

//-----------------------------------------------------------------------------
// opcode
//-----------------------------------------------------------------------------

Opcode::Opcode(Type type_, int addr_, int size_)
	: type(type_), addr(addr_), size(size_) {
}

string Opcode::to_string() {
	ostringstream oss;

	// header
	string header = g_disasm_code.get_header(addr);
	if (!header.empty())
		oss << endl << header << endl;

	// label
	string label;
	if (g_asm_labels.find(addr, label))
		oss << label << ":" << endl;

	// opcode
	oss << setw(8) << "" << setw(24) << left;
	switch (type) {
	case Type::Undef:
	case Type::Unknown:
		oss << decode_undef();
		break;
	case Type::Asm:
		oss << decode_opcode();
		break;
	case Type::Defb:
		oss << decode_defb();
		break;
	case Type::Defw:
		oss << decode_defw();
		break;
	case Type::Defm:
		oss << decode_defm();
		break;
	default:
		assert(0);
	}

	// comment
	oss << setw(0) << "; ";
	string comment = g_disasm_code.get_comment(addr);
	if (comment.empty())
		oss << "[$" << fmt_hex(addr, 4) << "]";
	else
		oss << comment;
	oss << endl;

	return oss.str();
}

string Opcode::decode_opcode() {
	// get opcode
	string instr, args;
	auto p = opcode.find(' ');
	if (p == string::npos) {
		instr = opcode;
		args = "";
	}
	else {
		instr = opcode.substr(0, p);
		args = opcode.substr(p + 1);
	}

	// get arguments
	if (args.find("+DIS") != string::npos) {
		string s;
		if (dis > 0)
			s = string("+") + std::to_string(dis);
		else if (dis < 0)
			s = std::to_string(dis);
		else
			s = "";
		args = str_replace_all(args, "+DIS", s);
	}

	if (args.find("NN") != string::npos) {
		string s;
		int value;
		string label;
		if (!refer_to.empty() && g_asm_labels.find(refer_to, value)) {
			int offset = nn - value;
			if (offset < 0)
				s = refer_to + "-$" + fmt_hex(-offset, 4);
			else if (n > 0)
				s += refer_to + "+$" + fmt_hex(offset, 4);
			else
				s = refer_to;
		}
		else {
			s = decode_label(nn);
		}

		args = str_replace_all(args, "NN", s);
	}

	if (args.find("N") != string::npos) {
		string s = string("$") + fmt_hex(n, 2);
		args = str_replace_all(args, "N", s);
	}

	// result
	ostringstream oss;
	oss << setw(8) << left << instr
		<< setw(0) << left << args;
	return oss.str();
}

string Opcode::decode_undef() {
	ostringstream oss;
	oss << setw(8) << left << "defb"
		<< setw(0) << left << "$" << fmt_hex(peek(addr), 2);
	return oss.str();
}

string Opcode::decode_defb() {
	ostringstream oss;
	oss << setw(8) << left << "defb"
		<< setw(0) << left;
	bool first = true;
	for (auto& value : values) {
		if (!first)
			oss << ", ";
		first = false;

		oss << "$" << fmt_hex(value, 2);
	}
	return oss.str();
}

string Opcode::decode_defw() {
	ostringstream oss;
	oss << setw(8) << left << "defw"
		<< setw(0) << left;
	bool first = true;
	for (auto& value : values) {
		if (!first)
			oss << ", ";
		first = false;

		oss << decode_label(value);
	}
	return oss.str();
}

string Opcode::decode_defm() {
	ostringstream oss;
	oss << setw(8) << left << "defm"
		<< setw(0) << left
		<< "\"";
	for (auto& c : str) {
		if (c == '"')
			oss << "\\\"";
		else
			oss << c;
	}
	oss << "\"";
	return oss.str();
}

string Opcode::decode_label(int value) {
	ostringstream oss;
	string label;
	if (g_asm_labels.find(value, label))
		oss << label;
	else if (g_asm_labels.find(value - 1, label))
		oss << label << "+1";
	else if (g_asm_labels.find(value - 2, label))
		oss << label << "+2";
	else
		oss << "$" << fmt_hex(value, 4);
	return oss.str();
}

//-----------------------------------------------------------------------------
// disassembled code
//-----------------------------------------------------------------------------

DisasmCode g_disasm_code;

DisasmCode::DisasmCode() {
}

DisasmCode::~DisasmCode() {
	for (auto& opc : opcodes)
		if (opc)
			delete opc;
	for (auto& header : headers)
		if (header)
			delete header;
	for (auto& comment : comments)
		if (comment)
			delete comment;
}

Opcode::Type DisasmCode::get_type(int addr) {
	addr &= 0xffff;
	Opcode* opc = opcodes[addr];
	if (opc == nullptr)
		return Opcode::Type::Undef;
	else
		return opc->type;
}

Opcode* DisasmCode::get(int addr) {
	addr &= 0xffff;
	Opcode* opc = opcodes[addr];
	if (opc == nullptr)
		opc = opcodes[addr] = new Opcode(Opcode::Type::Unknown, addr, 1);
	return opc;
}

string DisasmCode::get_label(int addr) {
	string name;
	if (!g_asm_labels.find(addr, name)) {
		name = "L" + fmt_hex(addr, 4);
		g_asm_labels.add(name, addr);
	}
	return name;
}

string DisasmCode::get_header(int addr) {
	addr &= 0xffff;
	string* header = headers[addr];
	if (header == nullptr)
		return "";
	else
		return *header;
}

string DisasmCode::get_comment(int addr) {
	addr &= 0xffff;
	string* comment = comments[addr];
	if (comment == nullptr)
		return "";
	else
		return *comment;
}

void DisasmCode::set_unknown(int addr, int size) {
	for (int p = addr; p < addr + size; p++) {
		Opcode* opc = get(p);
		*opc = Opcode(Opcode::Type::Unknown, p & 0xffff, 1);
	}
}

void DisasmCode::set_defb(int addr, int count) {
	if (check_range_unknown(addr, 1 * count)) {
		Opcode* opc = get(addr);
		*opc = Opcode(Opcode::Type::Defb, addr & 0xffff, 1 * count);
		for (int p = addr; p < addr + 1 * count; p++)
			opc->values.push_back(peek(p));
		for (int p = addr + 1; p < addr + 1 * count; p++) {
			opc = get(addr);
			*opc = Opcode(Opcode::Type::DefbData, addr & 0xffff, 1);
		}
	}
}

void DisasmCode::set_defw(int addr, int count) {
	if (check_range_unknown(addr, 2 * count)) {
		Opcode* opc = get(addr);
		*opc = Opcode(Opcode::Type::Defw, addr & 0xffff, 2 * count);
		for (int p = addr; p < addr + 2 * count; p += 2)
			opc->values.push_back(dpeek(p));
		for (int p = addr + 1; p < addr + 2 * count; p++) {
			opc = get(addr);
			*opc = Opcode(Opcode::Type::DefwData, addr & 0xffff, 1);
		}
	}
}

void DisasmCode::set_defm(int addr, int len) {
	if (check_range_unknown(addr, len)) {
		Opcode* opc = get(addr);
		*opc = Opcode(Opcode::Type::Defm, addr & 0xffff, len);
		for (int p = addr; p < addr + len; p++)
			opc->str += decode_zx81(peek(p));
		for (int p = addr + 1; p < addr + len; p++) {
			opc = get(addr);
			*opc = Opcode(Opcode::Type::DefmData, addr & 0xffff, 1);
		}
	}
}

void DisasmCode::set_code(int addr) {
	Disasm dis;
	while (true) {
		Opcode* opc = get(addr);
		if (opc->type == Opcode::Type::Asm)
			return;

		Opcode opcode = dis.disasm(addr);
		if (opcode.opcode.empty()) {
			error("invalid disassembly at", string("$") + fmt_hex(addr, 4));
			return;
		}

		*opc = opcode;
		for (int p = addr + 1; p < addr + opc->size; p++)
			*(get(p)) = Opcode(Opcode::Type::AsmData, p & 0xffff, 1);

		if (opc->is_jump) {
			get_label(opc->nn);
			if (get_type(opc->nn) != Opcode::Type::Undef)
				set_code(opc->nn);		// recurse for branches
		}

		addr += opc->size;

		if (opc->ends_flow)
			break;
	}
}

void DisasmCode::add_header(int addr, const string& line) {
	addr &= 0xffff;
	string* header = headers[addr];
	if (header == nullptr)
		header = headers[addr] = new string(line);
	else
		*header += string("\n") + line;
}

void DisasmCode::set_comment(int addr, const string& text) {
	addr &= 0xffff;
	string* comment = comments[addr];
	if (comment == nullptr)
		comment = comments[addr] = new string(text);
	else
		*comment = text;
}

bool DisasmCode::check_range_unknown(int addr, int size) {
	for (int p = addr; p < addr + size; p++) {
		Opcode::Type type = get_type(p);
		if (type != Opcode::Type::Undef && type != Opcode::Type::Unknown) {
			error("memory address redefinition", string("$") + fmt_hex(addr, 4));
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// disassembler
//-----------------------------------------------------------------------------

string Disasm::dd1(int n, int x) {
	const char* n_lut[] = { "bc","de","hl","sp" };
	const char* x_lut[] = { "hl","ix","iy" };
	n &= 0x03;
	if (n == 2)
		return x_lut[x];
	else
		return n_lut[n];
}

string Disasm::dd2(int n, int x) {
	const char* n_lut[] = { "bc","de","hl","af" };
	const char* x_lut[] = { "hl","ix","iy" };
	n &= 0x03;
	if (n == 2)
		return x_lut[x];
	else
		return n_lut[n];
}

string Disasm::r1(int n, int x) {
	const char* n_lut[] = { "b","c","d","e","h","l","(hl)","a" };
	const char* x_lut[] = { "(hl)","(ix+DIS)","(iy+DIS)" };
	n &= 0x07;
	if (n == 6)
		return x_lut[x];
	else
		return n_lut[n];
}

string Disasm::x1(int x) {
	const char* x_lut[] = { "hl","ix","iy" };
	return x_lut[x];
}

string Disasm::flags1(int n) {
	const char* n_lut[] = { "nz","z","nc","c","po","pe","p","m" };
	n &= 0x07;
	return n_lut[n];
}

string Disasm::flags2(int n) {
	n &= 0x03;
	return flags1(n);
}

string Disasm::alu1(int n) {
	const char* n_lut[] = { "add a, ","adc a, ","sub ","sbc a, ","and ","xor ","or ","cp " };
	n &= 0x07;
	return n_lut[n];
}

string Disasm::rot1(int n) {
	const char* n_lut[] = { "rlc ","rrc ","rl ","rr ","sla ","sra ","sll ","srl " };
	n &= 0x07;
	return n_lut[n];
}

string Disasm::bit1(int n) {
	const char* n_lut[] = { "","bit ","res ","set " };
	n &= 0x03;
	return n_lut[n];
}

void Disasm::collect_nn() {
	opcode.nn = dpeek(addr); 
	addr += 2;
}

void Disasm::collect_n() {
	opcode.n = peek(addr++);
}

void Disasm::collect_dis() {
	opcode.dis = speek(addr++);
}

void Disasm::collect_jr() {
	opcode.n = speek(addr++);
	opcode.nn = addr + opcode.n;
}

Opcode Disasm::disasm(int addr_) {
	addr = addr_;
	opcode = Opcode(Opcode::Type::Asm, addr, 0);

	int x = 0;		// hl, ix, iy
	bool done = false;
	while (!done) {
		done = true;
		int b = peek(addr++);
		switch (b) {
		case 0x00: 
			opcode.opcode = "nop"; 
			break;
		case 0x01: case 0x11: case 0x21: case 0x31:
			opcode.opcode = string("ld ") + dd1(b >> 4, x) + ", NN";
			collect_nn();
			break;
		case 0x02: case 0x12:
			opcode.opcode = string("ld (") + dd1(b >> 4, 0) + "), a";
			break;
		case 0x03: case 0x13: case 0x23: case 0x33:
			opcode.opcode = string("inc ") + dd1(b >> 4, x);
			break;
		case 0x04: case 0x0c: case 0x14: case 0x1c: case 0x24: case 0x2c: case 0x34: case 0x3c:
			if (b == 0x34 && x != 0)
				collect_dis();
			opcode.opcode = string("inc ") + r1(b >> 3, x);
			break;
		case 0x05: case 0x0d: case 0x15: case 0x1d: case 0x25: case 0x2d: case 0x35: case 0x3d:
			if (b == 0x35 && x != 0)
				collect_dis();
			opcode.opcode = string("dec ") + r1(b >> 3, x);
			break;
		case 0x06: case 0x0e: case 0x16: case 0x1e: case 0x26: case 0x2e: case 0x36: case 0x3e:
			if (b == 0x36 && x != 0)
				collect_dis();
			opcode.opcode = string("ld ") + r1(b >> 3, x) + ", N";
			collect_n();
			break;
		case 0x07:
			opcode.opcode = "rlca"; 
			break;
		case 0x08:
			opcode.opcode = "ex af, af'";
			break;
		case 0x09: case 0x19: case 0x29: case 0x39:
			opcode.opcode = string("add ") + x1(x) + ", " + dd1(b >> 4, x); 
			break;
		case 0x0a: case 0x1a:
			opcode.opcode = string("ld a, (") + dd1(b >> 4, 0) + ")";
			break;
		case 0x0b: case 0x1b: case 0x2b: case 0x3b:
			opcode.opcode = string("dec ") + dd1(b >> 4, x);
			break;
		case 0x0f:
			opcode.opcode = "rrca"; 
			break;
		case 0x10:
			opcode.opcode = "djnz NN";
			collect_jr();
			opcode.is_jump = true;
			break;
		case 0x17:
			opcode.opcode = "rla"; 
			break;
		case 0x18:
			opcode.opcode = "jr NN";
			collect_jr();
			opcode.is_jump = true;
			opcode.ends_flow = true;
			break;
		case 0x1f:
			opcode.opcode = "rra"; 
			break;
		case 0x20: case 0x28: case 0x30: case 0x38:
			opcode.opcode = string("jr ") + flags2(b >> 3) + ", NN";
			collect_jr();
			opcode.is_jump = true;
			break;
		case 0x22:
			opcode.opcode = string("ld (NN), ") + x1(x);
			collect_nn();
			break;
		case 0x27:
			opcode.opcode = "daa"; 
			break;
		case 0x2a:
			opcode.opcode = string("ld ") + x1(x) + ", (NN)";
			collect_nn();
			break;
		case 0x2f:
			opcode.opcode = "cpl";
			break;
		case 0x32:
			opcode.opcode = "ld (NN), a";
			collect_nn();
			break;
		case 0x37:
			opcode.opcode = "scf"; 
			break;
		case 0x3a:
			opcode.opcode = "ld a, (NN)";
			collect_nn();
			break;
		case 0x3f:
			opcode.opcode = "ccf"; 
			break;
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75:            case 0x77:
		case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
			if (x != 0)
				if ((b & 0x07) == 0x06 || (b & 0x38) == 0x30)
					collect_dis();
			opcode.opcode = string("ld ") + r1(b >> 3, x) + ", " + r1(b, x);
			break;
		case 0x76:
			opcode.opcode = "halt";
			break;
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
		case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
		case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
		case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
		case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
			if (x != 0)
				if ((b & 0x07) == 0x06)
					collect_dis();
			opcode.opcode = alu1(b >> 3) + r1(b, x);
			break;
		case 0xc0: case 0xc8: case 0xd0: case 0xd8: case 0xe0: case 0xe8: case 0xf0: case 0xf8:
			opcode.opcode = string("ret ") + flags1(b >> 3);
			break;
		case 0xc1: case 0xd1: case 0xe1: case 0xf1:
			opcode.opcode = string("pop ") + dd2(b >> 4, x);
			break;
		case 0xc2: case 0xca: case 0xd2: case 0xda: case 0xe2: case 0xea: case 0xf2: case 0xfa:
			opcode.opcode = string("jp ") + flags1(b >> 3) + ", NN";
			collect_nn();
			opcode.is_jump = true;
			break;
		case 0xc3:
			opcode.opcode = "jp NN";
			collect_nn();
			opcode.is_jump = true;
			opcode.ends_flow = true;
			break;
		case 0xc4: case 0xcc: case 0xd4: case 0xdc: case 0xe4: case 0xec: case 0xf4: case 0xfc:
			opcode.opcode = string("call ") + flags1(b >> 3) + ", NN";
			collect_nn();
			opcode.is_jump = true;
			break;
		case 0xc5: case 0xd5: case 0xe5: case 0xf5:
			opcode.opcode = string("push ") + dd2(b >> 4, x);
			break;
		case 0xc6: case 0xce: case 0xd6: case 0xde: case 0xe6: case 0xee: case 0xf6: case 0xfe:
			opcode.opcode = alu1(b >> 3) + "N";
			collect_n();
			break;
		case 0xc7: case 0xcf: case 0xd7: case 0xdf: case 0xe7: case 0xef: case 0xf7: case 0xff:
			opcode.opcode = "rst " + fmt_hex(b & 0x34, 2) + "h";
			break;
		case 0xc9:
			opcode.opcode = "ret";
			opcode.ends_flow = true;
			break;
		case 0xcb:
			if (x != 0)
				if ((b & 0x07) == 0x06)
					collect_dis();
			b = peek(addr++);
			if (b <= 0x3f) {
				opcode.opcode = rot1(b >> 3) + r1(b, 0);
			}
			else {
				opcode.opcode = bit1(b >> 6) + to_string((b >> 3) & 0x07) + ", " + r1(b, 0);
			}
			break;
		case 0xcd:
			opcode.opcode = "call NN";
			collect_nn();
			opcode.is_jump = true;
			break;
		case 0xd3:
			opcode.opcode = "out (N), a";
			collect_n();
			break;
		case 0xd9:
			opcode.opcode = "exx";
			break;
		case 0xdb:
			opcode.opcode = "in a, (N)";
			collect_n();
			break;
		case 0xdd:
			x = 1; done = false; break;
		case 0xe3:
			opcode.opcode = string("ex (sp), ") + x1(x);
			break;
		case 0xe9:
			opcode.opcode = string("jp (") + x1(x) + ")";
			opcode.ends_flow = true;
			break;
		case 0xeb:
			opcode.opcode = string("ex de, ") + x1(x);
			break;
		case 0xed:
			b = peek(addr++);
			switch (b) {
			case 0x40: case 0x48: case 0x50: case 0x58: case 0x60: case 0x68:            case 0x78:
				opcode.opcode = string("in ") + r1(b >> 3, 0) + ", (c)";
				break;
			case 0x41: case 0x49: case 0x51: case 0x59: case 0x61: case 0x69:            case 0x79:
				opcode.opcode = string("out (c), ") + r1(b >> 3, 0);
				break;
			case 0x42: case 0x52: case 0x62: case 0x72:
				opcode.opcode = string("sbc hl, ") + dd1(b >> 4, 0);
				break;
			case 0x43: case 0x53: case 0x63: case 0x73:
				opcode.opcode = string("ld (NN), ") + dd1(b >> 4, 0);
				collect_nn();
				break;
			case 0x44:
				opcode.opcode = "neg";
				break;
			case 0x45:
				opcode.opcode = "retn";
				opcode.ends_flow = true;
				break;
			case 0x46:
				opcode.opcode = "im 0";
				break;
			case 0x47:
				opcode.opcode = "ld i, a";
				break;
			case 0x4a: case 0x5a: case 0x6a: case 0x7a:
				opcode.opcode = string("adc hl, ") + dd1(b >> 4, 0);
				break;
			case 0x4b: case 0x5b: case 0x6b: case 0x7b:
				opcode.opcode = string("ld ") + dd1(b >> 4, 0) + ", (NN)";
				collect_nn();
				break;
			case 0x4d:
				opcode.opcode = "reti";
				opcode.ends_flow = true;
				break;
			case 0x4f:
				opcode.opcode = "ld r, a";
				break;
			case 0x56:
				opcode.opcode = "im 1";
				break;
			case 0x57:
				opcode.opcode = "ld a, i";
				break;
			case 0x5e:
				opcode.opcode = "im 2";
				break;
			case 0x5f:
				opcode.opcode = "ld a, r";
				break;
			case 0x67:
				opcode.opcode = "rrd";
				break;
			case 0x6f:
				opcode.opcode = "rld";
				break;
			case 0xa0:
				opcode.opcode = "ldi";
				break;
			case 0xa1:
				opcode.opcode = "cpi";
				break;
			case 0xa2:
				opcode.opcode = "ini";
				break;
			case 0xa3:
				opcode.opcode = "outi";
				break;
			case 0xa8:
				opcode.opcode = "ldd";
				break;
			case 0xa9:
				opcode.opcode = "cpd";
				break;
			case 0xaa:
				opcode.opcode = "ind";
				break;
			case 0xab:
				opcode.opcode = "outd";
				break;
			case 0xb0:
				opcode.opcode = "ldir";
				break;
			case 0xb1:
				opcode.opcode = "cpir";
				break;
			case 0xb2:
				opcode.opcode = "inir";
				break;
			case 0xb3:
				opcode.opcode = "otir";
				break;
			case 0xb8:
				opcode.opcode = "lddr";
				break;
			case 0xb9:
				opcode.opcode = "cpdr";
				break;
			case 0xba:
				opcode.opcode = "indr";
				break;
			case 0xbb:
				opcode.opcode = "otdr";
				break;
			case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
			case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
			case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
			case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
			case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
			case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
			case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
			case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
			case 0x4c: case 0x4e: case 0x54: case 0x55: case 0x5c: case 0x5d:
			case 0x64: case 0x65: case 0x66: case 0x6c: case 0x6d: case 0x6e:
			case 0x70: case 0x71: case 0x74: case 0x75: case 0x76: case 0x77:
			case 0x7c: case 0x7d: case 0x7e: case 0x7f:
			case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
			case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
			case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
			case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
			case 0xa4: case 0xa5: case 0xa6: case 0xa7:	case 0xac: case 0xad: case 0xae: case 0xaf:
			case 0xb4: case 0xb5: case 0xb6: case 0xb7:	case 0xbc: case 0xbd: case 0xbe: case 0xbf:
			case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7:
			case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf:
			case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6: case 0xd7:
			case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xde: case 0xdf:
			case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
			case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
			case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
			case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
				break;
			default:
				assert(0);
			}
			break;
		case 0xf3:
			opcode.opcode = "di";
			break;
		case 0xf9:
			opcode.opcode = string("ld sp, ") + x1(x);
			break;
		case 0xfb:
			opcode.opcode = "ei";
			break;
		case 0xfd:
			x = 2; done = false; break;
		default:
			assert(0);
		}
	}

	opcode.size = addr - opcode.addr;
	return opcode;
}
