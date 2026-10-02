//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disasm.h"
#include "encode.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "parser.h"
#include "utils.h"
#include "zfloat.h"
#include "zx81.h"
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

//-----------------------------------------------------------------------------
// virtual machine
//-----------------------------------------------------------------------------

ZX81 g_zx81;

ZX81::ZX81() {
    // BASIC system
    d_file_bytes = Memory::get_empty_d_file();
    e_line_bytes = Memory::get_empty_e_line();
}

void ZX81::init_video_to_stkend(int addr, Bytes& d_file_bytes,
                                Bytes& e_line_bytes) {
    dpoke(D_FILE, addr);
    dpoke(NXTLIN, addr);
    addr = poke_bytes(addr, d_file_bytes);

    // vars
    dpoke(VARS, addr);
    poke(addr++, 0x80);

    init_e_line_to_stkend(addr, e_line_bytes);
}

void ZX81::init_e_line_to_stkend(int addr, Bytes& e_line_bytes) {
    // edit line
    dpoke(E_LINE, addr);
    addr = poke_bytes(addr, e_line_bytes);

    // calculator stack
    dpoke(STKBOT, addr);
    dpoke(STKEND, addr);
}

//-----------------------------------------------------------------------------
// read BASIC files
//-----------------------------------------------------------------------------

void ZX81::read_b81_file(const string& filename) {
    basic_lines.clear();
    basic_vars.clear();
    in_asm = false;

    ifstream ifs(filename);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        fatal_error("read file", filename);
    }

    err_set_filename(filename);

    string text;
    int line_num = 0;
    while (getline(ifs, text)) {
        line_num++;
        err_set_line_num(line_num);
        while (!text.empty() && text.back() == '\\') {
            text.pop_back();
            text.push_back(' ');
            string cont;
            if (!getline(ifs, cont)) {
                break;
            }
            line_num++;
            err_set_line_num(line_num);
            text += cont;
        }
        p = text.c_str();
        parse_line();
    }

    err_clear();
}

//-----------------------------------------------------------------------------
// parse BASIC files
//-----------------------------------------------------------------------------

void ZX81::skip_spaces() {
    while (*p != '\0' && isspace(*p)) {
        p++;
    }
}

bool ZX81::match(const string& compare) {
    skip_spaces();
    for (size_t i = 0; i < compare.size(); i++) {
        if (p[i] == '\0') {
            return false;
        }
        if (toupper(p[i]) != toupper(compare[i])) {
            return false;
        }
    }
    p += compare.size();
    return true;
}

bool ZX81::parse_integer(int& value) {
    skip_spaces();
    const char* p0 = p;
    if (*p == '$') {	// hex
        if (!isxdigit(p[1])) {
            return false;
        }
        p++;
        while (isxdigit(*p)) {
            p++;
        }
        value = INT(strtol(p0 + 1, NULL, 16));
        return true;
    }
    else {				// decimal
        if (!isdigit(*p)) {
            return false;
        }
        while (isdigit(*p)) {
            p++;
        }
        value = atoi(p0);
        return true;
    }
}

bool ZX81::parse_number(double& value, string& value_text) {
    skip_spaces();
    const char* p0 = p;

    // collect mantissa
    int num_dots = 0;
    int num_digits = 0;
    while (*p != '\0') {
        if (*p == '.') {
            p++;
            num_dots++;
            if (num_dots > 1) {
                p = p0;
                return false;
            }
        }
        else if (isdigit(*p)) {
            p++;
            num_digits++;
        }
        else {
            break;
        }
    }
    if (num_digits == 0) {
        p = p0;
        return false;
    }

    // collect exponent
    if (toupper(*p) == 'E') {
        p++;
        if (*p == '-' || *p == '+') {
            p++;
        }
        int exp = 0;
        if (!parse_integer(exp)) {
            p = p0;
            return false;
        }
    }

    value = atof(p0);
    value_text = string(p0, p);
    return true;
}

bool ZX81::parse_string(string& str) {
    string out;
    skip_spaces();
    const char* p0 = p;

    if (*p != '"') {
        return false;
    }
    p++;
    while (*p != '\0' && *p != '"') {
        if (*p == '\\') {
            out.push_back(*p++);
            out.push_back(*p++);
        }
        else {
            out.push_back(*p++);
        }
    }
    if (*p != '"') {
        p = p0;
        return false;
    }
    else {
        p++;
        str = out;
        return true;
    }
}

