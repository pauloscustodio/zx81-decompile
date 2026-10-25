//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "ctl_file.h"
#include "errors.h"
#include <fstream>

void CtlFile::load_ctl_file(const std::string& filename) {
    elements.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        fatal_error("open file: " + filename);
    }
}
