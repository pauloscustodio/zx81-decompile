//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "utils.h"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

string fmt_hex(int value, int digits) {
	ostringstream oss;
	oss << setw(digits) << setfill('0') << hex << value;
	return oss.str();
}

string fmt_line_number(int value, int digits) {
	ostringstream oss;
	oss << setw(digits) << setfill(' ') << value;
	return oss.str();
}

string str_chomp(string str) {
	while (!str.empty() && isspace(str.back()))
		str.pop_back();
	return str;
}

string str_replace_all(string str, const string& from, const string& to) {
	string::size_type n = 0;
	while ((n = str.find(from, n)) != string::npos) {
		str.replace(n, from.size(), to);
		n += to.size();
	}
	return str;
}
