//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include "labels.h"
#include "memory.h"
#include <array>
#include <string>
#include <vector>

struct Opcode {
    enum class Type {
        Undef, Unknown,
        Asm, AsmData,
        Defb, DefbData,
        Defw, DefwData,
        Defm, DefmData
    };
    Type type = Type::Unknown;
    int addr = 0;
    int size = 0;
    std::string opcode;			// normalized form, e.g. "jp NN"
    std::string refer_to;		// label to refer to instead of nn
    int n = 0;
    int nn = 0;
    int dis = 0;
    bool is_jump = false;
    bool ends_flow = false;
    std::vector<int> values;		// for defb, defw
    std::string str;				// for defm

    explicit Opcode() = default;
    explicit Opcode(Type type_, int addr_, int size_ = 1);
};

class DisasmCode {
public:
    explicit DisasmCode(Memory* memory) : mem(memory) {}
    virtual ~DisasmCode();
    DisasmCode(const DisasmCode& other) = delete;
    DisasmCode& operator=(const DisasmCode& other) = delete;

    Opcode::Type get_type(int addr);
    Opcode* get(int addr);
    std::string get_add_label(int addr);
    std::string get_header(int addr);
    std::string get_comment(int addr);

    void set_unknown(int addr, int size);
    void set_defb(int addr, int count);
    void set_defw(int addr, int count);
    void set_defm(int addr, int len);
    void set_code(int addr);

    void set_label(int addr, const std::string& label);
    void add_header(int addr, const std::string& line);
    void set_comment(int addr, const std::string& text);

    AsmLabels& get_labels();

private:
    Memory* mem = nullptr;
    std::array<Opcode*, MEM_SIZE> opcodes{ 0 };
    std::array<std::string*, MEM_SIZE> headers{ 0 };
    std::array<std::string*, MEM_SIZE> comments{ 0 };
    AsmLabels asm_labels;

    bool check_range_unknown(int addr, int size);
};

class Disasm {
public:
    explicit Disasm(Memory* mem_) : mem(mem_) {}

    Opcode disasm(int addr_);

private:
    Memory* mem = nullptr;
    int addr = 0;
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
