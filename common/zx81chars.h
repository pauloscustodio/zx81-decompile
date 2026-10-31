//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
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
    T_symbol,
    T_unary_minus,
    T_log_not,
    T_log_and,
    T_log_or,
    T_bin_not,
    T_bin_and,
    T_bin_xor,
    T_bin_or,
    T_mod,
    T_conditional,
    T_left_shift,
    T_right_shift,
    T_ASMPC,

#define X(code, name, str) name = code,
#include "zx81chars.def"
#undef X
};

std::string decode_zx81(char c);
Bytes encode_zx81(const std::string& str, const SourceLoc& loc);
Bytes encode_zx81(const char*& p, const SourceLoc& loc);
Byte encode_zx81_char(const char*& p, const SourceLoc& loc);
std::string encode_hex(const Bytes& bytes);
