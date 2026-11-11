//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "ctl_file.h"
#include "disasm.h"
#include "memory.h"
#include <unordered_map>

class Decompiler {
public:
    explicit Decompiler(Memory* mem_, CtlFile* ctl_file_, Basic* basic_,
                        DisasmCode* disasm_code_)
        : mem(mem_), ctl_file(ctl_file_), basic(basic_), disasm_code(disasm_code_) {};
    virtual ~Decompiler() = default;

    void decompile();

private:
    Memory* mem = nullptr;
    CtlFile* ctl_file = nullptr;
    Basic* basic = nullptr;
    DisasmCode* disasm_code = nullptr;
    std::unordered_map<int, SourceLine*> source_lines;
    int addr = 0;
    int end = 0;

    void decompile_sysvars();
    void decompile_dfile();
    void decompile_vars();
    void decompile_basic();
    void decompile_basic_line(SourceLine& line);
    bool decompile_rem_code(SourceLine& line);
    bool decompile_number(SourceLine& line);
    bool decompile_ident(SourceLine& line);
    bool decompile_string(SourceLine& line);
    void decompile_newline(SourceLine& line);
    void init_ctl_mem();
    void map_source_lines();
    void analyse_ctl_file();
    void analyse_goto();
    void analyse_usr();
};
