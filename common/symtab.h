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
    std::string name;
    int value;
    SourceLoc loc;

    explicit Symbol(const std::string& name_, int value_,
                    const SourceLoc& loc_)
        : name(name_), value(value_), loc(loc_) {}

};

class Symtab {
public:
    explicit Symtab();
    virtual ~Symtab();
    Symtab(const Symtab& other) = delete;
    Symtab& operator=(const Symtab& other) = delete;

    void init();

    void add(const std::string& name, int value,
             const SourceLoc& loc);
    void update(const std::string& name, int value,
                const SourceLoc& loc);

    Symbol* get(const std::string& name) const;	// nullptr if not found
    Symbol* find(int value) const;			// last defined symbol with this value

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
