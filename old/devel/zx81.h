//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#define NUM_ELEMS(a)	(sizeof(a) / sizeof(a[0]))

string decode_zx81(char c);
vector<uint8_t> encode_zx81(const char*& p);

struct BasicLine {
    int addr{ 0 };
    int line_num{ 0 };
    int size{ 0 };
    string label;
    vector<Token> tokens;
};

struct BasicVar {
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

class AsmLabels {
public:
    void add(const string& name, int value);

    bool find(const string& name, int& value) const;
    bool find(int value, string& name) const;

    void clear();

private:
    unordered_map<string, int> by_name;
    unordered_map<int, string> by_value;
};

struct AsmLine {
    enum class Type { Undef, Asm, Byte, Word };
    Type type{ Type::Undef };
    int addr{ 0 };
    int size{ 0 };

    string opcode;		// e.g. "ld a,N"
    string instr;		// e.g. "ld a,23"
    int op1{ 0 };
    int op2{ 0 };

    AsmLine(int addr_ = 0) : addr(addr_) {}
};

struct ZX81 {
    ZX81();
    virtual ~ZX81();
    ZX81(const ZX81& other) = delete;
    ZX81& operator=(const ZX81& other) = delete;

    // memory structure
    array<uint8_t, RAM_SIZE> ram{ 0 };
    vector<AsmLine*> asm_code;

    // BASIC structure
    vector<BasicLine> basic_lines;
    vector<BasicVar> basic_vars;
    vector<uint8_t> d_file_bytes;
    vector<uint8_t> e_line_bytes;

    // manipulate memory
    int peek(int addr) const;
    int speek(int addr) const;
    int dpeek(int addr) const;
    int dpeek_be(int addr) const;
    double fpeek(int addr) const;
    string str_peek(int addr, int len) const;
    string bytes_peek(int addr, int len) const;

    void poke(int addr, int value);
    void dpoke(int addr, int value);
    void dpoke_be(int addr, int value);
    void fpoke(int addr, double value);
    int str_poke(int addr, const string& str);
    template<class T>
    int bytes_poke(int addr, const T& bytes) {
        int len = static_cast<int>(bytes.size());
        for (auto& byte : bytes) {
            poke(addr++, byte);
        }
        return len;
    }

    int get_line_addr(int line_num);

    // compile/decompile BASIC
    void compile();
    void decompile();

    // read/write files
    void read_p_file(const string& filename);
    void write_p_file(const string& filename) const;

    void read_b81_file(const string& filename);
    void write_b81_file(const string& filename) const;

private:
    void init_ram();
    void init_video_to_stkend(int addr);
    void init_e_line_to_stkend(int addr);
    void init_labels();

    // decompile BASIC
    int addr{ 0 };
    int end{ 0 };
    int error_line_num{ 0 };
    int autostart{ 0 };
    void decompile_basic();
    void decompile_basic_line(BasicLine& line);
    bool decompile_rem_code(BasicLine& line);
    bool decompile_number(BasicLine& line);
    bool decompile_ident(BasicLine& line);
    bool decompile_string(BasicLine& line);
    void decompile_newline(BasicLine& line);
    void decompile_vars();

    // compile BASIC
    int auto_increment{ 10 };
    unordered_map<string, BasicLine*> basic_labels;
    void delete_empty_lines();
    void compute_line_numbers();
    void compile_basic(int pass);
    void compile_vars();
    void compile_number(vector<uint8_t>& bytes, double value);
    void compile_string(vector<uint8_t>& bytes, const string& str);
    void compile_ident(vector<uint8_t>& bytes, const string& ident);

    // write BASIC file
    void write_sysvars(ofstream& ofs) const;
    void write_basic_lines(ofstream& ofs) const;
    void write_video(ofstream& ofs) const;
    void write_basic_vars(ofstream& ofs) const;
    void write_basic_system(ofstream& ofs) const;

    // parse BASIC file
    bool in_asm{ false };
    void skip_spaces(const char*& p);
    bool match(const char*& p, const string& compare);
    bool parse_integer(const char*& p, int& value);
    bool parse_number(const char*& p, double& value, string& value_text);
    bool parse_string(const char*& p, string& str);
    bool parse_ident(const char*& p, string& ident);
    bool parse_label(const char*& p, string& ident);
    bool parse_line_num_ref(const char*& p, string& ident);
    bool parse_line_addr_ref(const char*& p, string& ident);
    bool parse_end(const char*& p);
    void parse_line(const char* p);
    void parse_meta_line(const char* p);
    void parse_basic_line(const char* p);
    void parse_basic_var(const char* p);

    // disassemble code
    AsmLabels asm_labels;
    AsmLine* get_asm_line(int addr) const;
    AsmLine* make_asm_line(int addr);
    AsmLine* disasm(int addr);
    AsmLine* disasm1(int addr);
    void disasm_cb(int& addr, AsmLine* line);
    void disasm_ed(int& addr, AsmLine* line);
    void disasm_x(int& addr, AsmLine* line, const string& x);
    void disasm_x_cb(int& addr, AsmLine* line, const string& x, int offset);
    void write_asm_lines(ofstream& ofs, int start_addr, int end_addr) const;

    // assemble code
    void parse_asm_line(const char* p);
    void parse_asm_defb(const char* p, int addr, vector<uint8_t>& bytes);
};
