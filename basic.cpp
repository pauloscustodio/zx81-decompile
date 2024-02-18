//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "disasm.h"
#include "encode.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "parser.h"
#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

//-----------------------------------------------------------------------------
// Symtab
//-----------------------------------------------------------------------------

Symbol::Symbol(Type type_, const string& name_, int value_)
	: type(type_), name(name_), value(value_) {
}

Symtab::Symtab() {
	init();
}

Symtab::~Symtab() {
	for (auto& it : symbols)
		delete it.second;
	symbols.clear();
}

void Symtab::init() {
	for (auto& it : symbols) 
		delete it.second;
	symbols.clear();

#define X(name, value)		add(Symbol::Type::Const, #name, value);
#include "consts.def"
}

void Symtab::add(Symbol::Type type, const string& name, int value) {
	auto it = symbols.find(name);
	if (it != symbols.end()) 
		error("duplicate definition", name);
	else {
		auto symbol = new Symbol(type, name, value);
		symbols[name] = symbol;
	}
}

void Symtab::update(Symbol::Type type, const string& name, int value) {
	auto symbol = get(name);
	if (symbol) {
		symbol->type = type;
		symbol->value = value;
	}
	else
		add(type, name, value);
}

Symbol* Symtab::get(const string& name) {
	auto it = symbols.find(name);
	if (it != symbols.end())
		return it->second;
	else
		return nullptr;
}

