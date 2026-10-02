//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "lexer.h"
#include "model.h"
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>

void Memory::clear() {
    data.clear();
    data_type.clear();
}

uint8_t Memory::peek(uint addr) const {
    if (addr < BASE_ADDR || addr >= BASE_ADDR + data.size()) {
        return 0;
    }
    return data[addr - BASE_ADDR];
}

uint16_t Memory::peek_word(uint addr) const {
    if (addr < BASE_ADDR || addr + 1 >= BASE_ADDR + data.size()) {
        return 0;
    }
    return data[addr - BASE_ADDR] | (data[addr - BASE_ADDR + 1] << 8);
}

uint16_t Memory::peek_word_be(uint addr) const {
    if (addr < BASE_ADDR || addr + 1 >= BASE_ADDR + data.size()) {
        return 0;
    }
    return (data[addr - BASE_ADDR] << 8) | data[addr - BASE_ADDR + 1];
}

void Memory::poke(uint addr, uint8_t value) {
    if (addr < BASE_ADDR) {
        return;
    }
    if (addr >= BASE_ADDR + data.size()) {
        data.resize(addr - BASE_ADDR + 1);
    }
    data[addr - BASE_ADDR] = value;
}

void Memory::poke_word(uint addr, uint16_t value) {
    if (addr < BASE_ADDR) {
        return;
    }
    if (addr + 1 >= BASE_ADDR + data.size()) {
        data.resize(addr - BASE_ADDR + 2);
    }
    data[addr - BASE_ADDR] = value & 0xFF;
    data[addr - BASE_ADDR + 1] = (value >> 8) & 0xFF;
}

void Memory::poke_word_be(uint addr, uint16_t value) {
    if (addr < BASE_ADDR) {
        return;
    }
    if (addr + 1 >= BASE_ADDR + data.size()) {
        data.resize(addr - BASE_ADDR + 2);
    }
    data[addr - BASE_ADDR] = (value >> 8) & 0xFF;
    data[addr - BASE_ADDR + 1] = value & 0xFF;
}

MemoryType Memory::get_type(uint addr) const {
    if (addr < BASE_ADDR || addr >= BASE_ADDR + data_type.size()) {
        return MemoryType::Unknown;
    }
    return data_type[addr - BASE_ADDR];
}

void Memory::set_type(MemoryType type, uint addr) {
    if (addr < BASE_ADDR) {
        return;
    }
    if (addr >= BASE_ADDR + data_type.size()) {
        data_type.resize(addr - BASE_ADDR + 1, MemoryType::Unknown);
    }
    data_type[addr - BASE_ADDR] = type;
}

void Memory::set_type(MemoryType type, uint addr, uint size) {
    for (uint i = 0; i < size; ++i) {
        set_type(type, addr + i);
    }
}

void Memory::load_file(const std::string& filename, uint addr) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        fatal("Failed to open file: " + filename);
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    end_of_prog = addr + file_size;

    std::vector<uint8_t> buffer(file_size);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);

    for (uint i = 0; i < file_size; ++i) {
        poke(addr + i, buffer[i]);
    }
}

bool Line::empty() const {
    return line_num == -1 && label.empty() && tokens.empty();
}

void Line::clear() {
    type = SourceType::BASIC;
    line_num = -1;
    label.clear();
    tokens.clear();
    start_addr = 0;
    asm_length = 0;
}

// 10-digit precision to match 32-bit mantissa
std::string double_to_string(double value) {
    std::ostringstream oss;
    if (floor(value) == value) {
        oss << value;
    }
    else {
        oss << std::setprecision(10) << value;
    }
    return oss.str();
}

std::string fixed_width_string(const std::string& text, size_t width) {
    if (text.length() < width) {
        return text + std::string(width - text.length(), ' ');
    }
    else {
        return text.substr(0, width);
    }
}
