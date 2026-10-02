//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

using uint = unsigned int;

#define X(name, value)  static inline constexpr uint name = value;
#include "sysvars.def"
#undef X
