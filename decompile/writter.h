//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "basic_info.h"
#include "disasm.h"
#include "memory.h"
#include <fstream>
#include <string>

class BasWriter {
public:
    explicit BasWriter(Memory* mem_, Basic* basic_, BasicInfo* basic_info_,
                       DisasmCode* disasm_)
        : mem(mem_), basic(basic_), basic_info(basic_info_), disasm(disasm_) {}
    virtual ~BasWriter() {
        ofs.close();
    }
    BasWriter(const BasWriter& other) = delete;
    BasWriter& operator=(const BasWriter& other) = delete;

    bool write_bas_file(const std::string& filename);

private:
    Memory* mem = nullptr;
    Basic* basic = nullptr;
    BasicInfo* basic_info = nullptr;
    DisasmCode* disasm = nullptr;
    bool wrote_equs = false;
    std::ofstream ofs;

    void write_sysvars();
    void write_basic_lines();
    void write_disassembly(int start_addr, int size);
    void write_opcode(Opcode* opcode);
    std::string decode_undef(Opcode* opcode);
    std::string decode_opcode(Opcode* opcode);
    std::string decode_defb(Opcode* opcode);
    std::string decode_defw(Opcode* opcode);
    std::string decode_defm(Opcode* opcode);
    std::string decode_label(int addr);
    void write_video();
    void write_basic_vars();
    void write_basic_memory_map();
};