//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disasm.h"
#include "errors.h"
#include "utils.h"
#include <iomanip>
#include <iostream>
#include <sstream>

AsmLabels::AsmLabels() {
#define X(name, value)		add(#name, value);
#include "zx81consts.def"
#undef X
}

void AsmLabels::add(const std::string& name, int value) {
    auto it = by_name.find(name);
    if (it != by_name.end()) {
        if (it->second == value) {
            return;     // nothing to do
        }
        error("label redefinition: " + name + " = " + fmt_hex(value,
                4) + " != " + fmt_hex(it->second, 4));
    }

    by_name[name] = value;
    by_value[value] = name;		// overwrite if tow labels with same value
}

int AsmLabels::get(const std::string& name) const {
    int value = 0;
    if (find(name, value)) {
        return value;
    }

    error("label undefined: " + name);
    return 0;
}

bool AsmLabels::find(const std::string& name, int& value) const {
    auto it = by_name.find(name);
    if (it == by_name.end()) {
        return false;
    }

    value = it->second;
    return true;
}

bool AsmLabels::find(int value, std::string& name) const {
    auto it = by_value.find(value);
    if (it == by_value.end()) {
        return false;
    }

    name = it->second;
    return true;
}

void AsmLabels::clear() {
    by_name.clear();
    by_value.clear();
}

DisasmCode::DisasmCode() = default;
DisasmCode::~DisasmCode() = default;

AsmLabels& DisasmCode::get_labels() {
    return labels;
}

const AsmLabels& DisasmCode::get_labels() const {
    return labels;
}

std::string DisasmCode::get_line_num_label(int line_num) {
    std::string label;
    if (labels.find(line_num, label)) {
        return label;
    }

    std::ostringstream oss;
    oss << "Line" << std::setw(5) << std::setfill('_') << line_num;
    label = oss.str();
    labels.add(label, line_num);
    return label;
}

std::string DisasmCode::get_asm_label(int addr) {
    std::string label;
    if (labels.find(addr, label)) {
        return label;
    }

    label = fmt_hex(addr, 4, "L");
    labels.add(label, addr);
    return label;
}

MemType DisasmCode::get_mem_type(int addr) const {
    return mem_map[addr & 0xffff];
}

void DisasmCode::set_mem_type(int addr, MemType type, int count) {
    for (int i = 0; i < count; ++i) {
        if (mem_map[(addr + i) & 0xffff] != MemType::Undefined &&
                mem_map[(addr + i) & 0xffff] != MemType::Unknown) {
            error("memory overlap at address " + fmt_hex(addr + i, 4));
        }
        mem_map[(addr + i) & 0xffff] = type;
    }
}

void DisasmCode::set_mem_type_no_overlap_check(int addr, MemType type,
        int count) {
    for (int i = 0; i < count; ++i) {
        mem_map[(addr + i) & 0xffff] = type;
    }
}

void DisasmCode::set_code(int addr, const std::string& label) {
    std::cout << "set_code: " << fmt_hex(addr, 4) << " = " << label << std::endl;

    if (!label.empty()) {
        labels.add(label, addr);
    }
}
