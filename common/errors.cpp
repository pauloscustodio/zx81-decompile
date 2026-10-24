//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include <cstdlib>
#include <iostream>

static int num_errors = 0;

void error(const SourceLoc& loc, const std::string& message) {
    if (loc.filename.empty()) {
        std::cout << "error: " << message << std::endl;
    }
    else {
        std::cout << loc.filename << ":" << loc.line << ": error: " << message <<
                  std::endl;
    }
    ++num_errors;
}

void error(const std::string& message) {
    error(SourceLoc(), message);
}

void fatal_error(const std::string& message) {
    error(message);
    exit_status();
}

[[noreturn]] void exit_status() {
    if (num_errors > 0) {
        std::exit(EXIT_FAILURE);
    }
    else {
        std::exit(EXIT_SUCCESS);
    }
}
