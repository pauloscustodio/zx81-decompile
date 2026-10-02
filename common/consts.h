//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>

#define NUM_ELEMS(a)	(sizeof(a) / sizeof(a[0]))
#define INT(x)			static_cast<int>(x)

typedef uint8_t Byte;
typedef std::vector<uint8_t> Bytes;

enum ZX81const {
#define X(name, value)		name = value,
#include "consts.def"
#undef X
};

static inline constexpr int MEM_SIZE = 0x10000;
static inline constexpr int RAM_ADDR = ERR_NO;
static inline constexpr int SAVE_ADDR = VERSN;
static inline constexpr int NumRows = 24;
static inline constexpr int NumCols = 32;
static inline constexpr int MaxLineNum = 0x3fff;
static inline constexpr int FlagFast = 0;
static inline constexpr int FlagSlow = 0x40;

static inline constexpr double Epsilon = 1e-10;