bool ZX81::parse_ident(string& ident) {
    skip_spaces();
    if (!isalpha(*p)) {
        return false;
    }
    while (isalnum(*p)) {
        ident.push_back(*p++);
    }
    return true;
}

bool ZX81::parse_label(string& ident) {
    skip_spaces();
    const char* p0 = p;
    if (!parse_line_num_ref(ident)) {
        p = p0;
        return false;
    }
    skip_spaces();
    if (*p != ':') {
        p = p0;
        return false;
    }
    p++;
    return true;
}

bool ZX81::parse_line_num_ref(string& ident) {
    skip_spaces();
    const char* p0 = p;
    if (*p != '@') {
        return false;
    }
    p++;
    if (!parse_ident(ident)) {
        p = p0;
        return false;
    }
    return true;
}

bool ZX81::parse_line_addr_ref(string& ident) {
    skip_spaces();
    const char* p0 = p;
    if (*p != '&') {
        return false;
    }
    p++;
    if (!parse_ident(ident)) {
        p = p0;
        return false;
    }
    return true;
}

bool ZX81::parse_end() {
    skip_spaces();
    if (*p == '\0' || *p == '#') {
        return true;
    }
    else {
        return false;
    }
}

void ZX81::parse_line() {
    const char* p0 = p;
    skip_spaces();
    if (*p == '\0') {
    }
    else if (*p == '#') {
        p++;
        parse_meta_line();
    }
    else if (in_asm) {
        p = p0;
        parse_asm_line();
    }
    else {
        parse_basic_line();
    }
}

void ZX81::parse_meta_line() {
    int n = 0;
    if (match("VARS")) {
        parse_basic_var();
    }
    else if (match("SYSVARS") && match("=")) {
        poke_bytes(VERSN, encode_zx81(p));
    }
    else if (match("D_FILE") && match("=")) {
        d_file_bytes = encode_zx81(p);
    }
    else if (match("WORKSPACE") && match("=")) {
        e_line_bytes = encode_zx81(p);
    }
    else if (match("AUTOSTART") && match("=") && parse_integer(n) && parse_end()) {
        autostart = n;
    }
    else if (match("FAST") && match("=") && parse_integer(n) && parse_end()) {
        fast = n ? true : false;
    }
    else if (match("INCREMENT") && match("=") && parse_integer(n) && parse_end()) {
        auto_increment = n;
    }
    else if (match("ASM")) {
        in_asm = true;
    }
    else if (match("ENDASM")) {
        in_asm = false;
    }
    else {
        // ignore, consider a comment
    }
}

void ZX81::parse_basic_line() {
    /*
    BasicParser parser;
    BasicLine1 line = parser.parse(p);
    basic_lines.push_back(line);
    */
}

void ZX81::parse_basic_var() {
    BasicVar1 var;
    bool is_string = false;
    bool is_array = false;
    int num_elements = 0;

    // get name
    if (!parse_ident(var.name)) {
        goto error;
    }

    // get string marker
    if (match("$")) {
        is_string = true;
        if (var.name.size() > 1) {
            error("name too long", var.name);
            return;
        }
    }

    // get array marker and dimensions
    if (match("(")) {
        is_array = true;
        if (var.name.size() > 1) {
            error("name too long", var.name);
            return;
        }

        num_elements = 1;
        do {
            int dimension = 0;
            if (!parse_integer(dimension)) {
                goto error;
            }
            num_elements *= dimension;
            var.dimensions.push_back(dimension);
        }
        while (match(","));

        if (!match(")")) {
            goto error;
        }
    }

    // get =
    if (!match("=")) {
        goto error;
    }

    // get value
    if (is_string == false && is_array == false) {
        string str;

        var.type = BasicVar1::Type::Number;
        if (!parse_number(var.value, str)) {
            goto error;
        }

        if (match(",")) {
            var.type = BasicVar1::Type::ForNextLoop;
            if (var.name.size() > 1) {
                error("name too long", var.name);
                return;
            }

            if (!parse_number(var.limit, str)) {
                goto error;
            }
            if (!match(",")) {
                goto error;
            }
            if (!parse_number(var.step, str)) {
                goto error;
            }
            if (!match(",")) {
                goto error;
            }
            if (!parse_integer(var.line_num)) {
                goto error;
            }
            if (!parse_end()) {
                goto error;
            }
        }
    }
    else if (is_string == false && is_array == true) {
        double value = 0.0;
        string str;

        var.type = BasicVar1::Type::ArrayNumbers;

        for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
            if (i > 0) {
                if (!match(",")) {
                    goto error;
                }
            }
            if (!parse_number(value, str)) {
                goto error;
            }
            var.values.push_back(value);
        }
        if (!parse_end()) {
            goto error;
        }
    }
    else if (is_string == true && is_array == false) {
        var.type = BasicVar1::Type::String;

        if (!parse_string(var.str)) {
            goto error;
        }
        if (!parse_end()) {
            goto error;
        }
    }
    else if (is_string == true && is_array == true) {
        string str;

        var.type = BasicVar1::Type::ArrayStrings;

        int last_dimension = var.dimensions.back();
        num_elements /= last_dimension;

        for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
            if (i > 0) {
                if (!match(",")) {
                    goto error;
                }
            }
            if (!parse_string(str)) {
                goto error;
            }
            if (str.size() != static_cast<size_t>(last_dimension)) {
                error("string length should be " + to_string(last_dimension));
                return;
            }
            var.strs.push_back(str);
        }
        if (!parse_end()) {
            goto error;
        }
    }

    basic_vars.push_back(var);
    return;

