//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include <array>
#include <string>
#include <vector>

class Memory {
public:
    explicit Memory();
    void clear();

    // read/write .p file
    bool load_p_file(const std::string& filename);
    bool save_p_file(const std::string& filename);
    int end_of_prog() const {
        return end_of_prog_addr;
    }

    // read memory
    int peek(int addr) const;
    int dpeek(int addr) const;
    int speek(int addr) const;
    int dpeek_be(int addr) const;
    double fpeek(int addr) const;
    Bytes peek_bytes(int addr, int size) const;
    int get_line_addr(int line_num) const;

    // get video memory
    std::vector<std::string> get_video_lines() const;
    std::vector<std::string> get_trimmed_video_lines() const;
    bool is_video_collapsed() const;

    // get workspace bytes
    Bytes get_workspace_bytes() const;

    // get autostart
    bool get_autostart() const;
    int get_autostart_line_num() const;

    // get fast flag
    bool get_fast() const;

    // write memory
    void poke(int addr, int value);
    void dpoke(int addr, int value);
    void dpoke_be(int addr, int value);
    void fpoke(int addr, double value);
    int poke_bytes(int addr, const Bytes& bytes);
    int poke_bytes(int addr, const Byte* data, int size);

    // init video and e_line memory
    void init_video_to_stkend(int addr);
    void init_e_line_to_stkend(int addr);

private:
    std::array<Byte, MEM_SIZE> mem{ 0 };
    int end_of_prog_addr = 0;           // addr of end of .p file

    int write_empty_d_file(int addr);
    int write_empty_vars(int addr);
};
