//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "decompiler.h"
#include "errors.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <string>
#include <vector>

void Decompiler::decompile() {
    decompile_sysvars();
    decompile_dfile();
}

void Decompiler::decompile_sysvars() {
    // get sysvars from memory
    basic->sysvars = mem->peek_bytes(SAVE_ADDR, PROG - SAVE_ADDR);

    // get autostart
    int d_file = mem->dpeek(D_FILE);
    int nxtlin = mem->dpeek(NXTLIN);
    if (nxtlin >= d_file) {
        basic->autostart = false;
        basic->autostart_line = 0;
    }
    else {
        basic->autostart = true;
        basic->autostart_line = mem->dpeek_be(nxtlin);
    }

    // fast
    if ((mem->peek(CDFLAG) & FlagSlow) == FlagSlow) {
        basic->fast = false;
    }
    else {
        basic->fast = true;
    }
}

void Decompiler::decompile_dfile() {
    int d_file = mem->dpeek(D_FILE);
    int vars = mem->dpeek(VARS);

    // dfile collapsed
    if (d_file + 1 + NumRows * (NumCols + 1) > vars) {
        basic->dfile_collapsed = true;
    }
    else {
        basic->dfile_collapsed = false;
    }

    // dfile lines
    if (mem->peek(d_file) != C_newline) {
        fatal_error("malformed dfile at address " + fmt_hex(d_file, 4)
                    + ": expected newline");
    }

    std::vector<std::string> dfile_lines;
    for (int addr = d_file + 1, row = 0; addr < vars; row++) {
        std::string dfile_line;
        for (int col = 0; ; col++) {
            int c = mem->peek(addr++);
            if (c == C_newline) {
                break;
            }
            if (col >= NumCols) {
                fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                            + ": line too long");
            }
            if ((c & 0x40) != 0) {
                fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                            + ": invalid character " + fmt_hex(c, 2));
            }

            dfile_line += decode_zx81(c);
        }
        if (row >= NumRows) {
            fatal_error("malformed dfile at address " + fmt_hex(addr - 1, 4)
                        + ": too many lines");
        }

        if (!basic->dfile_collapsed) {
            dfile_line = str_trim(dfile_line);
        }
        dfile_lines.push_back(dfile_line);
    }

    // trim empty lines at the end
    while (!dfile_lines.empty() && dfile_lines.back().empty()) {
        dfile_lines.pop_back();
    }

    basic->dfile_lines = dfile_lines;
}