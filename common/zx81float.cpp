//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "zx81float.h"
#include <array>
#include <cmath>
#include <cstdint>

std::array<uint8_t, 5> float_to_zx81(double value) {
    std::array<uint8_t, 5> out{};

    if (value == 0.0) {
        return out;
    }

    // Decompose:
    // value = significand * 2^exp
    // where 0.5 <= |significand| < 1.0
    int exp;
    double significand = std::frexp(std::fabs(value), &exp);

    // Convert to ZX81 exponent.
    // IEEE exponent stores a normalized 1.x value,
    // while frexp() returns 0.5 <= significand < 1.
    uint8_t zx_exp = static_cast<uint8_t>(exp + 128);

    // Generate 31 mantissa bits.
    uint32_t mantissa = 0;
    double frac = significand * 2.0 - 1.0; // remove implicit leading 1

    for (int i = 0; i < 31; ++i) {
        frac *= 2.0;
        mantissa <<= 1;
        if (frac >= 1.0) {
            mantissa |= 1;
            frac -= 1.0;
        }
    }

    // Put sign into bit 31.
    if (std::signbit(value)) {
        mantissa |= 0x80000000u;
    }

    out[0] = zx_exp;
    out[1] = static_cast<uint8_t>(mantissa >> 24);
    out[2] = static_cast<uint8_t>(mantissa >> 16);
    out[3] = static_cast<uint8_t>(mantissa >> 8);
    out[4] = static_cast<uint8_t>(mantissa);

    return out;
}

double zx81_to_float(const std::array<uint8_t, 5>& bytes) {
    if (bytes == std::array<uint8_t, 5> {})
        return 0.0;

    int exp = static_cast<int>(bytes[0]) - 128;
    double sign = (bytes[1] & 0x80) ? -1.0 : 1.0;

    uint32_t mant =
        (static_cast<uint32_t>(bytes[1] | 0x80) << 24) |
        (static_cast<uint32_t>(bytes[2]) << 16) |
        (static_cast<uint32_t>(bytes[3]) << 8)  |
        static_cast<uint32_t>(bytes[4]);

    return std::ldexp(sign * static_cast<double>(mant), exp - 32);
}
