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
    std::cerr << "Usage: zx81_decompile [-o file.bas] [-d] file.p" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    std::string out_file;
    std::string ctl_file;

    while (true) {
        int c = getopt(argc, argv, const_cast<char*>("do:c:"));
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'c':
            ctl_file = optarg;
            break;
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

    std::string p_file = argv[optind];
    if (out_file.empty()) {
        out_file = replace_extension(p_file, ".bas");
    }

    Memory* mem = new Memory();
    mem->read_p_file(p_file);

#if 0
    if (err_get_count()) {
        exit_status();
    }

    Basic basic;
    if (ctl_file.empty()) {
        decompile(basic);
    }
    else {
        decompile(basic, ctl_file);
    }
    if (err_get_count()) {
        exit_status();
    }

    basic.write_b81_file(out_file);
#endif

    delete mem;
    exit_status();
}
