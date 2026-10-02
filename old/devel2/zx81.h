//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "consts.h"
#include "basic.h"
#include "scan.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

struct Token1 {
    int		code{ T_none };
    int		ivalue{ 0 };		// for T_integer
    double	fvalue{ 0.0 };		// for T_float
    string  str;				// for T_string
    string	ident;				// for T_ident, T_line_addr, T_line_num
    Bytes	bytes;				// for T_code
    vector<Token1> rpn;			// for T_const_expr
};

struct BasicLine1 {
    int addr{ 0 };
    int line_num{ -1 };
    int size{ 0 };
    string label;
    vector<Token1> tokens;
};

struct BasicVar1 {
    enum class Type { Number, ArrayNumbers, ForNextLoop, String, ArrayStrings };
    Type type{ Type::Number };
    int addr{ 0 };
    int size{ 0 };

    // Number
    string name;
    double value{ 0.0 };

    // ForNextLoop
    double limit{ 0.0 };
    double step{ 0.0 };
    int line_num{ 0 };

    // ArrayNumbers
    vector<int> dimensions;
    vector<double> values;

    // String
    string str;

    // ArrayStrings
    vector<string> strs;
};

class ZX81 {
public:
    ZX81();

    // read/write .b81 file
    void read_b81_file(const string& filename);
    void write_b81_file(const string& filename) const;

    // compile/decompile
    void compile();
    void decompile();

private:
    // BASIC
    vector<BasicLine1> basic_lines;
    vector<BasicVar1> basic_vars;
    Bytes d_file_bytes;
    Bytes e_line_bytes;
    int autostart{ 0 };
    int auto_increment{ 10 };
    bool fast{ false };

    // memory
    void init_video_to_stkend(int addr, Bytes& d_file_bytes, Bytes& e_line_bytes);
    void init_e_line_to_stkend(int addr, Bytes& e_line_bytes);

    // parse BASIC files
    const char* p{ nullptr };
    bool in_asm{ false };
    void skip_spaces();
    bool match(const string& compare);
    bool parse_integer(int& value);
    bool parse_number(double& value, string& value_text);
    bool parse_string(string& str);
    bool parse_ident(string& ident);
    bool parse_label(string& ident);
    bool parse_line_num_ref(string& ident);
    bool parse_line_addr_ref(string& ident);
    bool parse_end();
    void parse_line();
    void parse_meta_line();
    void parse_basic_line();
    void parse_basic_var();
    void parse_asm_line();

    // write BASIC lines
    void write_sysvars(ofstream& ofs) const;
    void write_basic_lines(ofstream& ofs) const;
    void write_video(ofstream& ofs) const;
    void write_basic_vars(ofstream& ofs) const;
    void write_basic_system(ofstream& ofs) const;
    void write_basic_memory_map(ofstream& ofs) const;

    // compile
    int addr{ 0 };
    int end{ 0 };
    int pass{ 0 };
    void compute_line_numbers();
    void delete_empty_lines();
    void compile_basic();
    void compile_vars();
    void compile_number(double value);
    void compile_string(const string& str);
    void compile_ident(const string& ident);

    // decompile
    void decompile_sysvars();
    void decompile_d_file();
    void decompile_e_line();
    void decompile_basic();
    void decompile_basic_line(BasicLine1& line);
    bool decompile_rem_code(BasicLine1& line);
    bool decompile_number(BasicLine1& line);
    bool decompile_ident(BasicLine1& line);
    bool decompile_string(BasicLine1& line);
    void decompile_newline(BasicLine1& line);
    void decompile_vars();

    // diasassemble
    string fmt_asm(int addr, const string& opcode, const string& comment) const;
    void write_mem_info(ofstream& ofs, int start_addr, int len) const;
    void disassemble();

    // assemble
    void assemble_line(const string& asm_line);
};

extern ZX81 g_zx81;
