//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "memory.h"
#include "symtab.h"
#include <string>

class Compiler {
public:
    explicit Compiler(Basic* basic_);
    virtual ~Compiler() = default;

    void compile();
    void write_p_file(const std::string& filename);

private:
    Basic* basic = nullptr;
    Symtab symtab;
    Memory mem;
    int addr = 0;
    int pass = 0;

    void fix_line_refs();
    void remove_empty_basic_lines();
    void compile_prog();
    void start_basic_line(int line_num);
    void end_basic_line(int line_addr);
    void compile_basic(SourceLine& line);
    void compile_number(double value, const std::string& value_str,
                        const SourceLoc& loc);
    void compile_string(const std::string& value, const SourceLoc& loc);
    void compile_ident(const std::string& ident, const SourceLoc& loc);
    void compile_line_addr_ref(const std::string& ident, const SourceLoc& loc);
    void compile_vars();
    void set_asmpc(int asmpc);

    void assemble(AsmLine& line);
};