error:
    error("cannot parse", p);
}

void ZX81::parse_asm_line() {
    BasicLine1* last_line = &basic_lines.back();
    Token1& last_rem_code = last_line->tokens[1];
    if (last_rem_code.code != T_rem_code) {
        error("asm line without previous REM");
    }
    /*
    else
    	last_rem_code.asm_lines.push_back(p);
    */
}


//-----------------------------------------------------------------------------
// write BASIC files
//-----------------------------------------------------------------------------

void ZX81::write_b81_file(const string& filename) const {
    ofstream ofs(filename);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        fatal_error("write file", filename);
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        write_sysvars(ofs);
    }
    write_basic_lines(ofs);
    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        write_video(ofs);
    }
    write_basic_vars(ofs);
    write_basic_system(ofs);
    write_basic_memory_map(ofs);
}







//-----------------------------------------------------------------------------
// compile
//-----------------------------------------------------------------------------

void ZX81::compile() {
    delete_empty_lines();
    compute_line_numbers();
    g_asm_labels.clear();

    int last_end_addr = 0;
    pass = 1;
    int num_passes_ok = 0;
    while (true) {
        compile_basic();

        // next pass
        if (pass > 1 && addr == last_end_addr) {
            num_passes_ok++;
            if (num_passes_ok >= 2) {	// must run a second pass after end addr is ok
                break;
            }
        }
        last_end_addr = addr;
        pass++;
    }

    compile_vars();
}

void ZX81::compute_line_numbers() {
    int line_num = auto_increment;
    int last_line = -1;
    for (auto& line : basic_lines) {
        if (line.line_num < 0) {
            line.line_num = line_num;
            line_num += auto_increment;
        }
        else {
            line_num = line.line_num + auto_increment;
        }

        if (line.line_num <= last_line) {
            error("line " + to_string(line.line_num) + " follows line " + to_string(
                      last_line));
        }
        else if (line.line_num > MaxLineNum) {
            error("line " + to_string(line.line_num) + " above maximum of " + to_string(
                      MaxLineNum));
        }
    }
}

void ZX81::delete_empty_lines() {
    if (!basic_lines.empty()) {
        for (size_t i = 0; i < basic_lines.size() - 1; i++) {
            if (basic_lines[i].tokens.size() == 1
                    && basic_lines[i].tokens[0].code == C_newline) {
                if (!basic_lines[i].label.empty() && !basic_lines[i + 1].label.empty()) {
                    error("two labels on same line: " + basic_lines[i].label + " and " +
                          basic_lines[i + 1].label);
                }
                basic_lines[i + 1].label = basic_lines[i].label;
                basic_lines[i + 1].line_num = basic_lines[i].line_num;
                basic_lines.erase(basic_lines.begin() + i);
                i--;
            }
        }
    }
}

