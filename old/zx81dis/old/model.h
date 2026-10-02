//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "lexer.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

struct Prog;

enum class MemoryType : uint8_t {
    Undefined = 0,      // not loaded
    Unknown,            // not disassembled yet
    Asm, AsmData,       // first and following bytes of asm opcodes
    Defb, DefbData,     // first and following bytes of byte memory
    Defw, DefwData,     // first and following bytes of word memory
    Defm, DefmData,     // first and following bytes of char memory
    System, Basic, Video, Vars,     // basic system
};

char memory_type_char(MemoryType type);

struct Opcode {
    MemoryType type = MemoryType::Undefined;
    int addr = 0;
    int size = 0;
    std::string opcode;         // cannonical opcode, e.g. "jp NN"
    std::string refer_to;       // label to refer to instead of NN
    int n = 0, nn = 0, dis = 0; // opcode parameters
    bool is_jump = false;
    bool ends_flow = false;
    std::vector<int> values;    // for defb, defw
    std::string str;            // for defm

    explicit Opcode() = default;
    explicit Opcode(MemoryType type, int addr, int size);
    std::string to_string(Prog& prog);

private:
    std::string decode_opcode(Prog& prog);
    std::string decode_undef(Prog& prog);
    std::string decode_defb(Prog& prog);
    std::string decode_defw(Prog& prog);
    std::string decode_defm(Prog& prog);
    std::string decode_label(Prog& prog, int value);

};

struct Memory {
    static inline const int BASE_ADDR = 0x4000;
    std::vector<MemoryType> data_type;
    std::vector<Opcode> opcodes;   // disassembled code

    MemoryType get_type(int addr) const;
    void set_type(MemoryType type, int addr);
    void set_type(MemoryType type, int addr, int size);

    Opcode& get_opcode(int addr);
    void set_unknown(int addr, int count);
    void set_defb(int addr, int count);
    void set_defw(int addr, int count);
    void set_defm(int addr, int count);
    void set_code(Prog& prog, int addr);
};

struct Comments {
    void clear();
    void add(int addr, const std::string& line);
    std::vector<std::string> get(int addr) const;

private:
    std::unordered_map<int, std::vector<std::string>> lines;
};

struct DisData {
    Labels basic_labels;
    Comments basic_header;
    Labels asm_labels;
    Comments asm_header;        // comment before the line
    Comments asm_comments;      // comment at the line

    std::string get_basic_label(int line_num);
    std::string get_asm_label(int addr);
};

struct Prog {
    std::string bas_filename;
    std::string p_filename;

    bool autostart = false;
    int autostart_line = 0;
    bool fast = false;
    int increment = 1;
    std::vector<uint8_t> sysvars;
    std::vector<std::string> dfile;
    std::vector<Variable> variables;
    std::vector<uint8_t> trailer;
    bool dfile_colapsed = false;

    std::vector<Line> lines;

    Memory mem;

    // disassembly data
    DisData dis_data;
};

std::string double_to_string(double value);
std::string fixed_width_string(const std::string& text, size_t width);
