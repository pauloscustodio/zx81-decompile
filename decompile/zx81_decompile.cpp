//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "ctl_file.h"
#include "decompiler.h"
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
    std::string out_filename;
    std::string ctl_filename;

    while (true) {
        int c = getopt(argc, argv, const_cast<char*>("do:c:"));
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'c':
            ctl_filename = optarg;
            break;
        case 'o':
            out_filename = optarg;
            break;
        default:
            exit_usage();
        }
    }

    if (argc != optind + 1) {
        exit_usage();
    }

    std::string p_file = argv[optind];
    if (out_filename.empty()) {
        out_filename = replace_extension(p_file, ".bas");
    }

    Memory* mem = new Memory();
    mem->read_p_file(p_file);

    CtlFile* ctl_file = new CtlFile();
    if (!ctl_filename.empty()) {
        ctl_file->load_ctl_file(ctl_filename);
    }

    Basic* basic = new Basic();
    Decompiler* decompiler = new Decompiler(mem, ctl_file, basic);
    decompiler->decompile();
    if (get_error_count()) {
        exit_status();
    }

    basic->write_bas_file(out_filename);

    delete decompiler;
    delete basic;
    delete ctl_file;
    delete mem;
    exit_status();
}
