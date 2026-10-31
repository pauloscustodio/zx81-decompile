//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "symtab.h"

Symbol::Symbol(Type type_, const std::string& name_, int value_,
               const SourceLoc& loc_)
    : type(type_), name(name_), value(value_), loc(loc_) {
}

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

#define X(name, value)		add(Symbol::Type::Const, #name, value, SourceLoc());
#include "zx81consts.def"
}

void Symtab::add(Symbol::Type type, const std::string& name, int value,
                 const SourceLoc& loc) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        error(loc, "duplicate definition:" + name);
    }
    else {
        auto symbol = new Symbol(type, name, value, loc);
        symbols[name] = symbol;
    }
}

void Symtab::update(Symbol::Type type, const std::string& name, int value,
                    const SourceLoc& loc) {
    auto symbol = get(name);
    if (symbol) {
        symbol->type = type;
        symbol->value = value;
        symbol->loc = loc;
    }
    else {
        add(type, name, value, loc);
    }
}

Symbol* Symtab::get(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}

Symbol* Symtab::find(int value) {
    for (auto& it : symbols) {
        if (it.second->value == value) {
            return it.second;
        }
    }
    return nullptr;
}
