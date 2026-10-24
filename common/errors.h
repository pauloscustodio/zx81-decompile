//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>

struct SourceLoc {
    std::string filename;
    int line;

    SourceLoc(const std::string& filename = "", int line = 0)
        : filename(filename), line(line) {}
};

int get_error_count();
void error(const SourceLoc& loc, const std::string& message);
void error(const std::string& message);
[[noreturn]] void fatal_error(const std::string& message);
[[noreturn]] void exit_status();
