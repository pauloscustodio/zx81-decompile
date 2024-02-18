//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

class Scan {
public:
	void set_text(const string& text);
	const char*& pos() { return p; }

	void skip_spaces();
	bool at_end(char comment_char = '\0');
	bool parse_integer(int& value);
	bool parse_number(double& value, string& value_text);
	bool parse_string(string& str);
	bool parse_ident(string& ident);
	bool match(const string& compare);
	int match_one_of(int not_found_result, vector<pair<string, int>> compare_list);

private:
	string text;
	const char* p{""};
};
