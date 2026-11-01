//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include <string>
#include <vector>

struct CtlElement {
    enum class Type { None, Code };
    SourceLoc loc;
    Type type = Type::None;
    int addr = -1;
    std::string label;
};

struct CtlFile {
    std::vector<CtlElement> elems;

    void load_ctl_file(const std::string& filename);
};
