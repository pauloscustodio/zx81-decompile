//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "../config.h"
#include "ass_analyse.h"
#include "ass_emit_p.h"
#include "ass_parser.h"
#include "dis_analyse_asm.h"
#include "dis_analyse_bas.h"
#include "dis_data.h"
#include "dis_decode.h"
#include "dis_emit_bas.h"
#include "errors.h"
#include "model.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>

static const std::string COPYRIGHT =
    "Copyright (C) Paulo Custodio 2023-2026\n"
#ifdef Z88DK_VERSION
    "Version: " Z88DK_VERSION "\n"
#endif
    ;

[[noreturn]]
static void exit_usage() {
    std::cerr << COPYRIGHT;
    std::cerr << "Usage: z88dk-zx81dis <file.p>        --> builds <file.bas>" <<
              std::endl;
    std::cerr << "Usage: z88dk-zx81dis [-r] <file.bas> --> builds <file.p>" <<
              std::endl;
    exit(EXIT_FAILURE);
}

static void assemble(const std::string& bas_filename) {
    if (!str_ends_with(bas_filename, ".bas")) {
        fatal("Input file must have .bas extension");
    }

    std::string p_filename = replace_extension(bas_filename, ".p");

    Prog prog;
    ass_parse_prog(prog, bas_filename);
    ass_analyse_prog(prog);
    ass_emit_p(p_filename, prog);
}

static void disassemble(const std::string& p_filename) {
    if (!str_ends_with(p_filename, ".p")) {
        fatal("Input file must have .p extension");
    }
    std::string bas_filename = replace_extension(p_filename, ".bas");
    std::string dat_filename = replace_extension(p_filename, ".dat");

    Prog prog;
    dis_decode_prog(prog, p_filename);
    dis_load_dat(prog, dat_filename);
    dis_analyse_bas(prog);
    dis_analyse_asm(prog);
    dis_emit_bas(bas_filename, prog);
}

int main(int argc, char* argv[]) {
    bool do_disassemble = true;
    if (argc > 1 && std::string(argv[1]) == "-r") {
        do_disassemble = false;
        argc--;
        argv++;
    }

    if (argc != 2) {
        exit_usage();
    }

    if (do_disassemble) {
        std::string p_filename = argv[1];
        disassemble(p_filename);
    }
    else {
        std::string bas_filename = argv[1];
        assemble(bas_filename);
    }

    exit_error_status();
}

