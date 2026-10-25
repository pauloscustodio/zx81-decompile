//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "types.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>

void Basic::write_bas_file(const std::string& filename) const {
    // open file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        fatal_error("write file: " + filename);
    }

    // AUTOSTART and FAST flags
    if (autostart) {
        file << "#AUTOSTART=1" << std::endl;
        file << "#AUTOSTART_LINE=" << autostart_line << std::endl;
    }

    if (fast) {
        file << "#FAST=1" << std::endl;
    }

    // DFILE_COLLAPSED and DFILE lines
    if (dfile_collapsed) {
        file << "#DFILE_COLLAPSED=1" << std::endl;
    }
    for (const auto& line : dfile_lines) {
        file << "#DFILE=\"" << line << "\"" << std::endl;
    }

    // SYSVARS
    Bytes sysvars = this->sysvars;
    while (!sysvars.empty() && sysvars.back() == 0) {
        sysvars.pop_back();
    }
    if (!sysvars.empty()) {
        file << "#SYSVARS=";
        bool first = true;
        for (auto byte : sysvars) {
            if (!first) {
                file << ",";
            }
            first = false;
            if (byte < 10) {
                file << static_cast<unsigned int>(byte);
            }
            else {
                file << fmt_hex(byte, 2);
            }
        }
        file << std::endl;
    }

    // dump BASIC VARS

    // dump BASIC source lines
}
