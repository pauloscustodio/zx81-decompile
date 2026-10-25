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

struct Symbol {
    enum class Type { Const, Addr };
    Type type{ Type::Const };
    std::string name;
    int value;

    Symbol(Type type_ = Type::Const, const std::string& name_ = "", int value_ = 0);
};

class Symtab {
public:
    Symtab();
    virtual ~Symtab();
    Symtab(const Symtab& other) = delete;
    Symtab& operator=(const Symtab& other) = delete;

    void init();

    void add(Symbol::Type type, const std::string& name, int value);
    void update(Symbol::Type type, const std::string& name, int value);

    Symbol* get(const std::string& name);	// nullptr if not found
    Symbol* find(int value);			// last defined symbol with this value

    auto begin() {
        return symbols.begin();
    }
    auto end() {
        return symbols.end();
    }
    auto cbegin() const {
        return symbols.cbegin();
    }
    auto cend() const {
        return symbols.cend();
    }
    auto size() const {
        return symbols.size();
    }

private:
    std::unordered_map<std::string, Symbol*> symbols;
};

struct Patch {
    enum class Type { Byte, Word, SByte, JrOffset };
    Type type{ Type::Byte };
    int offset{ 0 };
    Expr rpn;

    Patch(Type type_ = Type::Byte) : type(type_) {}
};

