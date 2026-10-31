//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "compiler.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "parser.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>

static void exit_usage() {
    std::cerr << "Usage: zx81_compile [-o file.p] file.bas" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    std::string out_filename;

    while (true) {
        int c = getopt(argc, argv, const_cast<char*>("o:"));
        if (c == -1) {
            break;
        }
        switch (c) {
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

    std::string bas_filename = argv[optind];
    if (out_filename.empty()) {
        out_filename = replace_extension(bas_filename, ".p");
    }

    Basic* basic = new Basic();
    Parser* parser = new Parser(basic);
    parser->parse_bas_file(bas_filename);
    if (get_error_count()) {
        exit_status();
    }

    Compiler* compiler = new Compiler(basic);
    compiler->compile();
    if (get_error_count()) {
        exit_status();
    }

    compiler->write_p_file(out_filename);

    delete compiler;
    delete parser;
    delete basic;
    exit_status();
}
