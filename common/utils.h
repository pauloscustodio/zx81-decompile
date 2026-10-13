//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
using namespace std;

string fmt_hex(int value, int digits = 2);
string fmt_line_number(int value, int digits = 5);
string str_chomp(string str);
string str_replace_all(string str, const string& from, const string& to);

std::string basename(const std::string& filename);
std::string replace_extension(const std::string& filename,
                              const std::string& extension);
