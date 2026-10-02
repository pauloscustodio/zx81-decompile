//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "memory.h"
#include <string>

Symbol::Symbol(Type type_, const std::string& name_, int value_,
               const SourceLoc& loc_)
    : type(type_),
      name(name_),
      value(value_),
      loc(loc_) {
}

Symtab::Symtab() {
    init();
}

Symtab::~Symtab() {
    for (auto& pair : symbols) {
        delete pair.second;
    }
}

void Symtab::init() {
    for (auto& pair : symbols) {
        delete pair.second;
    }
    symbols.clear();

#define X(name, value)		add(Symbol::Type::Const, #name, value, SourceLoc());
#include "consts.def"
#undef X
}

void Symtab::add(Symbol::Type type, const std::string& name, int value,
                 const SourceLoc& loc) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        error(loc, "duplicate definition: " + name);
    }
    else {
        auto symbol = new Symbol(type, name, value, loc);
        symbols[name] = symbol;
    }
}

void Symtab::update(Symbol::Type type, const std::string& name, int value,
                    const SourceLoc& loc) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        it->second->type = type;
        it->second->value = value;
        it->second->loc = loc;
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
    return nullptr;
}

Symbol* Symtab::find(int value) {
    Symbol* result = nullptr;
    for (auto& pair : symbols) {
        if (pair.second->value == value) {
            result = pair.second;
        }
    }
    return result;
}

Basic::Basic() {
    clear();
}

void Basic::clear() {
    source_lines.clear();
    basic_vars.clear();
    video_lines.clear();
    video_collapsed = false;
    autostart = false;
    autostart_line_num = 0;
    auto_increment = 1;
    fast = false;
}
