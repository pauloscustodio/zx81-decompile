//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include <string>

class Compiler {
public:
    void compile(Basic& basic);

private:
    Basic* basic{ nullptr };

    void compile_expr(const Expr& rpn);
    void compile_vars();
    void assemble(AsmLine& asm_line);
};

void compile(Basic& basic);
