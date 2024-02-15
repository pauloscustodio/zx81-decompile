//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <string>
using namespace std;

class Errors {
public:
	void clear();
	void set_filename(const string& filename);
	void set_line_num(int line_num);

	const string& get_filename() const { return m_filename; }
	int get_line_num() const { return m_line_num; }
	int get_count() const { return m_count; }

	void error(const string& message, const string& arg = "");
	void fatal_error(const string& message, const string& arg = "");
	void exit_status() const;

private:
	int m_count;
	int m_line_num;
	string m_filename;

};

extern Errors g_errors;
