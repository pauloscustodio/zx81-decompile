//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include <array>
#include <cstdint>

std::array<Byte, 5> float_to_zx81(double value);
double zx81_to_float(const std::array<uint8_t, 5>& bytes);
