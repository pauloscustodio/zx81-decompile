//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "compiler.h"
#include "errors.h"
#include "memory.h"
#include "release_assert.h"
#include "zx81chars.h"
#include "zx81float.h"
#include <iostream>
#include <sstream>

static Compiler g_compiler;

void Compiler::assemble(AsmLine& asm_line) {
    // EQU
    if (!asm_line.equ_rpn.empty()) {
        if (pass == 1) {
            int value = basic->eval_const_expr(asm_line.equ_rpn);
            basic->symtab.update(Symbol::Type::Const, asm_line.label, value);
        }
    }
    else {
        if (!asm_line.label.empty()) {
            basic->symtab.update(Symbol::Type::Addr, asm_line.label, asmpc);
        }

        addr = poke_bytes(addr, asm_line.bytes);
        if (pass == 2) {
            err_set_filename(asm_line.source_filename);
            err_set_line_num(asm_line.source_line_num);

            for (auto& patch : asm_line.patches) {
                int value = 0;
                basic->eval_expr(patch.rpn, asmpc, value, true);	// show errors
                switch (patch.type) {
                case Patch::Type::Byte:
                    poke(asmpc + patch.offset, value);
                    break;
                case Patch::Type::Word:
                    dpoke(asmpc + patch.offset, value);
                    break;
                case Patch::Type::SByte:
                    if (value < -128 || value > 127) {
                        error("integer out of range", std::to_string(value));
                    }
                    poke(asmpc + patch.offset, value);
                    break;
                case Patch::Type::JrOffset:
                    value -= asmpc + 2;
                    if (value < -128 || value > 127) {
                        error("integer out of range", std::to_string(value));
                    }
                    poke(asmpc + patch.offset, value);
                    break;
                default:
                    release_assert(0);
                }
            }

            err_clear();
        }
    }
}

void compile(Basic& basic) {
    g_compiler.compile(basic);
}
