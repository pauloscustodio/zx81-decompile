//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <array>
#include <cstdint>
using namespace std;

array<uint8_t, 5> float_to_zx81(double value);
double zx81_to_float(array<uint8_t, 5> bytes);
