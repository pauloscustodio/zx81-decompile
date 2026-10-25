//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

struct CtlElement {
    enum class Type { Basic, Asm };
    Type type = Type::Basic;
    int line_num = -1;
    int addr = -1;
    std::string name;
};

struct CtlFile {
    std::vector<CtlElement> elements;

    void load_ctl_file(const std::string& filename);
};
