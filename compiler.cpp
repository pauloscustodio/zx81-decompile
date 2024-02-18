//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "compiler.h"
#include "encode.h"
#include "errors.h"
#include "memory.h"
#include "zfloat.h"
#include <cassert>
#include <iostream>
#include <sstream>
using namespace std;

static Compiler g_compiler;

void Compiler::compile(Basic& basic_) {
	basic = &basic_;

	compute_line_numbers();
	basic->symtab.init();

	int last_end_addr = 0;
	for (pass = 1; pass <= 2; pass++) {
		compile_prog();

		if (addr != last_end_addr)	// source size changed
			pass = 0;				// keep on pass 1
		
		last_end_addr = addr;
	}

	compile_vars();

	basic = nullptr;
}

void Compiler::compute_line_numbers() {
	int line_num = basic->auto_increment;
	int last_line = -1;
	for (auto& line : basic->source_lines) {
		if (line.type == SourceLine::Type::Basic) {
			if (line.basic_line.line_num < 0) {
				line.basic_line.line_num = line_num;
				line_num += basic->auto_increment;
			}
			else {
				line_num = line.basic_line.line_num + basic->auto_increment;
			}

			if (line.basic_line.line_num <= last_line)
				error("line " + to_string(line.basic_line.line_num) + " follows line " + to_string(last_line));
			else if (line.basic_line.line_num > MaxLineNum)
				error("line " + to_string(line.basic_line.line_num) + " above maximum of " + to_string(MaxLineNum));
		}
	}
}

void Compiler::compile_prog() {
	addr = PROG;
	line_addr = 0;
	bool last_basic = false;
	for (auto& line : basic->source_lines) {
		switch (line.type) {
		case SourceLine::Type::Basic:
			if (line_addr > 0 && !last_basic) {	// end last asm statement
				poke(addr++, C_newline);
				end_basic_line();
			}
			start_basic_line(line.basic_line.line_num);
			compile_basic(line.basic_line);
			end_basic_line();
			last_basic = true;
			break;
		case SourceLine::Type::Asm:			// concatenate to last basic line
			if (line_addr == 0) {			// create REM line
				start_basic_line(0);
				poke(addr++, C_REM);
			}
			else if (last_basic) {
				addr--;						// overwrite newline
			}
			asmpc = addr;
			assemble(line.asm_line);
			end_basic_line();
			last_basic = false;
			break;
		default:
			assert(0);
		}
	}
	if (line_addr > 0 && !last_basic) {
		poke(addr++, C_newline);
		end_basic_line();
	}

	init_video_to_stkend(addr, basic->d_file_bytes, basic->e_line_bytes);

	// autostart
	if (basic->autostart) {
		int line_addr = get_line_addr(basic->autostart);
		dpoke(NXTLIN, line_addr);
	}
	else
		dpoke(NXTLIN, dpeek(D_FILE));

	// fast
	if (basic->fast)
		poke(CDFLAG, peek(CDFLAG) & ~FlagSlow);
	else
		poke(CDFLAG, peek(CDFLAG) | FlagSlow);
}

void Compiler::start_basic_line(int line_num) {
	line_addr = addr;
	dpoke_be(addr, line_num); addr += 2;
	dpoke(addr, 0); addr += 2;		// placeholder for size
}

void Compiler::end_basic_line() {
	dpoke(line_addr + 2, addr - line_addr - 4);		// fill size
}

void Compiler::compile_basic(BasicLine& basic_line) {
	// define label on possibly multiple passes 1
	if (pass == 1 && !basic_line.label.empty())
		basic->symtab.update(Symbol::Type::Addr, basic_line.label, line_addr);

	// process each token
	for (auto& token : basic_line.tokens) {
		switch (token.code) {
		case T_none:
			break;
		case T_integer:
			compile_number(token.ivalue);
			break;
		case T_float:
			compile_number(token.fvalue);
			break;
		case T_string:
			compile_string(token.svalue);
			break;
		case T_ident:
			compile_ident(token.ident);
			break;
		case T_rem_code:
			for (auto& c : token.bytes)
				poke(addr++, c);
			break;
		case T_line_addr_ref:
			compile_line_addr_ref(token.ident);
			break;
		case T_line_num_ref:
			compile_line_num_ref(token.ident);
			break;
		case T_const_expr:
			compile_expr(token.rpn);
			break;
		default:
			assert(token.code < 0x100);
			poke(addr++, token.code);
		}
	}
}

void Compiler::compile_number(double value) {
	ostringstream oss;

	// encode digits
	oss << value;
	string value_str = oss.str();
	Bytes str_bytes = encode_zx81(value_str);
	for (auto& c : str_bytes)
		poke(addr++, c);

	// encode number marker
	poke(addr++, C_number);

	// encode fp number
	array<Byte, 5> fp_bytes = float_to_zx81(value);
	for (auto& c : fp_bytes)
		poke(addr++, c);
}

void Compiler::compile_string(const string& str) {
	Bytes str_bytes = encode_zx81(str);
	poke(addr++, C_dquote);
	for (auto& c : str_bytes)
		poke(addr++, c);
	poke(addr++, C_dquote);
}

void Compiler::compile_ident(const string& ident) {
	Bytes str_bytes = encode_zx81(ident);
	for (auto& c : str_bytes)
		poke(addr++, c);
}

void Compiler::compile_line_addr_ref(const string& ident) {
	auto symbol = basic->symtab.get(ident);
	if (!symbol && pass == 2)
		error("undefined symbol", ident);
	if (symbol)
		compile_number(symbol->value);
	else
		compile_number(0);
}

