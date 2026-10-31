//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include "errors.h"
#include "scan.h"
#include "types.h"
#include <string>

class Parser {
public:
    explicit Parser(Basic* basic_)
        : basic(basic_) {};

    void parse_bas_file(const std::string& filename);

private:
    Basic* basic = nullptr;
    bool in_asm = false;
    Scanner scan;
    SourceLoc loc;
    int last_line_num = -1;

    // parser
    void parse_line();

    // meta parser
    void parse_pragma_line();
    bool parse_basic_var(BasicVar& var);
    Bytes parse_byte_list();

    // basic parser
    bool parse_basic_line(SourceLine& source_line);
    bool parse_ref(const std::string& prefix, std::string& ident);
    bool parse_line_num_ref(std::string& ident);
    bool parse_line_addr_ref(std::string& ident);
    bool parse_label(std::string& ident);
    void parse_label_line_num(SourceLine& source_line);

    // assembler parser
    bool parse_asm_line(SourceLine& source_line);

};