Symbol* Symtab::find(int value) {
	for (auto& it : symbols) {
		if (it.second->value == value)
			return it.second;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// Basic
//-----------------------------------------------------------------------------

Basic::Basic() {
	clear();
}

void Basic::clear() {
	source_lines.clear();
	basic_vars.clear();
	symtab.init();
	d_file_bytes = get_empty_d_file();
	e_line_bytes = get_empty_e_line();
	autostart = 0;
	auto_increment = 10;
	fast = false;
}

int Basic::eval_const_expr(const Expr& rpn) {
	vector<int> stack;
	Symbol* symbol{ nullptr };
	int value = 0;

	for (auto& token : rpn) {
		switch (token.code) {
		case T_integer:
			stack.push_back(token.ivalue);
			break;
		case T_line_num_ref: 
			symbol = symtab.get(token.ident);
			if (symbol && symbol->type == Symbol::Type::Const)
				stack.push_back(dpeek_be(symbol->value));
			else {
				error("const expression refers to non-const symbol", token.ident);
				return 0;
			}
			break;
		case T_line_addr_ref: 
			symbol = symtab.get(token.ident);
			if (symbol && symbol->type == Symbol::Type::Const)
				stack.push_back(symbol->value);
			else {
				error("const expression refers to non-const symbol", token.ident);
				return 0;
			}
			break;
		case T_unary_minus: 
			stack.back() = -stack.back(); 
			break;
		case C_plus: 
			value = stack.back(); 
			stack.pop_back(); 
			stack.back() += value; 
			break;
		case C_minus: 
			value = stack.back(); 
			stack.pop_back(); 
			stack.back() -= value; 
			break;
		case C_mult: 
			value = stack.back(); 
			stack.pop_back(); 
			stack.back() *= value; 
			break;
		case C_div: 
			value = stack.back(); 
			stack.pop_back(); 
			stack.back() /= value; 
			break;
		default: 
			assert(0);
		}
	}

	assert(stack.size() == 1);
	return stack.back();
}

bool Basic::eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error) {
	vector<int> stack;
	Symbol* symbol{ nullptr };
	bool result = true;

	for (auto& token : rpn) {
		switch (token.code) {
		case T_integer:
			stack.push_back(token.ivalue);
			break;
		case T_line_num_ref:
			symbol = symtab.get(token.ident);
			if (symbol)
				stack.push_back(dpeek_be(symbol->value));
			else {
				result = false;
				if (do_error)
					error("undefined symbol", token.ident);
				stack.push_back(0);
			}
			break;
		case T_line_addr_ref:
			symbol = symtab.get(token.ident);
			if (symbol)
				stack.push_back(symbol->value);
			else {
				result = false;
				if (do_error)
					error("undefined symbol", token.ident);
				stack.push_back(0);
			}
			break;
		case T_ASMPC:
			stack.push_back(asmpc);
			break;
		case T_unary_minus:
			stack.back() = -stack.back();
			break;
		case C_plus:
			value = stack.back();
			stack.pop_back();
			stack.back() += value;
			break;
		case C_minus:
			value = stack.back();
			stack.pop_back();
			stack.back() -= value;
			break;
		case C_mult:
			value = stack.back();
			stack.pop_back();
			stack.back() *= value;
			break;
		case C_div:
			value = stack.back();
			stack.pop_back();
			stack.back() /= value;
			break;
		default:
			assert(0);
		}
	}

	assert(stack.size() == 1);
	value = stack.back();
	return result;
}

void Basic::write_b81_file(const string& filename) {
	ofstream ofs(filename);
	if (!ofs.is_open()) {
		perror(filename.c_str());
		fatal_error("write file", filename);
	}

	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
		write_sysvars(ofs);
	write_basic_lines(ofs);
	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
		write_video(ofs);
	write_basic_vars(ofs);
	write_basic_system(ofs);
	write_basic_memory_map(ofs);
}

void Basic::parse_b81_file(const string& filename) {
	::parse_b81_file(*this, filename);
}

void Basic::write_sysvars(ofstream& ofs) const {
	ofs << "# [VERSN     =  " << peek(VERSN) << "]" << endl;
	ofs << "# [E_PPC     =  " << dpeek(E_PPC) << "]" << endl;
	ofs << "# [D_FILE    = $" << fmt_hex(dpeek(D_FILE), 4) << "]" << endl;
	ofs << "# [DF_CC     = $" << fmt_hex(dpeek(DF_CC), 4) << "]" << endl;
	ofs << "# [VARS      = $" << fmt_hex(dpeek(VARS), 4) << "]" << endl;
	ofs << "# [DEST      = $" << fmt_hex(dpeek(DEST), 4) << "]" << endl;
	ofs << "# [E_LINE    = $" << fmt_hex(dpeek(E_LINE), 4) << "]" << endl;
	ofs << "# [CH_ADD    = $" << fmt_hex(dpeek(CH_ADD), 4) << "]" << endl;
	ofs << "# [X_PTR     = $" << fmt_hex(dpeek(X_PTR), 4) << "]" << endl;
	ofs << "# [STKBOT    = $" << fmt_hex(dpeek(STKBOT), 4) << "]" << endl;
	ofs << "# [STKEND    = $" << fmt_hex(dpeek(STKEND), 4) << "]" << endl;
	ofs << "# [BREG      =  " << peek(BREG) << "]" << endl;
	ofs << "# [MEM       = $" << fmt_hex(dpeek(MEM), 4) << "]" << endl;
	ofs << "# [FREE1     =  " << peek(FREE1) << "]" << endl;
	ofs << "# [DF_SZ     =  " << peek(DF_SZ) << "]" << endl;
	ofs << "# [S_TOP     =  " << dpeek(S_TOP) << "]" << endl;
	ofs << "# [LAST_K    = $" << fmt_hex(dpeek(LAST_K), 4) << "]" << endl;
	ofs << "# [DEBOUNCE  =$" << fmt_hex(peek(DEBOUNCE), 2) << "]" << endl;
	ofs << "# [MARGIN    =  " << peek(MARGIN) << "]" << endl;
	ofs << "# [NXTLIN    = $" << fmt_hex(dpeek(NXTLIN), 4) << "]" << endl;
	ofs << "# [OLDPPC    =  " << dpeek(OLDPPC) << "]" << endl;
	ofs << "# [FLAGX     = $" << fmt_hex(peek(FLAGX), 2) << "]" << endl;
	ofs << "# [STRLEN    =  " << dpeek(STRLEN) << "]" << endl;
	ofs << "# [T_ADDR    = $" << fmt_hex(dpeek(T_ADDR), 4) << "]" << endl;
	ofs << "# [SEED      = $" << fmt_hex(dpeek(SEED), 4) << "]" << endl;
	ofs << "# [FRAMES    = $" << fmt_hex(dpeek(FRAMES), 4) << "]" << endl;
	ofs << "# [COORDS_X  =  " << peek(COORDS_X) << "]" << endl;
	ofs << "# [COORDS_Y  =  " << peek(COORDS_Y) << "]" << endl;
	ofs << "# [PR_CC     = $" << fmt_hex(peek(PR_CC), 2) << "]" << endl;
	ofs << "# [S_POSN_COL=  " << peek(S_POSN_COL) << "]" << endl;
	ofs << "# [S_POSN_ROW=  " << peek(S_POSN_ROW) << "]" << endl;
	ofs << "# [CDFLAG    = $" << fmt_hex(peek(CDFLAG), 2) << "]" << endl;

	ofs << "# [PRBUFF=";
	for (int i = 0; i < 33; i++)
		ofs << "\\" << fmt_hex(peek(PRBUFF + i), 2);
	ofs << "]" << endl;

	ofs << "# [MEMBOT=";
	for (int i = 0; i < 30; i++)
		ofs << "\\" << fmt_hex(peek(MEMBOT + i), 2);
	ofs << "]" << endl;

	ofs << "# [FREE2     = $" << fmt_hex(dpeek(FREE2), 4) << "]" << endl;
	ofs << endl;
}

void Basic::write_basic_lines(ofstream& ofs) const {
	for (auto& line : source_lines) {
		if ((optflags & FLAG_DEBUG) == FLAG_DEBUG)
			ofs << "# [$" << fmt_hex(line.addr, 4) << "]" << endl;
		if (line.type == SourceLine::Type::Basic) {
			if (!line.basic_line.label.empty())
				ofs << "@" << line.basic_line.label << ":" << endl;

			ofs << fmt_line_number(line.basic_line.line_num) << " ";

			for (auto& token : line.basic_line.tokens) {
				switch (token.code) {
				case T_none:
					break;
				case T_float:
					ofs << token.svalue;
					break;
				case T_string:
					ofs << "\"" << token.svalue << "\"";
					break;
				case T_ident:
					ofs << token.ident;
					break;
				case T_rem_code:
					write_mem_info(ofs, line.addr + 5, INT(token.bytes.size()));
					break;
				case T_line_addr_ref:
					ofs << "&" << token.ident;
					break;
				case T_line_num_ref:
					ofs << "@" << token.ident;
					break;
				case C_space:
					ofs << "_";
					break;
				case C_newline:
					ofs << endl;
					break;
				default:
					assert(token.code < 0x100);
					ofs << decode_zx81(token.code);
				}
			}
		}
	}

	if (!source_lines.empty())
		ofs << endl;
}

void Basic::write_video(ofstream& ofs) const {
	int addr = dpeek(D_FILE);
	size_t p = 0;
	while (p < d_file_bytes.size()) {
		ofs << "# [$" << fmt_hex(addr, 4) << "] = \"";
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

void Basic::write_basic_vars(ofstream& ofs) const {
	int num_elements = 0;
	int last_dimension = 0;

	int addr = 0;
	for (auto& var : basic_vars) {
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
		ofs << "# [$" << fmt_hex(addr, 4) << "] = $" << fmt_hex(peek(addr), 2) << endl;

	if ((optflags & FLAG_DEBUG) == FLAG_DEBUG || !basic_vars.empty())
		ofs << endl;
}

void Basic::write_basic_system(ofstream& ofs) const {
	int d_file = dpeek(D_FILE);
	int vars = dpeek(VARS);
	int e_line = dpeek(E_LINE);
	int stkbot = dpeek(STKBOT);

	ofs << "#SYSVARS=" << encode_hex(peek_bytes(VERSN, PROG - VERSN)) << endl;
	ofs << "#D_FILE=" << encode_hex(peek_bytes(d_file, vars - d_file)) << endl;
	ofs << "#WORKSPACE=" << encode_hex(peek_bytes(e_line, stkbot - e_line)) << endl;
	ofs << endl;

	// autostart
	ofs << "#AUTOSTART=" << autostart << endl;

	// fast mode
	ofs << "#FAST=" << fast << endl;
}

void Basic::write_basic_memory_map(ofstream& ofs) const {
	ofs << endl;
	int d_file = dpeek(D_FILE);
	int vars = dpeek(VARS);
	int e_line = dpeek(E_LINE);
	for (int addr = RAM_ADDR; addr < e_line; addr += 64) {
		ofs << "# [$" << fmt_hex(addr, 4) << "] ";
		for (int p = addr; p < addr + 64; p++) {
			if (p < PROG)
				ofs << "!";
			else if (p < d_file) {
				switch (g_disasm_code.get_type(p)) {
				case Opcode::Type::Undef: ofs << "#"; break;
				case Opcode::Type::Unknown: ofs << "-"; break;
				case Opcode::Type::Asm: ofs << "C"; break;
				case Opcode::Type::AsmData: ofs << "C"; break;
				case Opcode::Type::Defb: ofs << "B"; break;
				case Opcode::Type::DefbData: ofs << "B"; break;
				case Opcode::Type::Defw: ofs << "W"; break;
				case Opcode::Type::DefwData: ofs << "W"; break;
				case Opcode::Type::Defm: ofs << "T"; break;
				case Opcode::Type::DefmData: ofs << "T"; break;
				default:
					assert(0);
				}
			}
			else if (p < vars)
				ofs << "*";
			else
				ofs << "$";
		}
		ofs << endl;
	}
}

void Basic::write_mem_info(ofstream& ofs, int start_addr, int len) const {
	ofs << endl;			// newline after REM
	ofs << "#ASM" << endl;

	for (int addr = start_addr; addr < start_addr + len; ) {
		Opcode* opc = g_disasm_code.get(addr);
		ofs << opc->to_string();
		addr += opc->size;
	}

	ofs << "#ENDASM";	// followed by newline from REM
}


//-----------------------------------------------------------------------------
// Lines
//-----------------------------------------------------------------------------

AsmLine::AsmLine()
	: source_filename(err_get_filename()), source_line_num(err_get_line_num()) {
}

BasicLine::BasicLine()
	: source_filename(err_get_filename()), source_line_num(err_get_line_num()) {
}

