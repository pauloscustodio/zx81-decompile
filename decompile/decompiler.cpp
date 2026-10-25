//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "decompiler.h"
#include "errors.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <string>
#include <vector>

void Decompiler::decompile() {
    (void)ctl_file;
    decompile_sysvars();
    decompile_dfile();
    decompile_vars();
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

void Decompiler::decompile_vars() {
    int addr = mem->dpeek(VARS);
    int c = 0;
    while ((c = mem->peek(addr)) != 0x80) {
        BasicVar var;

        if ((c & 0xe0) == 0x60) {		// single letter variable
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::Number;
            var.name = decode_zx81(c);
            var.value = mem->fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
            var.type = BasicVar::Type::Number;

            // first letter
            addr++;
            c &= 0x3f;
            c |= 0x20;
            var.name = decode_zx81(c);

            // second, ... letter
            while (((c = mem->peek(addr)) & 0xc0) == 0x00) {
                addr++;
                var.name += decode_zx81(c);
            }

            // last letter
            c = mem->peek(addr);
            if ((c & 0xc0) != 0x80) {
                error("invalid multi-letter variable " + fmt_hex(c, 2)
                      + " at address " + fmt_hex(addr, 4));
                return;
            }
            else {
                addr++;
                c &= 0x7f;
                var.name += decode_zx81(c);
            }
            var.value = mem->fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0x80) {	// array of numbers
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ArrayNumbers;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dims = mem->peek(addr++);
            int num_elems = 1;
            for (int i = 0; i < num_dims; i++) {
                int dim = mem->dpeek(addr);
                addr += 2;
                num_elems *= dim;
                var.dims.push_back(dim);
            }
            for (int i = 0; i < num_elems; i++) {
                double value = mem->fpeek(addr);
                addr += 5;
                var.values.push_back(value);
            }

            release_assert(addr0 + size == addr);
        }
        else if ((c & 0xe0) == 0xe0) {	// for-next loop
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ForNextLoop;
            var.name = decode_zx81(c);

            var.value = mem->fpeek(addr);
            addr += 5;
            var.limit = mem->fpeek(addr);
            addr += 5;
            var.step = mem->fpeek(addr);
            addr += 5;
            var.line_num = mem->dpeek(addr);
            addr += 2;
        }
        else if ((c & 0xe0) == 0x40) {	// string
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::String;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            for (int i = 0; i < size; i++) {
                c = mem->peek(addr++);
                var.str += decode_zx81(c);
            }
        }
        else if ((c & 0xe0) == 0xc0) {	// array of strings
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::ArrayStrings;
            var.name = decode_zx81(c);

            int size = mem->dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dims = mem->peek(addr++);
            int num_elems = 1;
            for (int i = 0; i < num_dims; i++) {
                int dim = mem->dpeek(addr);
                addr += 2;
                num_elems *= dim;
                var.dims.push_back(dim);
            }

            int last_dim = var.dims.back();		// size of each string
            num_elems /= last_dim;

            for (int i = 0; i < num_elems; i++) {
                std::string str;
                for (int j = 0; j < last_dim; j++) {
                    c = mem->peek(addr++);
                    str += decode_zx81(c);
                }
                var.strs.push_back(str);
            }

            release_assert(addr0 + size == addr);
        }
        else {
            error("invalid variable marker " + fmt_hex(c, 2)
                  + " at address " + fmt_hex(addr, 4));
            break;
        }

        basic->basic_vars.push_back(var);
    }
}
