//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include "expr.h"
#include "types.h"
#include <iostream>
#include <string>
#include <vector>

struct AsmLine {
    SourceLoc loc;
    std::string label;
    Expr equ_expr;			// if equ
    Bytes bytes;
    std::vector<Patch> patches;

    explicit AsmLine(const SourceLoc& loc_)
        : loc(loc_), equ_expr(loc_) {}
};

struct SourceLine {
    enum class Type { Basic, Asm };
    Type type = Type::Basic;
    SourceLoc loc;
    int addr = -1;
    std::string label;
    int line_num = -1;
    std::vector<Token> tokens;
    AsmLine asm_line;       // for Asm

    SourceLine(Type type_, const SourceLoc& loc_)
        : type(type_), loc(loc_), asm_line(loc_) {}
};

struct BasicVar {
    enum class Type { Number, ArrayNumbers, ForNextLoop, String, ArrayStrings };
    Type type = Type::Number;
    SourceLoc loc;

    std::string name;       // excluding the '$' for strings

    // Number
    double value = 0.0;

    // ForNextLoop
    double limit = 0.0;
    double step = 0.0;
    int line_num = 0;

    // ArrayNumbers
    std::vector<int> dims;
    std::vector<double> values;

    // String
    std::string str;

    // ArrayStrings
    std::vector<std::string> strs;

    explicit BasicVar(Type type_, const SourceLoc& loc_)
        : type(type_), loc(loc_) {}
};

struct Basic {
    Bytes sysvars;
    std::vector<SourceLine> source_lines;
    std::vector<std::string> dfile_lines;
    std::vector<BasicVar> basic_vars;
    bool dfile_collapsed = false;
    bool autostart = false;
    int autostart_line = 0;
    int increment = 10;
    bool rem_invert = false;
    bool fast = false;

    void write_bas_file(const std::string& filename) const;

private:
    void write_sysvars(std::ostream& os) const;
    void write_dfile(std::ostream& os) const;
    void write_vars(std::ostream& os) const;
    void write_source_lines(std::ostream& os) const;
};