void ZX81::compile_basic() {
    addr = PROG;

    for (auto& line : basic_lines) {
        int line_addr = addr;
        dpoke(addr, 0);
        addr += 2;		// placeholder for line number
        dpoke(addr, 0);
        addr += 2;		// placeholder for size

        // define label
        if (pass == 1 && !line.label.empty()) {
            g_asm_labels.add(line.label, line_addr);
        }

        for (auto& token : line.tokens) {
            switch (token.code) {
            case T_none:
                break;
            case T_float:
                compile_number(token.fvalue);
                break;
            case T_string:
                compile_string(token.str);
                break;
            case T_ident:
                compile_ident(token.ident);
                break;
            case T_rem_code:
                for (auto& c : token.bytes) {
                    poke(addr++, c);
                }
                /*
                for (auto& asm_line : token.asm_lines)
                	assemble_line(asm_line);
                */
                break;
            case T_line_num_ref:
                if (pass == 1) {
                    compile_number(0);
                }
                else {
                    compile_number(dpeek_be(g_asm_labels.get(token.ident)));
                }
                break;
            case T_line_addr_ref:
                if (pass == 1) {
                    compile_number(0);
                }
                else {
                    compile_number(g_asm_labels.get(token.ident));
                }
                break;
            case T_const_expr:
                if (pass == 1) {
                    compile_number(0);
                }
                else {
                    vector<int> stack;
                    for (auto& optoken : token.rpn) {
                        int value = 0;
                        switch (optoken.code) {
                        case T_integer:
                            stack.push_back(optoken.ivalue);
                            break;
                        case T_line_num_ref:
                            stack.push_back(dpeek_be(g_asm_labels.get(optoken.ident)));
                            break;
                        case T_line_addr_ref:
                            stack.push_back(g_asm_labels.get(token.ident));
                            break;
                        case T_unary_minus:
                            stack.back() = -stack.back();
                            break;
                        case C_plus:
                            value = stack.back();
                            stack.pop_back();
                            stack.back() += value;
                            break;
                        case C_minus:
                            value = stack.back();
                            stack.pop_back();
                            stack.back() -= value;
                            break;
                        case C_mult:
                            value = stack.back();
                            stack.pop_back();
                            stack.back() *= value;
                            break;
                        case C_div:
                            value = stack.back();
                            stack.pop_back();
                            stack.back() /= value;
                            break;
                        default:
                            assert(0);
                        }
                    }
                    compile_number(stack.back());
                }
                break;
            default:
                assert(token.code < 0x100);
                poke(addr++, token.code);
            }
        }

        // define addr and size
        line.addr = line_addr;
        line.size = addr - line_addr - 4;

        dpoke_be(line_addr, line.line_num);
        dpoke(line_addr + 2, line.size);
    }

    init_video_to_stkend(addr, d_file_bytes, e_line_bytes);

    // autostart
    if (autostart) {
        int line_addr = get_line_addr(autostart);
        dpoke(NXTLIN, line_addr);
    }
    else {
        dpoke(NXTLIN, dpeek(D_FILE));
    }

    // fast
    if (fast) {
        poke(CDFLAG, peek(CDFLAG) & ~FlagSlow);
    }
    else {
        poke(CDFLAG, peek(CDFLAG) | FlagSlow);
    }
}

