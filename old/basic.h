//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include "zx81chars.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::vector<Token> Expr;

struct Patch {
    enum class Type { Byte, Word, SByte, JrOffset };
    Type type{ Type::Byte };
    int offset{ 0 };
    Expr rpn;

    Patch(Type type_ = Type::Byte) : type(type_) {}
};

