//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "disasm.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "parser.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81chars.h"
#include <fstream>
#include <iostream>

//-----------------------------------------------------------------------------
// Basic
//-----------------------------------------------------------------------------

void Basic::write_basic_memory_map(std::ofstream& ofs) const {
    ofs << std::endl;
    int d_file = dpeek(D_FILE);
    int vars = dpeek(VARS);
    int e_line = dpeek(E_LINE);
    for (int addr = RAM_ADDR; addr < e_line; addr += 64) {
        ofs << "# [" << fmt_hex(addr, 4) << "] ";
        for (int p = addr; p < addr + 64; p++) {
            if (p < PROG) {
                ofs << "!";
            }
            else if (p < d_file) {
                switch (g_disasm_code.get_type(p)) {
                case Opcode::Type::Undef:
                    ofs << "#";
                    break;
                case Opcode::Type::Unknown:
                    ofs << "-";
                    break;
                case Opcode::Type::Asm:
                    ofs << "C";
                    break;
                case Opcode::Type::AsmData:
                    ofs << "C";
                    break;
                case Opcode::Type::Defb:
                    ofs << "B";
                    break;
                case Opcode::Type::DefbData:
                    ofs << "B";
                    break;
                case Opcode::Type::Defw:
                    ofs << "W";
                    break;
                case Opcode::Type::DefwData:
                    ofs << "W";
                    break;
                case Opcode::Type::Defm:
                    ofs << "T";
                    break;
                case Opcode::Type::DefmData:
                    ofs << "T";
                    break;
                default:
                    release_assert(0);
                }
            }
            else if (p < vars) {
                ofs << "*";
            }
            else {
                ofs << "$";
            }
        }
        ofs << std::endl;
    }
}

void Basic::write_mem_info(std::ofstream& ofs, int start_addr, int len) const {
    ofs << std::endl;			// newline after REM
    ofs << "#ASM" << std::endl;

    for (int addr = start_addr; addr < start_addr + len; ) {
        Opcode* opc = g_disasm_code.get(addr);
        ofs << opc->to_string();
        addr += opc->size;
    }

    ofs << "#ENDASM";	// followed by newline from REM
}
