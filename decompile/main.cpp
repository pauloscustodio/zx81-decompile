//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "ctl_file.h"
#include "decompiler.h"
#include "disasm.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "utils.h"
#include "writter.h"
#include <cstdlib>
#include <iostream>
#include <string>

[[noreturn]]
static void exit_usage() {
    std::cerr << "Usage: zx81_decompile [-o file.bas] [-c file.ctl] file.p" <<
              std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    std::string ctl_file;
    std::string out_file;

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
        case 'd':
            optflags |= FLAG_DEBUG;
            break;
        default:
            exit_usage();
        }
    }

    if (argc != optind + 1) {
        exit_usage();
    }

    // get input and output file names
    std::string p_file = argv[optind];
    if (out_file.empty()) {
        out_file = replace_extension(p_file, ".bas");
    }

    // load memory and control file
    Memory* mem = new Memory();
    if (!mem->load_p_file(p_file)) {
        exit_error_status();
    }

    CtlFile* ctl = new CtlFile();
    if (!ctl_file.empty()) {
        if (!ctl->load_file(ctl_file)) {
            exit_error_status();
        }
    }

    // decompile
    Basic* basic = new Basic();
    DisasmCode* disasm = new DisasmCode(mem);
    Decompiler* decompiler = new Decompiler(mem, basic, ctl, disasm);
    decompiler->decompile();

    // output .bas file
    BasicInfo* basic_info = decompiler->get_basic_info();
    BasWriter* bas_writer = new BasWriter(mem, basic, basic_info, disasm);
    if (!bas_writer->write_bas_file(out_file)) {
        exit_error_status();
    }

    // release memory
    delete bas_writer;
    delete decompiler;
    delete disasm;
    delete basic;
    delete ctl;
    delete mem;

    exit_error_status();
}
