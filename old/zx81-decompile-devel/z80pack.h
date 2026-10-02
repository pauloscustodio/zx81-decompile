//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

extern "C" {
#include "z80pack/sim.h"

    extern char Disass_Str[];
    extern char Opcode_Str[];

    void disass(int cpu, unsigned char** p, int adr, unsigned char* base);
};