void Compiler::compile_line_num_ref(const string& ident) {
	auto symbol = basic->symtab.get(ident);
	if (!symbol && pass == 2)
		error("undefined symbol", ident);
	if (symbol)
		compile_number(dpeek_be(symbol->value));
	else
		compile_number(0);
}

void Compiler::compile_expr(const Expr& rpn) {
	int value = 0;
	if (pass == 1)
		basic->eval_expr(rpn, asmpc, value, false);
	else 
		basic->eval_expr(rpn, asmpc, value, true);		// error if symbol not found
	compile_number(value);
}

void Compiler::compile_vars() {
	addr = dpeek(VARS);

	Bytes str_bytes;
	array<Byte, 5> fp_bytes{ 0 };
	int last_dimension = 0;
	int size = 0;

	for (auto& var : basic->basic_vars) {
		Bytes name_bytes = encode_zx81(var.name);
		assert(name_bytes.size() > 0);

		switch (var.type) {
		case BasicVar::Type::Number:
			// put name
			if (var.name.size() == 1) {
				poke(addr++, (name_bytes[0] & 0x3f) | 0x60);			// 011-letter
			}
			else {
				poke(addr++, (name_bytes[0] & 0x3f) | 0xa0);			// 101-letter
				for (size_t i = 1; i < name_bytes.size() - 1; i++)
					poke(addr++, name_bytes[i] & 0x3f);					// 001-letter
				poke(addr++, (name_bytes.back() & 0x3f) | 0x80);		// 101-letter
			}

			// put value
			fp_bytes = float_to_zx81(var.value);
			addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
			break;

		case BasicVar::Type::ArrayNumbers:
			// put name
			poke(addr++, (name_bytes[0] & 0x1f) | 0x80);				// 100-letter

			// put dimensions
			size = 1
				+ 2 * INT(var.dimensions.size())
				+ 5 * INT(var.values.size());
			dpoke(addr, size); addr += 2;
			poke(addr++, var.dimensions.size() & 0xff);

			for (auto& dimension : var.dimensions) {
				dpoke(addr, dimension); addr += 2;
			}

			// put values
			for (auto& value : var.values) {
				fp_bytes = float_to_zx81(value);
				addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
			}
			break;

		case BasicVar::Type::ForNextLoop:
			// put name
			poke(addr++, (name_bytes[0] & 0x3f) | 0xe0);				// 111-letter

			// put values
			fp_bytes = float_to_zx81(var.value);
			addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

			fp_bytes = float_to_zx81(var.limit);
			addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

			fp_bytes = float_to_zx81(var.step);
			addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

			dpoke(addr, var.line_num); addr += 2;
			break;

		case BasicVar::Type::String:
			// put name
			poke(addr++, (name_bytes[0] & 0x1f) | 0x40);				// 010-letter

			dpoke(addr, INT(var.str.size())); addr += 2;

			str_bytes = encode_zx81(var.str);
			addr = poke_bytes(addr, str_bytes);
			break;

		case BasicVar::Type::ArrayStrings:
			// put name
			poke(addr++, (name_bytes[0] & 0x1f) | 0xc0);				// 110-letter

			// put dimensions
			last_dimension = var.dimensions.back();
			size = 1
				+ 2 * INT(var.dimensions.size())
				+ last_dimension * INT(var.strs.size());
			dpoke(addr, size); addr += 2;
			poke(addr++, INT(var.dimensions.size()));

			for (auto& dimension : var.dimensions) {
				dpoke(addr, dimension); addr += 2;
			}

			// put strings
			for (auto& str : var.strs) {
				str_bytes = encode_zx81(str);
				addr = poke_bytes(addr, str_bytes);
			}
			break;

		default:
			assert(0);
		}
	}

	poke(addr++, 0x80);
	init_e_line_to_stkend(addr, basic->e_line_bytes);
}

void Compiler::assemble(AsmLine& asm_line) {
	// EQU
	if (!asm_line.equ_rpn.empty()) {
		if (pass == 1) {
			int value = basic->eval_const_expr(asm_line.equ_rpn);
			basic->symtab.update(Symbol::Type::Const, asm_line.label, value);
		}
	}
	else {
		if (!asm_line.label.empty()) 
			basic->symtab.update(Symbol::Type::Addr, asm_line.label, asmpc);

		addr = poke_bytes(addr, asm_line.bytes);
		if (pass == 2) {
			err_set_filename(asm_line.source_filename);
			err_set_line_num(asm_line.source_line_num);

			for (auto& patch : asm_line.patches) {
				int value = 0;
				basic->eval_expr(patch.rpn, asmpc, value, true);	// show errors
				switch (patch.type) {
				case Patch::Type::Byte:
					poke(asmpc + patch.offset, value);
					break;
				case Patch::Type::Word:
					dpoke(asmpc + patch.offset, value);
					break;
				case Patch::Type::SByte:
					if (value < -128 || value >127)
						error("integer out of range", std::to_string(value));
					poke(asmpc + patch.offset, value);
					break;
				case Patch::Type::JrOffset:
					value -= asmpc + 2;
					if (value < -128 || value >127)
						error("integer out of range", std::to_string(value));
					poke(asmpc + patch.offset, value);
					break;
				default:
					assert(0);
				}
			}

			err_clear();
		}
	}
}

void compile(Basic& basic) {
	g_compiler.compile(basic);
}
