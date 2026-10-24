//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include <string>

enum ZX81char {
    T_none = 0x100,
    T_integer,
    T_float,
    T_string,
    T_ident,
    T_rem_code,
    T_line_addr_ref,
    T_line_num_ref,
    T_const_expr,
    T_unary_minus,
    T_ASMPC,

#define X(code, name, str) name = code,
#include "zx81chars.def"
#undef X
};

std::string decode_zx81(char c);
Bytes encode_zx81(const std::string& str, const SourceLoc& loc);
Bytes encode_zx81(const char*& p, const SourceLoc& loc);
std::string encode_hex(const Bytes& bytes);
