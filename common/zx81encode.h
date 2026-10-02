//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include "errors.h"
#include <string>

std::string decode_zx81(char c);
std::string decode_zx81_str_char(char c);
Bytes encode_zx81(const std::string& str, const SourceLoc& loc);
Bytes encode_zx81(const char*& p, const SourceLoc& loc);
std::string encode_hex(const Bytes& bytes);
