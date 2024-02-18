//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "decompiler.h"
#include "disasm.h"
#include "encode.h"
#include "errors.h"
#include "memory.h"
#include "utils.h"
#include <cassert>
using namespace std;

static Decompiler g_decompiler;

void Decompiler::decompile(Basic& basic_) {
	basic = &basic_;

	decompile_sysvars();
	decompile_d_file();
	decompile_e_line();
	decompile_basic();
	decompile_vars();
	disassemble();

	basic = nullptr;
}

void Decompiler::decompile_sysvars() {
	// autostart
	int d_file = dpeek(D_FILE);
	int nxtlin = dpeek(NXTLIN);
	if (nxtlin >= d_file)
		basic->autostart = 0;
	else
		basic->autostart = dpeek_be(nxtlin);

	// fast
	if ((peek(CDFLAG) & FlagSlow) == FlagSlow)
		basic->fast = false;
	else
		basic->fast = true;
}

void Decompiler::decompile_d_file() {
	int d_file = dpeek(D_FILE);
	int vars = dpeek(VARS);
	basic->d_file_bytes = peek_bytes(d_file, vars - d_file);
}

void Decompiler::decompile_e_line() {
	int e_line = dpeek(E_LINE);
	int stkbot = dpeek(STKBOT);
	basic->e_line_bytes = peek_bytes(e_line, stkbot - e_line);
}

void Decompiler::decompile_basic() {
	basic->source_lines.clear();
	addr = PROG;
	while (addr < dpeek(D_FILE)) {
		SourceLine line(SourceLine::Type::Basic);
		line.addr = addr;
		line.basic_line.line_num = dpeek_be(addr);
		int size = dpeek(addr + 2);

		addr += 4;
		end = addr + size;

		decompile_basic_line(line.basic_line);
		basic->source_lines.push_back(line);
	}
}

void Decompiler::decompile_basic_line(BasicLine& basic_line) {
	if (decompile_rem_code(basic_line)) {
	}
	else {
		while (addr < end - 1) {
			if (decompile_number(basic_line)) {
			}
			else if (decompile_ident(basic_line)) {
			}
			else if (decompile_string(basic_line)) {
			}
			else {
				Token token;
				token.code = static_cast<ZX81char>(peek(addr++));
				basic_line.tokens.push_back(token);
			}
		}
		decompile_newline(basic_line);
	}
}

bool Decompiler::decompile_rem_code(BasicLine& basic_line) {
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
			basic_line.tokens.push_back(rem);

			Token bytes;
			bytes.code = T_rem_code;
			for (int p = addr + 1; p < end - 1; p++) {
				bytes.bytes.push_back(peek(p));
			}
			g_disasm_code.set_unknown(addr, INT(bytes.bytes.size()));
			basic_line.tokens.push_back(bytes);

			addr = end - 1;
			decompile_newline(basic_line);
			return true;
		}
	}
	return false;
}

bool Decompiler::decompile_number(BasicLine& basic_line) {
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
	double value2 = fpeek(p); p += 5;

	if (abs(value1 - value2) > 1e-6)
		error("number " + to_string(value1) + " != " + to_string(value2));

	token.code = T_float;
	token.svalue = num;
	token.fvalue = value1;
	basic_line.tokens.push_back(token);
	addr = p;
	return true;
}

bool Decompiler::decompile_ident(BasicLine& basic_line) {
	Token token;
	int p = addr;
	while (true) {
		int c = peek(p);
		if (c < C_A || c > C_Z)
			break;
		token.ident.push_back(c - C_A + 'A');
		p++;
	}
	if (p == addr)					// no letters found
		return false;
	else {
		token.code = T_ident;
		basic_line.tokens.push_back(token);
		addr = p;
		return true;
	}
}

bool Decompiler::decompile_string(BasicLine& basic_line) {
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
		token.svalue += decode_zx81(c);
		p++;
	}
	p++;		// skip end dquote

	token.code = T_string;
	basic_line.tokens.push_back(token);
	addr = p;
	return true;
}

void Decompiler::decompile_newline(BasicLine& basic_line) {
	Token token;
	token.code = static_cast<ZX81char>(peek(addr++));
	if (token.code != C_newline)
		error("missing newline");

	basic_line.tokens.push_back(token);
}

void Decompiler::decompile_vars() {
	addr = dpeek(VARS);
	int c = 0;
	while ((c = peek(addr)) != 0x80) {
		BasicVar var;
		var.addr = addr;
		int addr0 = addr;

		if ((c & 0xe0) == 0x60) {		// single letter variable
			addr++;
			c &= 0x3f;
			c |= 0x20;

			var.type = BasicVar::Type::Number;
			var.name = decode_zx81(c);
			var.value = fpeek(addr); addr += 5;
		}
		else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
			var.type = BasicVar::Type::Number;

			// first letter
			addr++;
			c &= 0x3f;
			c |= 0x20;
			var.name = decode_zx81(c);

			// second, ... letter
			while (((c = peek(addr)) & 0xc0) == 0x00) {
				addr++;
				c &= 0x3f;
				c |= 0x20;
				var.name += decode_zx81(c);
			}

			// last letter
			c = peek(addr);
			if ((c & 0xc0) != 0x80) {
				error("invalid multi-letter variable", fmt_hex(c, 2));
				return;
			}
			else {
				addr++;
				c &= 0x3f;
				c |= 0x20;
				var.name += decode_zx81(c);
			}
			var.value = fpeek(addr); addr += 5;
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
				double value = fpeek(addr); addr += 5;
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

			var.value = fpeek(addr); addr += 5;
			var.limit = fpeek(addr); addr += 5;
			var.step = fpeek(addr); addr += 5;
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
			error("invalid variable marker", string("$") + fmt_hex(c, 2));
			break;
		}

		var.size = addr - addr0;

		basic->basic_vars.push_back(var);
	}
}

void Decompiler::disassemble() {
	// search all USR n in Basic
	for (auto& line : basic->source_lines) {
		if (line.type == SourceLine::Type::Basic) {
			vector<Token>& tokens = line.basic_line.tokens;
			if (tokens.size() > 1) {
				for (size_t i = 0; i < tokens.size() - 1; i++) {
					if (tokens[i].code == C_USR && tokens[i + 1].code == T_float) {
						int addr = INT(tokens[i + 1].fvalue);
						if ((addr >= 0 && addr < 0x2000)
							|| (addr >= PROG + 5 && addr < dpeek(E_LINE))) {
							string label = g_disasm_code.get_label(addr);
							tokens[i + 1].code = T_line_addr_ref;
							tokens[i + 1].ident = label;
							g_disasm_code.set_code(addr);
						}
					}
				}
			}
		}
	}
}

void decompile(Basic& basic) {
	g_decompiler.decompile(basic);
}
