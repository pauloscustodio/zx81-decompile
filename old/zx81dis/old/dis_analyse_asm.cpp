//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "dis_analyse_asm.h"
#include "disz80.h"
#include "model.h"
#include "sysvars.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static void init_mem_map(Prog& prog) {
    for (int addr = VERSN; addr < PROG; addr++) {
        prog.mem.set_type(MemoryType::System, addr);
    }
    for (int addr = PROG; addr < prog.mem.peek_word(D_FILE); addr++) {
        prog.mem.set_type(MemoryType::Basic, addr);
    }
    for (int addr = prog.mem.peek_word(D_FILE); addr < prog.mem.peek_word(VARS);
            addr++) {
        prog.mem.set_type(MemoryType::Video, addr);
    }
    for (int addr = prog.mem.peek_word(VARS); addr < prog.mem.peek_word(E_LINE);
            addr++) {
        prog.mem.set_type(MemoryType::Vars, addr);
    }
    for (auto& line : prog.lines) {
        if (line.asm_length != 0) {
            for (int addr = line.asm_start_addr();
                    addr < line.asm_start_addr() + line.asm_length; addr++) {
                prog.mem.set_type(MemoryType::Unknown, addr);
            }
        }
    }
}

static void find_usr_routines(Prog& prog) {
    for (auto& line : prog.lines) {
        if (line.type != SourceType::BASIC) {
            continue;
        }
        for (auto& token : line.tokens) {
            if (token.keyword != Keyword::USR) {
                continue;
            }
            size_t pos = &token - &line.tokens[0] + 1;
            if (pos < line.tokens.size() &&
                    (line.tokens[pos].type == TokenType::Float ||
                     line.tokens[pos].type == TokenType::Integer)) {
                int usr_addr = (line.tokens[pos].type == TokenType::Float) ?
                               static_cast<int>(line.tokens[pos].nvalue) :
                               line.tokens[pos].ivalue;
                if (usr_addr >= VERSN && usr_addr < static_cast<int>(prog.mem.end_of_prog)) {
                    // USR routine; follow code and replace with label reference
                    prog.mem.set_code(prog, usr_addr);
                    if (get_error_count() > 0) {
                        return;    // prevent cascade of errors
                    }
                    std::string label = prog.dis_data.get_asm_label(usr_addr);
                    line.tokens[pos].type = TokenType::LabelRefAddr;
                    line.tokens[pos].text = "&" + label;
                    line.tokens[pos].svalue = label;
                    line.tokens[pos].ivalue = usr_addr;
                }
            }
        }
    }
}

void dis_analyse_asm(Prog& prog) {
    init_mem_map(prog);
    find_usr_routines(prog);
}
