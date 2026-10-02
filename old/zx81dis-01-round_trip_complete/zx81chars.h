//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include <cstdint>
#include <string>
#include <vector>

using uint = unsigned int;

// character constants
#define X(str, code, name) static inline constexpr uint CH##name = code;
#include "zx81chars.def"
#undef X

std::string decode_zx81_char(uint8_t code);
bool encode_zx81_char(const char*& p, bool check_keywords,
                      uint8_t& out_code,
                      const SourceLoc& loc);
bool encode_zx81_string(const char*& p, char delimiter,
                        std::vector<uint8_t>& bytes,
                        const SourceLoc& loc);
std::string zx81_char_name(uint8_t code);
