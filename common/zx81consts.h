//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

enum ZX81const {
#define X(name, value)		name = value,
#include "zx81consts.def"
#undef X
};

static inline const int MEM_SIZE = 0x10000;
static inline const int RAM_ADDR = ERR_NO;
static inline const int SAVE_ADDR = VERSN;
static inline const int NumRows = 24;
static inline const int NumCols = 32;
static inline const int MaxLineNum = 0x3fff;
static inline const int FlagSlow = 0x40;
static inline const int FlagFast = 0x00;
