//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include "consts.h"

std::string decode_zx81(char c);
Bytes encode_zx81(const std::string& str);
Bytes encode_zx81(const char*& p);
std::string encode_hex(const Bytes& bytes);
