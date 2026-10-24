//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>

std::string fmt_hex(int value, int digits = 2);
std::string fmt_line_number(int value, int digits = 5);
std::string str_chomp(std::string str);
std::string str_replace_all(std::string str, const std::string& from,
                            const std::string& to);

std::string basename(const std::string& filename);
std::string replace_extension(const std::string& filename,
                              const std::string& extension);
