//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include "zx81chars.h"
#include <string>

struct Token {
    ZX81char code = T_none;
    int ivalue = 0;         // for T_integer
    double fvalue = 0.0;    // for T_float
    std::string svalue;     // for T_string
    std::string ident;      // for T_ident, T_line_addr, T_line_num
    Bytes bytes;            // for T_rem_code
};

