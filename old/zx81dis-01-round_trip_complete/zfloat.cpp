//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "zfloat.h"
#include <array>
#include <cstdint>
#include <cmath>

double zx81_to_float(std::array<uint8_t, 5> bytes) {
    if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0 && bytes[3] == 0
            && bytes[4] == 0) {
        return 0.0;
    }
    else {
        int exp = bytes[0] - 128;
        double sign = (bytes[1] & 0x80) ? -1 : 1;
        uint32_t mant = ((bytes[1] | 0x80) << 24) | (bytes[2] << 16) |
                        (bytes[3] << 8) | (bytes[4]);
        double value = sign * mant * pow(2, exp - 32);
        return value;
    }
}
