//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "compiler.h"
#include "errors.h"
#include "getopt.h"
#include "parser.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>

static void exit_usage() {
    std::cerr << "Usage: zx81_compile [-m file.map] [-o file.p] file.bas" <<
              std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    std::string out_filename;
    std::string map_filename;

    while (true) {
        int c = getopt(argc, argv, const_cast<char*>("m:o:"));
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'm':
            map_filename = optarg;
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

    // get the input .bas filename
    std::string bas_filename = argv[optind];
    if (out_filename.empty()) {
        out_filename = replace_extension(bas_filename, ".p");
    }

    // parse
    Basic* basic = new Basic();
    Parser* parser = new Parser(basic);
    parser->parse_bas_file(bas_filename);
    if (get_error_count()) {
        exit_status();
    }

    // compile
    Compiler* compiler = new Compiler(basic);
    compiler->compile();
    if (get_error_count()) {
        exit_status();
    }

    // write output files
    compiler->write_p_file(out_filename);
    if (!map_filename.empty()) {
        compiler->write_map_file(map_filename);
    }

    delete compiler;
    delete parser;
    delete basic;
    exit_status();
}
