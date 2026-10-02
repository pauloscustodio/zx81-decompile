//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "basic_info.h"
#include "ctl_file.h"
#include "disasm.h"
#include "memory.h"
#include <vector>

class Decompiler {
public:
    explicit Decompiler(Memory* mem_, Basic* basic_, CtlFile* ctl_file_,
                        DisasmCode* disasm_);
    virtual ~Decompiler();
    Decompiler(const Decompiler&& other) = delete;
    Decompiler& operator=(const Decompiler&& other) = delete;

    Decompiler(const Decompiler& other) = delete;
    Decompiler& operator=(const Decompiler& other) = delete;

    void decompile();
    BasicInfo* get_basic_info();

private:
    Memory* mem = nullptr;
    Basic* basic = nullptr;
    CtlFile* ctl_file = nullptr;
    DisasmCode* disasm = nullptr;
    BasicInfo* basic_info = nullptr;
    int addr = 0;
    int end = 0;

    void decompile_sysvars();
    void decompile_basic();
    void decompile_basic_line(BasicLine& basic_line);
    bool decompile_rem_code(BasicLine& basic_line);
    bool decompile_number(BasicLine& basic_line);
    bool decompile_ident(BasicLine& basic_line);
    bool decompile_string(BasicLine& basic_line);
    void decompile_newline(BasicLine& basic_line);
    void decompile_vars();
    void disassemble();
    void find_gotos();
    void find_usrs();
    bool find_function_arg(const std::vector<Token>& tokens, size_t& pos,
                           const std::vector<ZX81char>& functions, int& arg);
};
