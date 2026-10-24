//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>

std::string fmt_hex(int value, int digits = 2,
                    const std::string& prefix = "0x");
std::string fmt_line_number(int value, int digits = 5);
std::string str_chomp(std::string str);
std::string str_replace_all(std::string str, const std::string& from,
                            const std::string& to);

std::string basename(const std::string& filename);
std::string replace_extension(const std::string& filename,
                              const std::string& extension);

// cctype functions need unsigned char input
// to avoid undefined behavior on negative values
bool isdigit_safe(char c);
bool isxdigit_safe(char c);
bool isspace_safe(char c);
bool isalpha_safe(char c);
bool isalnum_safe(char c);
bool isprint_safe(char c);
bool isgraph_safe(char c);
bool ispunct_safe(char c);
bool iscntrl_safe(char c);
bool islower_safe(char c);
bool isupper_safe(char c);
char tolower_safe(char c);
char toupper_safe(char c);
