//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include "symtab.h"
#include "token.h"
#include <vector>

struct Expr {
    std::vector<Token> rpn; // Reverse Polish Notation
    SourceLoc loc; // location of the expression in the source code

    explicit Expr(const SourceLoc& loc_) : loc(loc_) {}
    bool empty() const {
        return rpn.empty();
    }
};

struct Patch {
    enum class Type { Byte, Word, SByte, JrOffset };
    Type type = Type::Byte;
    int offset = 0;
    Expr expr;

    explicit Patch(Type type_, const SourceLoc& loc_)
        : type(type_), expr(loc_) {}
};

int eval_expr(const Expr& expr, const Symtab& symtab);
int eval_const_expr(const Expr& expr);
