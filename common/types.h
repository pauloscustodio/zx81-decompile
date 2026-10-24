//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <vector>

using Byte = uint8_t;
using Bytes = std::vector<uint8_t>;

#define NUM_ELEMS(a)	(sizeof(a) / sizeof(a[0]))
#define INT(x)			static_cast<int>(x)
#define BYTE(x)			static_cast<Byte>(x)
