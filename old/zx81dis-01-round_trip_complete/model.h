//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "lexer.h"

using uint = unsigned int;

enum class MemoryType : uint8_t {
    Unknown = 0,
    Code,
    DataChar,
    DataByte,
    DataWord,
};

struct Memory {
    static inline const uint BASE_ADDR = 0x4000;
    std::vector<uint8_t> data;
    std::vector<MemoryType> data_type;
    uint end_of_prog = 0;

    void clear();
    uint size() const {
        return static_cast<uint>(data.size());
    }
    uint base_addr() const {
        return BASE_ADDR;
    }
    uint end_addr() const {
        return BASE_ADDR + static_cast<uint>(data.size());
    }

    uint8_t peek(uint addr) const;
    uint16_t peek_word(uint addr) const;
    uint16_t peek_word_be(uint addr) const; // big-endian
    void poke(uint addr, uint8_t value);
    void poke_word(uint addr, uint16_t value);
    void poke_word_be(uint addr, uint16_t value); // big-endian

    MemoryType get_type(uint addr) const;
    void set_type(MemoryType type, uint addr);
    void set_type(MemoryType type, uint addr, uint size);

    void load_file(const std::string& filename, uint addr);
};

struct Variable {
    std::string name;
    bool is_string_var = false;
    bool is_array_var = false;
    bool is_loop_var = false;
    double limit = 0.0;
    double step = 0.0;
    int line_num = -1;
    std::vector<int> dims;
    std::vector<double> nvalues;
    std::vector<std::string> svalues;
};

struct Line {
    SourceType type = SourceType::BASIC;
    int line_num = -1;
    std::string label;
    std::vector<Token> tokens;
    uint start_addr = 0;        // address of the line in memory
    uint asm_length = 0;        // length of the ASM code in bytes

    uint asm_start_addr() const {
        return start_addr + 5;
    }

    bool empty() const;
    void clear();
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
};

std::string double_to_string(double value);
std::string fixed_width_string(const std::string& text, size_t width);
