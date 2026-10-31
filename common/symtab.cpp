//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "symtab.h"

Symtab::Symtab() {
    init();
}

Symtab::~Symtab() {
    for (auto& it : symbols) {
        delete it.second;
    }
    symbols.clear();
}

void Symtab::init() {
    for (auto& it : symbols) {
        delete it.second;
    }
    symbols.clear();

#define X(name, value)		add(#name, value, SourceLoc());
#include "zx81consts.def"
}

void Symtab::add(const std::string& name, int value,
                 const SourceLoc& loc) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        error(loc, "duplicate definition: " + name);
    }
    else {
        auto symbol = new Symbol(name, value, loc);
        symbols[name] = symbol;
    }
}

void Symtab::update(const std::string& name, int value,
                    const SourceLoc& loc) {
    auto symbol = get(name);
    if (symbol) {
        symbol->value = value;
        symbol->loc = loc;
    }
    else {
        add(name, value, loc);
    }
}

Symbol* Symtab::get(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}

Symbol* Symtab::find(int value) const {
    for (auto& it : symbols) {
        if (it.second->value == value) {
            return it.second;
        }
    }
    return nullptr;
}
