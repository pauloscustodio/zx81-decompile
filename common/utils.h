//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>

bool str_ends_with(const std::string& str, const std::string& ending);
std::string str_replace_all(std::string str, const std::string& from,
                            const std::string& to);
std::string basename(const std::string& filename);
std::string replace_extension(const std::string& filename,
                              const std::string& extension);
std::string str_tolower(std::string str);
std::string str_toupper(std::string str);
std::string str_trim(const std::string& str);
std::string fmt_hex(int value, int digits = 2,
                    const std::string& prefix = "0x");

// digits = 7 + space to align with TAB position in BASIC source lines
std::string fmt_line_number(int value, int digits = 7);

// precision = 10 to match ZX81 float precision
std::string fmt_double(double value, int precision = 10);

std::string str_chomp(std::string str);