void ZX81::compile_vars() {
    addr = dpeek(VARS);

    Bytes str_bytes;
    array<Byte, 5> fp_bytes{ 0 };
    int last_dimension = 0;
    int size = 0;

    for (auto& var : basic_vars) {
        Bytes name_bytes = encode_zx81(var.name);
        assert(name_bytes.size() > 0);

        switch (var.type) {
        case BasicVar1::Type::Number:
            // put name
            if (var.name.size() == 1) {
                poke(addr++, (name_bytes[0] & 0x3f) | 0x60);			// 011-letter
            }
            else {
                poke(addr++, (name_bytes[0] & 0x3f) | 0xa0);			// 101-letter
                for (size_t i = 1; i < name_bytes.size() - 1; i++) {
                    poke(addr++, name_bytes[i] & 0x3f);    // 001-letter
                }
                poke(addr++, (name_bytes.back() & 0x3f) | 0x80);		// 101-letter
            }

            // put value
            fp_bytes = float_to_zx81(var.value);
            addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
            break;

        case BasicVar1::Type::ArrayNumbers:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0x80);				// 100-letter

            // put dimensions
            size = 1
                   + 2 * INT(var.dimensions.size())
                   + 5 * INT(var.values.size());
            dpoke(addr, size);
            addr += 2;
            poke(addr++, var.dimensions.size() & 0xff);

            for (auto& dimension : var.dimensions) {
                dpoke(addr, dimension);
                addr += 2;
            }

            // put values
            for (auto& value : var.values) {
                fp_bytes = float_to_zx81(value);
                addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));
            }
            break;

        case BasicVar1::Type::ForNextLoop:
            // put name
            poke(addr++, (name_bytes[0] & 0x3f) | 0xe0);				// 111-letter

            // put values
            fp_bytes = float_to_zx81(var.value);
            addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            fp_bytes = float_to_zx81(var.limit);
            addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            fp_bytes = float_to_zx81(var.step);
            addr = poke_bytes(addr, fp_bytes.data(), INT(fp_bytes.size()));

            dpoke(addr, var.line_num);
            addr += 2;
            break;

        case BasicVar1::Type::String:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0x40);				// 010-letter

            dpoke(addr, INT(var.str.size()));
            addr += 2;

            str_bytes = encode_zx81(var.str);
            addr = poke_bytes(addr, str_bytes);
            break;

        case BasicVar1::Type::ArrayStrings:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0xc0);				// 110-letter

            // put dimensions
            last_dimension = var.dimensions.back();
            size = 1
                   + 2 * INT(var.dimensions.size())
                   + last_dimension * INT(var.strs.size());
            dpoke(addr, size);
            addr += 2;
            poke(addr++, INT(var.dimensions.size()));

            for (auto& dimension : var.dimensions) {
                dpoke(addr, dimension);
                addr += 2;
            }

            // put strings
            for (auto& str : var.strs) {
                str_bytes = encode_zx81(str);
                addr = poke_bytes(addr, str_bytes);
            }
            break;

        default:
            assert(0);
        }
    }

    poke(addr++, 0x80);
    init_e_line_to_stkend(addr, e_line_bytes);
}

void ZX81::compile_number(double value) {
    ostringstream oss;

    // encode digits
    oss << value;
    string value_str = oss.str();
    Bytes str_bytes = encode_zx81(value_str);
    for (auto& c : str_bytes) {
        poke(addr++, c);
    }

    // encode number marker
    poke(addr++, C_number);

    // encode fp number
    array<Byte, 5> fp_bytes = float_to_zx81(value);
    for (auto& c : fp_bytes) {
        poke(addr++, c);
    }
}

void ZX81::compile_string(const string& str) {
    Bytes str_bytes = encode_zx81(str);
    poke(addr++, C_dquote);
    for (auto& c : str_bytes) {
        poke(addr++, c);
    }
    poke(addr++, C_dquote);
}

void ZX81::compile_ident(const string& ident) {
    Bytes str_bytes = encode_zx81(ident);
    for (auto& c : str_bytes) {
        poke(addr++, c);
    }
}

//-----------------------------------------------------------------------------
// decompile
//-----------------------------------------------------------------------------

void ZX81::decompile() {
    decompile_sysvars();
    decompile_d_file();
    decompile_e_line();
    decompile_basic();
    decompile_vars();
    disassemble();
}

void ZX81::decompile_sysvars() {
    // autostart
    int d_file = dpeek(D_FILE);
    int nxtlin = dpeek(NXTLIN);
    if (nxtlin >= d_file) {
        autostart = 0;
    }
    else {
        autostart = dpeek_be(nxtlin);
    }

    // fast
    if ((peek(CDFLAG) & FlagSlow) == FlagSlow) {
        fast = false;
    }
    else {
        fast = true;
    }
}

void ZX81::decompile_d_file() {
    int d_file = dpeek(D_FILE);
    int vars = dpeek(VARS);
    d_file_bytes = peek_bytes(d_file, vars - d_file);
}

void ZX81::decompile_e_line() {
    int e_line = dpeek(E_LINE);
    int stkbot = dpeek(STKBOT);
    e_line_bytes = peek_bytes(e_line, stkbot - e_line);
}

void ZX81::decompile_basic() {
    basic_lines.clear();
    addr = PROG;
    while (addr < dpeek(D_FILE)) {
        BasicLine1 line;
        line.addr = addr;
        line.line_num = dpeek_be(addr);
        line.size = dpeek(addr + 2);

        addr += 4;
        end = addr + line.size;

        decompile_basic_line(line);
        basic_lines.push_back(line);
    }
}

void ZX81::decompile_basic_line(BasicLine1& line) {
    if (decompile_rem_code(line)) {
    }
    else {
        while (addr < end - 1) {
            if (decompile_number(line)) {
            }
            else if (decompile_ident(line)) {
            }
            else if (decompile_string(line)) {
            }
            else {
                Token1 token;
                token.code = peek(addr++);
                line.tokens.push_back(token);
            }
        }
        decompile_newline(line);
    }
}

