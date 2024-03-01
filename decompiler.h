//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "parser.h"
#include <string>
using namespace std;

struct MemElement {
	enum class Type {Defb, Defw, Defm, Code, Comment};
	Type type{ Type::Defb };
	int addr{ 0 };
	int size{ 1 };
	string label;
	string comment;
	string header;
};

class Decompiler : public Scanner {
public:
	void decompile(Basic& basic);
	void decompile(Basic& basic, const string& ctl_filename);

private:
	Basic* basic{ nullptr };
	int addr{ 0 };
	int end{ 0 };
	vector<MemElement> mem_elements;

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
	void parse_ctl_file(const string& ctl_filename);
	void parse_ctl_line();
};

void decompile(Basic& basic);
void decompile(Basic& basic, const string& ctl_filename);
