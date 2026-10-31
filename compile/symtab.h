//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include <string>
#include <unordered_map>

struct Symbol {
    enum class Type { Const, Addr };
    Type type = Type::Const;
    std::string name;
    int value;
    SourceLoc loc;

    explicit Symbol(Type type_ = Type::Const,
                    const std::string& name_ = "", int value_ = 0,
                    const SourceLoc& loc_ = SourceLoc());
};

class Symtab {
public:
    explicit Symtab();
    virtual ~Symtab();
    Symtab(const Symtab& other) = delete;
    Symtab& operator=(const Symtab& other) = delete;

    void init();

    void add(Symbol::Type type, const std::string& name, int value,
             const SourceLoc& loc);
    void update(Symbol::Type type, const std::string& name, int value,
                const SourceLoc& loc);

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