bool ZX81::decompile_rem_code(BasicLine1& line) {
    if (peek(addr) == C_REM) {
        bool is_code = false;
        for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
            if ((peek(p) & 0x40) == 0x40) {			// special char
                is_code = true;
                break;
            }
        }
        if (is_code) {
            Token1 rem;
            rem.code = C_REM;
            line.tokens.push_back(rem);

            Token1 bytes;
            bytes.code = T_rem_code;
            for (int p = addr + 1; p < end - 1; p++) {
                bytes.bytes.push_back(peek(p));
            }
            g_disasm_code.set_unknown(addr, INT(bytes.bytes.size()));
            line.tokens.push_back(bytes);

            addr = end - 1;
            decompile_newline(line);
            return true;
        }
    }
    return false;
}

bool ZX81::decompile_number(BasicLine1& line) {
    Token1 token;
    string num;

    // get mantissa
    int p = addr;
    int num_dots = 0;
    int num_digits = 0;
    while (true) {
        int c = peek(p);
        if (c == C_dot) {
            p++;
            num_dots++;
            num.push_back('.');
            if (num_dots > 1) {
                return false;
            }
        }
        else if (c >= C_0 && c <= C_9) {
            p++;
            num_digits++;
            num.push_back(c - C_0 + '0');
        }
        else {
            break;
        }
    }
    if (num_digits == 0) {
        return false;
    }

    // get exponent
    int c = peek(p);
    if (p == C_E) {
        p++;
        num.push_back('E');
        c = peek(p);
        if (c == C_plus || c == C_minus) {
            p++;
            num.push_back(c == C_plus ? '+' : '-');
        }
        c = peek(p);
        if (c < C_0 || c > C_9) {
            return false;
        }
        while (c >= C_0 && c <= C_9) {
            p++;
            num.push_back(c - C_0 + '0');
            c = peek(p);
        }
    }

    double value1 = atof(num.c_str());

    // get number marker
    c = peek(p);
    if (c != C_number) {
        return false;
    }
    p++;

    // get fp value
    double value2 = fpeek(p);
    p += 5;

    if (abs(value1 - value2) > 1e-6) {
        error("number " + to_string(value1) + " != " + to_string(value2));
    }

    token.code = T_float;
    token.str = num;
    token.fvalue = value1;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

bool ZX81::decompile_ident(BasicLine1& line) {
    Token1 token;
    int p = addr;
    while (true) {
        int c = peek(p);
        if (c < C_A || c > C_Z) {
            break;
        }
        token.ident.push_back(c - C_A + 'A');
        p++;
    }
    if (p == addr) {				// no letters found
        return false;
    }
    else {
        token.code = T_ident;
        line.tokens.push_back(token);
        addr = p;
        return true;
    }
}

bool ZX81::decompile_string(BasicLine1& line) {
    Token1 token;
    int p = addr;
    if (peek(p) != C_dquote) {
        return false;
    }
    p++;
    while (true) {
        int c = peek(p);
        if (c == C_newline) {
            return false;
        }
        if (c == C_dquote) {
            break;
        }
        token.str += decode_zx81(c);
        p++;
    }
    p++;		// skip end dquote

    token.code = T_string;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

void ZX81::decompile_newline(BasicLine1& line) {
    Token1 token;
    token.code = peek(addr++);
    if (token.code != C_newline) {
        error("missing newline");
    }

    line.tokens.push_back(token);
}

void ZX81::decompile_vars() {
    addr = dpeek(VARS);
    int c = 0;
    while ((c = peek(addr)) != 0x80) {
        BasicVar1 var;
        var.addr = addr;
        int addr0 = addr;

        if ((c & 0xe0) == 0x60) {		// single letter variable
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar1::Type::Number;
            var.name = decode_zx81(c);
            var.value = fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
            var.type = BasicVar1::Type::Number;

            // first letter
            addr++;
            c &= 0x3f;
            c |= 0x20;
            var.name = decode_zx81(c);

            // second, ... letter
            while (((c = peek(addr)) & 0xc0) == 0x00) {
                addr++;
                c &= 0x3f;
                c |= 0x20;
                var.name += decode_zx81(c);
            }

            // last letter
            c = peek(addr);
            if ((c & 0xc0) != 0x80) {
                error("invalid multi-letter variable", fmt_hex(c, 2));
                return;
            }
            else {
                addr++;
                c &= 0x3f;
                c |= 0x20;
                var.name += decode_zx81(c);
            }
            var.value = fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0x80) {	// array of numbers
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar1::Type::ArrayNumbers;
            var.name = decode_zx81(c);

            int size = dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dimensions = peek(addr++);
            int num_elements = 1;
            for (int i = 0; i < num_dimensions; i++) {
                int dimension = dpeek(addr);
                addr += 2;
                num_elements *= dimension;
                var.dimensions.push_back(dimension);
            }
            for (int i = 0; i < num_elements; i++) {
                double value = fpeek(addr);
                addr += 5;
                var.values.push_back(value);
            }

            assert(addr0 + size == addr);
        }
        else if ((c & 0xe0) == 0xe0) {	// for-next loop
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar1::Type::ForNextLoop;
            var.name = decode_zx81(c);

            var.value = fpeek(addr);
            addr += 5;
            var.limit = fpeek(addr);
            addr += 5;
            var.step = fpeek(addr);
            addr += 5;
            var.line_num = dpeek(addr);
            addr += 2;
        }
        else if ((c & 0xe0) == 0x40) {	// string
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar1::Type::String;
            var.name = decode_zx81(c);

            int size = dpeek(addr);
            addr += 2;
            for (int i = 0; i < size; i++) {
                c = peek(addr++);
                var.str += decode_zx81(c);
            }
        }
        else if ((c & 0xe0) == 0xc0) {	// array of strings
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar1::Type::ArrayStrings;
            var.name = decode_zx81(c);

            int size = dpeek(addr);
            addr += 2;
            int addr0 = addr;

            int num_dimensions = peek(addr++);
            int num_elements = 1;
            for (int i = 0; i < num_dimensions; i++) {
                int dimension = dpeek(addr);
                addr += 2;
                num_elements *= dimension;
                var.dimensions.push_back(dimension);
            }

            int last_dimension = var.dimensions.back();		// size of each string
            num_elements /= last_dimension;

            for (int i = 0; i < num_elements; i++) {
                string str;
                for (int j = 0; j < last_dimension; j++) {
                    c = peek(addr++);
                    str += decode_zx81(c);
                }
                var.strs.push_back(str);
            }

            assert(addr0 + size == addr);
        }
        else {
            error("invalid variable marker", string("$") + fmt_hex(c, 2));
            break;
        }

        var.size = addr - addr0;

        basic_vars.push_back(var);
    }
}

//-----------------------------------------------------------------------------
// write disassembly
//-----------------------------------------------------------------------------

string ZX81::fmt_asm(int addr, const string& opcode,
                     const string& comment) const {
    string instr, args;
    auto p = opcode.find('\t');
    if (p == string::npos) {
        instr = opcode;
        args = "";
    }
    else {
        instr = opcode.substr(0, p);
        args = opcode.substr(p + 1);
    }

    ostringstream oss;
    oss << setw(8) << ""
        << setw(8) << left << instr
        << setw(16) << left << args
        << setw(0) << "; [" << fmt_hex(addr, 4) << "] " << comment;
    return oss.str();
}

void ZX81::write_mem_info(ofstream& ofs, int start_addr, int len) const {
    ofs << endl;			// newline after REM
    ofs << "#ASM" << endl;

    for (int addr = start_addr; addr < start_addr + len; ) {
        Opcode* opc = g_disasm_code.get(addr);
        ofs << opc->to_string();
        addr += opc->size;
    }

    ofs << "#ENDASM";	// followed by newline from REM
}

void ZX81::disassemble() {
    // search all USR n in Basic
    for (auto& line : basic_lines) {
        if (line.tokens.size() > 1) {
            for (size_t i = 0; i < line.tokens.size() - 1; i++) {
                if (line.tokens[i].code == C_USR && line.tokens[i + 1].code == T_float) {
                    int addr = INT(line.tokens[i + 1].fvalue);
                    string label = g_disasm_code.get_label(addr);
                    line.tokens[i + 1].code = T_line_addr_ref;
                    line.tokens[i + 1].ident = label;
                    g_disasm_code.set_code(addr);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// assemble code in T_rem_code
//-----------------------------------------------------------------------------

void ZX81::assemble_line(const string& asm_line) {
    int n;
    p = asm_line.c_str();
    if (match("DEFB") && parse_integer(n)) {
        poke(addr++, n & 0xff);
    }
}
