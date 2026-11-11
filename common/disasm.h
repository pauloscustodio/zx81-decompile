//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "memory.h"
#include "zx81consts.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Labels {
public:
    explicit Labels() = default;
    virtual ~Labels() = default;

    virtual void init() {}
    virtual std::string add_or_get_from_value(int value) = 0;

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

class BasicLabels : public Labels {
public:
    explicit BasicLabels() = default;
    virtual ~BasicLabels() = default;

    std::string add_or_get_from_value(int line_num) override;
};

class AsmLabels : public Labels {
public:
    explicit AsmLabels();
    virtual ~AsmLabels() = default;

    void init() override;
    std::string add_or_get_from_value(int addr) override;
};

enum class MemType : uint8_t {
    Undefined = 0,  // not part of loaded memory
    Sysvars = '$',  // system variables
    Dfile = '#',    // display file
    Vars = 'v',     // BASIC variables
    Basic = 'b',    // BASIC code
    Asm = 'C',      // code
    Byte = 'B',     // defb
    Word = 'W',     // defw
    Text = 'T',     // defm
    Unknown = '-',  // part of memory but not yet known
};

// hold one disassembled instruction or data
struct Opcode {
    MemType type = MemType::Unknown;
    int addr = 0;
    int size = 0;
    std::string opcode;			// normalized form, e.g. "jp NN"
    std::string refer_to;		// label to refer to instead of nn
    int n = 0;                  // for ld a, n
    int nn = 0;                 // for jp nn, call nn, defb nn, defw nn
    int dis = 0;                // for inc (ix+dis), ld a, (ix+dis), etc
    bool is_jump = false;       // true if nn is a jump target
    bool ends_flow = false;     // true if instruction ends flow
    std::vector<int> values;	// for defb, defw
    std::string str;			// for defm

    explicit Opcode(MemType type_ = MemType::Unknown, int addr_ = 0, int size_ = 1)
        : type(type_), addr(addr_), size(size_) {}

    std::string to_string(const AsmLabels& labels) const;

private:
    std::string decode_opcode(const AsmLabels& labels) const;
    std::string decode_defb() const;
    std::string decode_defw(const AsmLabels& labels) const;
    std::string decode_defm() const;
    std::string decode_label(const AsmLabels& labels, int value) const;
};

// do Z80 disassembly of a single instruction at a given address
class Disasm {
public:
    explicit Disasm(Memory* mem_) : mem(mem_) {}
    virtual ~Disasm() = default;

    // returns false if instruction does not exist
    bool disasm(int addr_, Opcode& opcode);

private:
    Memory* mem = nullptr;
    int addr = 0;

    static std::string dd1(int n, int x);
    static std::string dd2(int n, int x);
    static std::string r1(int n, int x);
    static std::string x1(int x);
    static std::string flags1(int n);
    static std::string flags2(int n);
    static std::string alu1(int n);
    static std::string rot1(int n);
    static std::string bit1(int n);

    void collect_nn(Opcode& opcode);
    void collect_n(Opcode& opcode);
    void collect_dis(Opcode& opcode);
    void collect_jr(Opcode& opcode);
};

class DisasmCode {
public:
    DisasmCode(Memory* mem_) : mem(mem_), disasm(mem_) {}

    // BASIC labels
    BasicLabels& get_basic_labels();
    const BasicLabels& get_basic_labels() const;

    // ASM labels
    AsmLabels& get_asm_labels();
    const AsmLabels& get_asm_labels() const;

    // memory type
    MemType get_mem_type(int addr) const;
    void set_mem_type(int addr, MemType type, int count = 1);
    void set_mem_type_no_overlap_check(int addr, MemType type, int count = 1);
    void set_code(int addr);
    void set_byte(int addr, int count = 1);
    void set_word(int addr, int count = 1); // number of words, not bytes
    void set_text(int addr, int count = 1);

    // opcode
    const Opcode* get_opcode(int addr) const;

    // header and comment
    void add_header(int addr, const std::string& header);
    void add_comment(int addr, const std::string& comment);
    const std::vector<std::string>& get_header(int addr) const;
    const std::vector<std::string>& get_comment(int addr) const;

    // convert opcode to string
    std::string to_string(int addr, int size) const;

private:
    Memory* mem = nullptr;
    Disasm disasm;
    BasicLabels basic_labels;
    AsmLabels asm_labels;

    // memory type map, disassembled opcodes, headers and comments per address
    std::array<MemType, MEM_SIZE> mem_map{ MemType::Undefined };
    std::array<std::unique_ptr<Opcode>, MEM_SIZE> opcodes;
    std::array<std::unique_ptr<std::vector<std::string>>, MEM_SIZE> headers;
    std::array<std::unique_ptr<std::vector<std::string>>, MEM_SIZE> comments;
};
