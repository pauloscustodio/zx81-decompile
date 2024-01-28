//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "zfloat.h"
#include <cmath>
#include <cstdint>
using namespace std;

// unions to access to double raw bytes
union mydouble {
	double	 value;
	uint8_t  bytes[8];
	uint64_t raw;	// 1 bit sign, 11 bits exponent, 52 bits mantissa, bias 1023
};
static const int mydouble_exp_bias = 1023;
static_assert(sizeof(double) == 8, "expected 8 bytes");
static_assert(sizeof(mydouble) == 8, "expected 8 bytes");

// 1 byte exponent
// 4 bytes mantissa, with first bit replaced by sign bit
array<uint8_t, 5> float_to_zx81(double value) {
	array<uint8_t, 5> out{ 0 };

	if (value == 0.0)
		return out;

	mydouble f;
	f.value = value;

	// convert to zx81 format
	uint64_t exponent = ((f.raw >> 52) & 0x7ff) - mydouble_exp_bias + 129;
	uint64_t mantissa = (f.raw >> (52 - 32 + 1));	// align first bit to bit30, bit31 is sign
	if (value < 0.0)
		mantissa |= 1LL << 31;
	else
		mantissa &= ~(1LL << 31);

	// return
	size_t i = 0;
	out[i++] = exponent & 0xff;
	while (i < 5) {
		out[i++] = (mantissa >> 24) & 0xff;
		mantissa <<= 8;
	}
	return out;
}

double zx81_to_float(array<uint8_t, 5> bytes) {
	if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0 && bytes[3] == 0 && bytes[4] == 0)
		return 0.0;
	else {
		int exp = bytes[0] - 128;
		double sign = (bytes[1] & 0x80) ? -1 : 1;
		uint32_t mant = ((bytes[1] | 0x80) << 24) | (bytes[2] << 16) | (bytes[3] << 8) | (bytes[4]);
		double value = sign * mant * pow(2, exp - 32);
		return value;
	}
}
