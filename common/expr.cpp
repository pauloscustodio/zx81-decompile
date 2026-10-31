//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "expr.h"
#include "release_assert.h"
#include <vector>

int eval_expr(const Expr& expr, const Symtab& symtab) {
    std::vector<int> stack;
    int a = 0, b = 0, cond = 0;

    auto collect_symbol = [&](const std::string & name) {
        Symbol* symbol = symtab.get(name);
        if (symbol) {
            stack.push_back(symbol->value);
        }
        else {
            error(expr.loc, "undefined symbol: " + name);
            stack.push_back(0);
        }
    };

    for (auto& token : expr.rpn) {
        switch (token.code) {
        case T_integer:
            stack.push_back(token.ivalue);
            break;
        case T_symbol:
            collect_symbol(token.ident);
            break;
        case T_ASMPC:
            collect_symbol("ASMPC");
            break;
        case T_unary_minus:
            stack.back() = -stack.back();
            break;
        case T_log_not:
            stack.back() = !stack.back();
            break;
        case T_bin_not:
            stack.back() = ~stack.back();
            break;
        case C_mult:
            b = stack.back();
            stack.pop_back();
            stack.back() *= b;
            break;
        case C_div:
            b = stack.back();
            if (b == 0) {
                fatal_error("division by zero");
            }
            stack.pop_back();
            stack.back() /= b;
            break;
        case T_mod:
            b = stack.back();
            if (b == 0) {
                fatal_error("division by zero");
            }
            stack.pop_back();
            stack.back() %= b;
            break;
        case C_plus:
            b = stack.back();
            stack.pop_back();
            stack.back() += b;
            break;
        case C_minus:
            b = stack.back();
            stack.pop_back();
            stack.back() -= b;
            break;
        case T_left_shift:
            b = stack.back();
            stack.pop_back();
            stack.back() <<= b;
            break;
        case T_right_shift:
            b = stack.back();
            stack.pop_back();
            stack.back() >>= b;
            break;
        case C_lt:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a < b);
            break;
        case C_le:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a <= b);
            break;
        case C_gt:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a > b);
            break;
        case C_ge:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a >= b);
            break;
        case C_eq:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a == b);
            break;
        case C_ne:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a != b);
            break;
        case T_bin_and:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a & b);
            break;
        case T_bin_xor:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a ^ b);
            break;
        case T_bin_or:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a | b);
            break;
        case T_log_and:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a && b);
            break;
        case T_log_or:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            stack.push_back(a || b);
            break;
        case T_conditional:
            b = stack.back();
            stack.pop_back();
            a = stack.back();
            stack.pop_back();
            cond = stack.back();
            stack.pop_back();
            stack.push_back(cond ? a : b);
            break;

        default:
            release_assert(0);
        }
    }

    release_assert(stack.size() == 1);
    return stack.back();
}

int eval_const_expr(const Expr& expr) {
    Symtab dummy_symtab;
    return eval_expr(expr, dummy_symtab);
}
