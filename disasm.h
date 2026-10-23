//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class AsmLabels {
public:
    AsmLabels();

    void add(const std::string& name, int value);
    int get(const std::string& name);

    bool find(const std::string& name, int& value) const;
    bool find(int value, std::string& name) const;

    void clear();

    auto begin() {
        return by_name.begin();
    }
    auto end() {
        return by_name.end();
    }
    auto cbegin() const {
        return by_name.cbegin();
    }
    auto cend() const {
        return by_name.cend();
    }
    auto size() const {
        return by_name.size();
    }

private:
    std::unordered_map<std::string, int> by_name;
    std::unordered_map<int, std::string> by_value;
};

extern AsmLabels g_asm_labels;

struct Opcode {
    enum class Type { Undef, Unknown, Asm, AsmData, Defb, DefbData, Defw, DefwData, Defm, DefmData };
    Type type{ Type::Unknown };
    int addr{ 0 };
    int size{ 0 };
    std::string opcode;			// normalized form, e.g. "jp NN"
    std::string refer_to;		// label to refer to instead of nn
    int n{ 0 };
    int nn{ 0 };
    int dis{ 0 };
    bool is_jump{ false };
    bool ends_flow{ false };
    std::vector<int> values;		// for defb, defw
    std::string str;				// for defm

    Opcode(Type type_ = Type::Unknown, int addr_ = 0, int size_ = 1);
    std::string to_string();

private:
    std::string decode_opcode();
    std::string decode_undef();
    std::string decode_defb();
    std::string decode_defw();
    std::string decode_defm();
    std::string decode_label(int value);
};

class DisasmCode {
public:
    DisasmCode();
    virtual ~DisasmCode();
    DisasmCode(const DisasmCode& other) = delete;
    DisasmCode& operator=(const DisasmCode& other) = delete;

    Opcode::Type get_type(int addr);
    Opcode* get(int addr);
    std::string get_label(int addr);
    std::string get_header(int addr);
    std::string get_comment(int addr);

    void set_unknown(int addr, int size);
    void set_defb(int addr, int count);
    void set_defw(int addr, int count);
    void set_defm(int addr, int len);
    void set_code(int addr);
    void add_header(int addr, const std::string& line);
    void set_comment(int addr, const std::string& text);

private:
    std::array<Opcode*, MEM_SIZE> opcodes{ 0 };
    std::array<std::string*, MEM_SIZE> headers{ 0 };
    std::array<std::string*, MEM_SIZE> comments{ 0 };

    bool check_range_unknown(int addr, int size);
};

extern DisasmCode g_disasm_code;

class Disasm {
public:
    Opcode disasm(int addr_);

private:
    int addr{ 0 };
    Opcode opcode;

    static std::string dd1(int n, int x);
    static std::string dd2(int n, int x);
    static std::string r1(int n, int x);
    static std::string x1(int x);
    static std::string flags1(int n);
    static std::string flags2(int n);
    static std::string alu1(int n);
    static std::string rot1(int n);
    static std::string bit1(int n);

    void collect_nn();
    void collect_n();
    void collect_dis();
    void collect_jr();
};
