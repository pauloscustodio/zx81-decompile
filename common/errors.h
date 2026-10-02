//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>

struct SourceLoc {
    std::string filename;	// source location
    int line_num = 0;

    explicit SourceLoc() = default;
    explicit SourceLoc(const std::string& filename_, int line_num_)
        : filename(filename_),
          line_num(line_num_) {}
};

void error(const SourceLoc& loc, const std::string& message);
void error(const std::string& message);
int get_error_count();
[[noreturn]] void fatal_error(const std::string& message);
[[noreturn]] void exit_error_status();
