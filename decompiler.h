//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include <string>
using namespace std;

class Decompiler {
public:
	void decompile(Basic& basic);

private:
	Basic* basic{ nullptr };
	int addr{ 0 };
	int end{ 0 };

	void decompile_sysvars();
	void decompile_d_file();
	void decompile_e_line();
	void decompile_basic();
	void decompile_basic_line(BasicLine& basic_line);
	bool decompile_rem_code(BasicLine& basic_line);
	bool decompile_number(BasicLine& basic_line);
	bool decompile_ident(BasicLine& basic_line);
	bool decompile_string(BasicLine& basic_line);
	void decompile_newline(BasicLine& basic_line);
	void decompile_vars();
	void disassemble();
};

void decompile(Basic& basic);
