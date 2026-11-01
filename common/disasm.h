//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "zx81consts.h"
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

class AsmLabels {
public:
    explicit AsmLabels();
    virtual ~AsmLabels() = default;

    void add(const std::string& name, int value);
    int get(const std::string& name) const;

    bool find(const std::string& name, int& value) const;
    bool find(int value, std::string& name) const;

    void clear();

    auto begin() {
        return by_name.begin();
    }
    auto end() {
        return by_name.end();
    }
    auto begin() const {
        return by_name.begin();
    }
    auto end() const {
        return by_name.end();
    }
    auto size() const {
        return by_name.size();
    }

private:
    std::unordered_map<std::string, int> by_name;
    std::unordered_map<int, std::string> by_value;
};

enum class MemType : uint8_t {
    Undefined = 0,
    Sysvars = '$',
    Dfile = '#',
    Vars = 'v',
    Basic = 'b',
    Asm = 'C',
    Byte = 'B',
    Word = 'W',
    Unknown = '-',
};

class DisasmCode {
public:
    explicit DisasmCode();
    virtual ~DisasmCode();
    DisasmCode(const DisasmCode& other) = delete;
    DisasmCode& operator=(const DisasmCode& other) = delete;

    AsmLabels& get_labels();
    const AsmLabels& get_labels() const;

    MemType get_mem_type(int addr) const;
    void set_mem_type(int addr, MemType type, int count = 1);
    void set_mem_type_no_overlap_check(int addr, MemType type, int count = 1);
    void set_code(int addr, const std::string& label);

private:
    AsmLabels labels;
    std::array<MemType, MEM_SIZE> mem_map{ MemType::Undefined };

};