//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>

static void exit_usage() {
    std::cerr << "Usage: zx81_compile [-o file.p] file.bas" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    std::string out_file;

    while (true) {
        int c = getopt(argc, argv, const_cast<char*>("o:"));
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'o':
            out_file = optarg;
            break;
        default:
            exit_usage();
        }
    }

    if (argc != optind + 1) {
        exit_usage();
    }

    std::string b81_file = argv[optind];
    if (out_file.empty()) {
        out_file = replace_extension(b81_file, ".p");
    }

#if 0
    Basic basic;
    basic.parse_b81_file(b81_file);
    if (get_error_count()) {
        exit_status();
    }

    compile(basic);
#endif

    Memory* mem = new Memory();
    mem->write_p_file(out_file);
    if (get_error_count()) {
        exit_status();
    }

    delete mem;
    exit_status();
}
