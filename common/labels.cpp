//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "labels.h"
#include "utils.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

Labels::Labels() {
    clear();
}

void Labels::clear() {
    by_name.clear();
    by_value.clear();
}

void Labels::add(const std::string& name, int value) {
    auto it = by_name.find(name);
    if (it != by_name.end()) {
        if (it->second != value) {
            error("label redefinition: " + name + " = " + fmt_hex(value, 4));
        }
    }

    by_name[name] = value;
    by_value[value] = name;		// overwrite if two labels with same value
}

int Labels::get(const std::string& name) const {
    int value = 0;
    if (find(name, value)) {
        return value;
    }
    else {
        error("label undefined: " + name);
        return 0;
    }
}

std::string Labels::get_add(int value) {
    std::string name;
    if (find(value, name)) {
        return name;
    }
    else {
        name = generate_label(value);
        add(name, value);
        return name;
    }
}

bool Labels::find(const std::string& name, int& value) const {
    auto it = by_name.find(name);
    if (it == by_name.end()) {
        return false;
    }
    else {
        value = it->second;
        return true;
    }
}

bool Labels::find(int value, std::string& name) const {
    auto it = by_value.find(value);
    if (it == by_value.end()) {
        return false;
    }
    else {
        name = it->second;
        return true;
    }
}

std::vector<std::pair<std::string, int>> Labels::all_unsorted() const {
    std::vector<std::pair<std::string, int>> out;
    for (auto& [name, value] : by_name) {
        out.emplace_back(std::make_pair(name, value));
    }
    return out;
}

std::vector<std::pair<std::string, int>> Labels::sorted_by_name() const {
    auto sorted = all_unsorted();
    std::sort(sorted.begin(), sorted.end(),
    [](const auto & a, const auto & b) {
        return a.first < b.first;
    });
    return sorted;
}

std::vector<std::pair<std::string, int>> Labels::sorted_by_value() const {
    auto sorted = all_unsorted();
    std::sort(sorted.begin(), sorted.end(),
    [](const auto & a, const auto & b) {
        return a.second < b.second;
    });
    return sorted;
}

AsmLabels::AsmLabels() : Labels() {
    init();
}

void AsmLabels::init() {
#define X(name, value)		add(#name, value);
#include "consts.def"
#undef X
}

std::string AsmLabels::generate_label(int value) const {
    return "L" + fmt_hex(value, 4, "");
}

BasicLabels::BasicLabels() : Labels() {
    init();
}

void BasicLabels::init() {
}

std::string BasicLabels::generate_label(int value) const {
    std::ostringstream oss;
    oss << "LINE_" << std::setw(4) << std::setfill('0') << value;
    return oss.str();
}
