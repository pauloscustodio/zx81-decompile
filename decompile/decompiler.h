//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "memory.h"
#include "basic.h"
#include "ctl_file.h"

class Decompiler {
public:
    explicit Decompiler(Memory* mem_, CtlFile* ctl_file_, Basic* basic_)
        : mem(mem_), ctl_file(ctl_file_), basic(basic_) {};
    virtual ~Decompiler() = default;

    void decompile();

private:
    Memory* mem = nullptr;
    CtlFile* ctl_file = nullptr;
    Basic* basic = nullptr;

    void decompile_sysvars();
    void decompile_dfile();
};
