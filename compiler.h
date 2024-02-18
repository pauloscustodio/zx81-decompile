//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include <string>
using namespace std;

class Compiler {
public:
	void compile(Basic& basic);

private:
	Basic* basic{ nullptr };
	int line_addr{ 0 };
	int asmpc{ 0 };
	int addr{ 0 };
	int end{ 0 };
	int pass{ 0 };

	void compute_line_numbers();
	void compile_prog();
	void start_basic_line(int line_num);
	void end_basic_line();
	void compile_basic(BasicLine& basic_line);
	void compile_number(double value);
	void compile_string(const string& str);
	void compile_ident(const string& ident);
	void compile_line_addr_ref(const string& ident);
	void compile_line_num_ref(const string& ident);
	void compile_expr(const Expr& rpn);
	void compile_vars();
	void assemble(AsmLine& asm_line);
};

void compile(Basic& basic);
