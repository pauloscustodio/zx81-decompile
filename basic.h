//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "types.h"
#include "zx81consts.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

enum ZX81char {
    T_none = 0x100,
    T_integer,
    T_float,
    T_string,
    T_ident,
    T_rem_code,
    T_line_addr_ref,
    T_line_num_ref,
    T_const_expr,
    T_unary_minus,
    T_ASMPC,

#define X(code, name, str) name = code,
#include "zx81chars.def"
#undef X
};

struct Token {
    ZX81char code{ T_none };
    int		ivalue{ 0 };		// for T_integer
    double	fvalue{ 0.0 };		// for T_float
    std::string  svalue;				// for T_string
    std::string	ident;				// for T_ident, T_line_addr, T_line_num
    Bytes	bytes;				// for T_rem_code
    std::vector<Token> rpn;			// Expr for T_const_expr
};

typedef std::vector<Token> Expr;

struct Symbol {
    enum class Type { Const, Addr };
    Type type{ Type::Const };
    std::string name;
    int value;

    Symbol(Type type_ = Type::Const, const std::string& name_ = "", int value_ = 0);
};

class Symtab {
public:
    Symtab();
    virtual ~Symtab();
    Symtab(const Symtab& other) = delete;
    Symtab& operator=(const Symtab& other) = delete;

    void init();

    void add(Symbol::Type type, const std::string& name, int value);
    void update(Symbol::Type type, const std::string& name, int value);

    Symbol* get(const std::string& name);	// nullptr if not found
    Symbol* find(int value);			// last defined symbol with this value

    auto begin() {
        return symbols.begin();
    }
    auto end() {
        return symbols.end();
    }
    auto cbegin() const {
        return symbols.cbegin();
    }
    auto cend() const {
        return symbols.cend();
    }
    auto size() const {
        return symbols.size();
    }

private:
    std::unordered_map<std::string, Symbol*> symbols;
};

struct Patch {
    enum class Type { Byte, Word, SByte, JrOffset };
    Type type{ Type::Byte };
    int offset{ 0 };
    Expr rpn;

    Patch(Type type_ = Type::Byte) : type(type_) {}
};

struct AsmLine {
    std::string source_filename;
    int source_line_num;
    std::string label;
    Expr equ_rpn;			// if equ
    Bytes bytes;
    std::vector<Patch> patches;

    AsmLine();
};

struct BasicLine {
    std::string source_filename;
    int source_line_num;
    std::string label;
    int line_num{ -1 };
    std::vector<Token> tokens;

    BasicLine();
};

struct SourceLine {
    enum class Type { Basic, Asm };
    Type type{ Type::Basic };

    int addr{ 0 };
    BasicLine basic_line;
    AsmLine asm_line;

    SourceLine(Type type_ = Type::Basic) : type(type_) {}
};

struct BasicVar {
    enum class Type { Number, ArrayNumbers, ForNextLoop, String, ArrayStrings };
    Type type{ Type::Number };
    int addr{ 0 };
    int size{ 0 };

    // Number
    std::string name;
    double value{ 0.0 };

    // ForNextLoop
    double limit{ 0.0 };
    double step{ 0.0 };
    int line_num{ 0 };

    // ArrayNumbers
    std::vector<int> dimensions;
    std::vector<double> values;

    // String
    std::string str;

    // ArrayStrings
    std::vector<std::string> strs;
};

struct Basic {
    Basic();
    void clear();

    std::vector<SourceLine> source_lines;
    std::vector<BasicVar> basic_vars;
    Symtab symtab;
    Bytes d_file_bytes;
    Bytes e_line_bytes;
    int autostart;
    int auto_increment;
    bool fast;

    int eval_const_expr(const Expr& rpn);
    bool eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error = true);
    void parse_b81_file(const std::string& filename);
    void write_b81_file(const std::string& filename);

private:
    void write_sysvars(std::ofstream& ofs) const;
    void write_basic_lines(std::ofstream& ofs) const;
    void write_video(std::ofstream& ofs) const;
    void write_basic_vars(std::ofstream& ofs) const;
    void write_basic_system(std::ofstream& ofs) const;
    void write_mem_info(std::ofstream& ofs, int start_addr, int len) const;
    void write_basic_memory_map(std::ofstream& ofs) const;
};
