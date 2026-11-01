//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include "zx81consts.h"
#include <array>
#include <string>
#include <vector>

class Memory {
public:
    explicit Memory();
    virtual ~Memory() = default;

    void clear();
    void init_basic();

    // read/write .p file
    void read_p_file(const std::string& filename);
    void write_p_file(const std::string& filename);

    // read memory
    int peek(int addr) const;
    int dpeek(int addr) const;
    int speek(int addr) const;
    int dpeek_be(int addr) const;
    double fpeek(int addr) const;
    Bytes peek_bytes(int addr, int size) const;
    std::string peek_hex(int addr, int len) const;
    int get_line_addr(int line_num) const;

    // write memory
    void poke(int addr, int value);
    void dpoke(int addr, int value);
    void dpoke_be(int addr, int value);
    void fpoke(int addr, double value);
    int poke_bytes(int addr, const Bytes& bytes);
    int poke_bytes(int addr, const Byte* data, int size);

    static Bytes get_empty_d_file();

    void init_video_to_stkend(int addr, bool dfile_collapsed,
                              const std::vector<std::string>& dfile_lines,
                              bool default_ch_add_stack);
    void init_e_line_to_stkend(int addr, bool default_ch_add_stack);

private:
    std::array<Byte, MEM_SIZE> mem{ 0 };

    int write_empty_d_file(int addr);
    int write_empty_vars(int addr);
};
