//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "consts.h"
#include "errors.h"
#include "memory.h"
#include "utils.h"
#include "zx81encode.h"
#include "zx81float.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

Memory::Memory() {
    clear();
}

void Memory::clear() {
    int addr = PROG;
    dpoke(D_FILE, addr);
    dpoke(NXTLIN, addr);
    addr = write_empty_d_file(addr);
    dpoke(VARS, addr);
    addr = write_empty_vars(addr);
    dpoke(E_LINE, addr);
    dpoke(STKBOT, addr);
    dpoke(STKEND, addr);

    // print position
    dpoke(DF_CC, dpeek(D_FILE) + 1);
    poke(DF_SZ, 2);
    poke(S_POSNR, NumRows + 1);
    poke(S_POSNC, NumCols + 1);
    poke(PR_CC, PRBUFF >> 8);

    // system variables
    dpoke(MEM, MEMBOT);
    dpoke(LAST_K, 0xffff);
    poke(DEBOUNCE, 0xff);
    poke(MARGIN, 55);
    dpoke(FRAMES, 0xffff);
    poke(CDFLAG, FlagSlow);
    poke(PRBUFF + 32, C_newline);
}

bool Memory::load_p_file(const std::string& filename) {
    // open file
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        error("open file " + filename);
        return false;
    }

    // get size
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (size > MEM_SIZE - SAVE_ADDR) {
        error("file " + filename + " is too big");
        return false;
    }

    // read bytes
    ifs.read(reinterpret_cast<char*>(&mem[SAVE_ADDR]), size);
    size_t readed = ifs.gcount();
    if (readed != size) {
        perror(filename.c_str());
        error("read " + std::to_string(size) + " bytes from file " + filename);
        return false;
    }

    end_of_prog_addr = INT(SAVE_ADDR + size);
    return true;
}

bool Memory::save_p_file(const std::string& filename) {
    // open file
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        error("open file " + filename);
        return false;
    }

    size_t size = dpeek(E_LINE) - SAVE_ADDR;
    ofs.write(reinterpret_cast<const char*>(&mem[SAVE_ADDR]), size);
    size_t written = ofs.tellp();
    if (written != size) {
        perror(filename.c_str());
        error("write " + std::to_string(size) + " bytes to file " + filename);
        return false;
    }
    return true;
}

int Memory::peek(int addr) const {
    return mem[addr & 0xffff];
}

int Memory::dpeek(int addr) const {
    return peek(addr) | (peek(addr + 1) << 8);
}

int Memory::speek(int addr) const {
    int n = peek(addr);
    if (n >= 0x80) {
        n -= 0x100;
    }
    return n;
}

int Memory::dpeek_be(int addr) const {
    return (peek(addr) << 8) + peek(addr + 1);
}

double Memory::fpeek(int addr) const {
    std::array<Byte, 5> bytes{ 0 };
    for (int i = 0; i < 5; i++) {
        bytes[i] = peek(addr + i);
    }
    return zx81_to_float(bytes);
}

Bytes Memory::peek_bytes(int addr, int size) const {
    Bytes out;
    out.insert(out.end(), mem.begin() + addr, mem.begin() + addr + size);
    return out;
}

int Memory::get_line_addr(int line_num) const {
    int addr = PROG;
    int d_file = dpeek(D_FILE);
    while (addr < d_file) {
        int cur_line = dpeek_be(addr);
        if (cur_line >= line_num) {
            return addr;
        }
        int size = dpeek(addr + 2);
        addr += 4 + size;
    }
    return d_file;
}

std::vector<std::string> Memory::get_video_lines() const {
    std::vector<std::string> lines;
    for (int row = 0; row < NumRows; row++) {
        std::string line;
        for (int col = 0; col < NumCols + 1; col++) {
            int addr = dpeek(D_FILE) + 1 + row * (NumCols + 1) + col;
            int c = peek(addr);
            if (c == C_newline) {
                break;
            }
            else if (c == C_dquote) {
                line += fmt_hex(c, 2, "\\");
            }
            else {
                line += decode_zx81(c);
            }
        }
        lines.push_back(std::move(line));
    }
    return lines;
}

std::vector<std::string> Memory::get_trimmed_video_lines() const {
    auto lines = get_video_lines();
    for (auto& line : lines) {
        line = str_trim(line);
    }
    return lines;
}

bool Memory::is_video_collapsed() const {
    int video_size = dpeek(VARS) - dpeek(D_FILE);
    return video_size < 1 + NumRows * (1 + NumCols);
}

Bytes Memory::get_workspace_bytes() const {
    return peek_bytes(dpeek(E_LINE), end_of_prog_addr - dpeek(E_LINE));
}

// get autostart
bool Memory::get_autostart() const {
    return dpeek(NXTLIN) < dpeek(D_FILE);
}

int Memory::get_autostart_line_num() const {
    if (!get_autostart()) {
        return 0;
    }
    else {
        int nxtlin = dpeek(NXTLIN);
        int line_num = dpeek_be(nxtlin);
        return line_num;
    }
}

bool Memory::get_fast() const {
    if ((peek(CDFLAG) & FlagSlow) == FlagSlow) {
        return false;
    }
    else {
        return true;
    }
}

void Memory::poke(int addr, int value) {
    mem[addr & 0xffff] = value & 0xff;
}

void Memory::dpoke(int addr, int value) {
    poke(addr, value);
    poke(addr + 1, value >> 8);
}

void Memory::dpoke_be(int addr, int value) {
    poke(addr, value >> 8);
    poke(addr + 1, value);
}

void Memory::fpoke(int addr, double value) {
    std::array<Byte, 5> bytes = float_to_zx81(value);
    for (int i = 0; i < 5; i++) {
        poke(addr + i, bytes[i]);
    }
}

int Memory::poke_bytes(int addr, const Bytes& bytes) {
    return poke_bytes(addr, bytes.data(), INT(bytes.size()));
}

int Memory::poke_bytes(int addr, const Byte* data, int size) {
    while (size-- > 0) {
        poke(addr++, *(data++));
    }
    return addr;
}

void Memory::init_video_to_stkend(int addr) {
    dpoke(D_FILE, addr);
    dpoke(NXTLIN, addr);
    dpoke(DF_CC, addr + 1);

    // init video memory
    addr = write_empty_d_file(addr);

    // vars
    dpoke(VARS, addr);
    addr = write_empty_vars(addr);

    init_e_line_to_stkend(addr);
}

void Memory::init_e_line_to_stkend(int addr) {
    // edit line
    dpoke(E_LINE, addr);

    // calculator stack
    dpoke(STKBOT, addr);
    dpoke(STKEND, addr);
}

int Memory::write_empty_d_file(int addr) {
    poke(addr++, C_newline);
    for (int row = 0; row < NumRows; row++) {
        for (int col = 0; col < NumCols; col++) {
            poke(addr++, C_space);
        }
        poke(addr++, C_newline);
    }
    return addr;
}

int Memory::write_empty_vars(int addr) {
    poke(addr++, 0x80);
    return addr;
}

