//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "getopt.h"
#include "utils.h"
#include "zfloat.h"
#include "zx81.h"
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

//-----------------------------------------------------------------------------
// encode/decode zx81 character set
//-----------------------------------------------------------------------------

string decode_zx81(char c) {
    string code = zx81_chars[c & 0xff];
    if (code.size() > 1 && isalnum(code.front())) {
        code = string(" ") + code + " ";
    }
    return code;
}

vector<uint8_t> encode_zx81(const char*& p) {
    vector<uint8_t> bytes;
    while (*p != '\0') {
        if (p[0] == '\\' && isxdigit(p[1]) && isxdigit(p[2])) {
            bytes.push_back(strtol(p + 1, NULL, 16) & 0xff);
            p += 3;
        }
        else {
            bool encoded = false;
            for (size_t i = 0; i < NUM_ELEMS(zx81_chars); i++) {
                bool found = true;
                for (size_t j = 0; j < strlen(zx81_chars[i]); j++) {
                    if (toupper(p[j]) != toupper(zx81_chars[i][j])) {
                        found = false;
                        break;
                    }
                }
                if (found) {
                    bytes.push_back(i & 0xff);
                    encoded = true;
                    p += strlen(zx81_chars[i]);
                    break;
                }
            }
            if (!encoded) {
                ERROR("cannot encode: " << p);
                break;
            }
        }
    }
    return bytes;
}

//-----------------------------------------------------------------------------
// Asm labels
//-----------------------------------------------------------------------------

void AsmLabels::add(const string& name, int value) {
    auto it1 = by_name.find(name);
    if (it1 != by_name.end()) {
        ERROR("asm label redefinition: " << name);
    }

    auto it2 = by_value.find(value);
    if (it2 != by_value.end()) {
        ERROR("asm label redefinition: $" << fmt_hex(value, 4));
    }

    by_name[name] = value;
    by_value[value] = name;
}

bool AsmLabels::find(const string& name, int& value) const {
    auto it1 = by_name.find(name);
    if (it1 == by_name.end()) {
        return false;
    }
    else {
        value = it1->second;
        return true;
    }
}

bool AsmLabels::find(int value, string& name) const {
    auto it2 = by_value.find(value);
    if (it2 == by_value.end()) {
        return false;
    }
    else {
        name = it2->second;
        return true;
    }
}

void AsmLabels::clear() {
    by_name.clear();
    by_value.clear();
}

//-----------------------------------------------------------------------------
// init memory
//-----------------------------------------------------------------------------

ZX81::ZX81() {
    d_file_bytes.clear();
    d_file_bytes.push_back(C_newline);
    for (int row = 0; row < NumRows; row++) {
        for (int col = 0; col < NumCols; col++) {
            d_file_bytes.push_back(C_space);
        }
        d_file_bytes.push_back(C_newline);
    }

    e_line_bytes.clear();

    init_ram();
}

ZX81::~ZX81() {
    for (auto& m : mem_info)
        if (m) {
            delete m;
        }
}

void ZX81::init_ram() {
    init_video_to_stkend(PROG);

    // print position
    dpoke(DF_CC, dpeek(D_FILE) + 1);
    poke(DF_SZ, 2);
    poke(S_POSN_ROW, NumRows + 1);
    poke(S_POSN_COL, NumCols + 1);
    poke(PR_CC, PRBUFF >> 8);

    // system variables
    dpoke(MEM, MEMBOT);
    dpoke(LAST_K, 0xffff);
    poke(DEBOUNCE, 0xff);
    poke(MARGIN, 55);
    dpoke(FRAMES, 0xffff);
    poke(CDFLAG, FlagSlow);
    poke(PRBUFF + 32, C_newline);
}

void ZX81::init_video_to_stkend(int addr) {
    dpoke(D_FILE, addr);
    dpoke(NXTLIN, addr);
    addr += bytes_poke(addr, d_file_bytes);

    // vars
    dpoke(VARS, addr);
    poke(addr++, 0x80);

    init_e_line_to_stkend(addr);
}

void ZX81::init_e_line_to_stkend(int addr) {
    // edit line
    dpoke(E_LINE, addr);
    addr += bytes_poke(addr, e_line_bytes);

    // calculator stack
    dpoke(STKBOT, addr);
    dpoke(STKEND, addr);
}

void ZX81::init_labels() {
    asm_labels.clear();
#define X(name, value)		asm_labels.add(#name, value);
#include "consts.def"
}

//-----------------------------------------------------------------------------
// manipulate memory
//-----------------------------------------------------------------------------

int ZX81::peek(int addr) const {
    return mem[wrap_addr(addr)];
}

int ZX81::dpeek(int addr) const {
    return peek(addr) + (peek(addr + 1) << 8);
}

int ZX81::dpeek_be(int addr) const {
    return (peek(addr) << 8) + peek(addr + 1);
}

double ZX81::fpeek(int addr) const {
    array<uint8_t, 5> bytes{ 0 };
    for (int i = 0; i < 5; i++) {
        bytes[i] = peek(addr + i);
    }
    return zx81_to_float(bytes);
}

string ZX81::str_peek(int addr, int len) const {
    string out;
    for (int i = 0; i < len; i++) {
        out += decode_zx81(peek(addr + i));
    }
    return out;
}

string ZX81::bytes_peek(int addr, int len) const {
    string out;
    for (int i = 0; i < len; i++) {
        out += string("\\") + fmt_hex(peek(addr + i), 2);
    }
    return out;
}

void ZX81::poke(int addr, int value) {
    mem[wrap_addr(addr)] = value & 0xff;
}

void ZX81::dpoke(int addr, int value) {
    poke(addr, value);
    poke(addr + 1, value >> 8);
}

void ZX81::dpoke_be(int addr, int value) {
    poke(addr, value >> 8);
    poke(addr + 1, value);
}

void ZX81::fpoke(int addr, double value) {
    array<uint8_t, 5> bytes = float_to_zx81(value);
    for (int i = 0; i < 5; i++) {
        poke(addr + i, bytes[i]);
    }
}

int ZX81::str_poke(int addr, const string& str) {
    const char* p = str.c_str();
    vector<uint8_t> bytes = encode_zx81(p);
    int len = static_cast<int>(bytes.size());
    for (int i = 0; i < len; i++) {
        poke(addr + i, bytes[i]);
    }
    return len;
}

int ZX81::get_line_addr(int line_num) {
    int addr = PROG;
    int d_file = dpeek(D_FILE);
    while (addr < d_file) {
        int cur_line = dpeek_be(addr);
        if (cur_line >= line_num) {
            return addr;
        }
        int size = dpeek(addr + 2);
        addr += 4 + size;
    }
    return d_file;
}

//-----------------------------------------------------------------------------
// Decompile BASIC from ram
//-----------------------------------------------------------------------------

void ZX81::decompile() {
    decompile_basic();
    decompile_vars();
}

void ZX81::decompile_basic() {
    basic_lines.clear();
    addr = PROG;
    while (addr < dpeek(D_FILE)) {
        BasicLine line;
        line.addr = addr;
        line.line_num = dpeek_be(addr);
        line.size = dpeek(addr + 2);

        addr += 4;
        end = addr + line.size;

        decompile_basic_line(line);
        basic_lines.push_back(line);
    }
}

void ZX81::decompile_basic_line(BasicLine& line) {
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
                Token token;
                token.code = peek(addr++);
                line.tokens.push_back(token);
            }
        }
        decompile_newline(line);
    }
}

bool ZX81::decompile_rem_code(BasicLine& line) {
    if (peek(addr) == C_REM) {
        bool is_code = false;
        for (int p = addr + 1; p < end - 1; p++) {	// all chars except final newline
            if ((peek(p) & 0x40) == 0x40) {			// special char
                is_code = true;
                break;
            }
        }
        if (is_code) {
            Token rem;
            rem.code = C_REM;
            line.tokens.push_back(rem);

            Token bytes;
            bytes.code = T_rem_code;
            for (int p = addr + 1; p < end - 1; p++) {
                bytes.bytes.push_back(peek(p));
            }
            line.tokens.push_back(bytes);

            addr = end - 1;
            decompile_newline(line);
            return true;
        }
    }
    return false;
}

bool ZX81::decompile_number(BasicLine& line) {
    Token token;
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
        ERROR("line " << line.line_num << " number " << value1 << " != " << value2);
    }

    token.code = T_number;
    token.str = num;
    token.num = value1;
    line.tokens.push_back(token);
    addr = p;
    return true;
}

bool ZX81::decompile_ident(BasicLine& line) {
    Token token;
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

bool ZX81::decompile_string(BasicLine& line) {
    Token token;
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

void ZX81::decompile_newline(BasicLine& line) {
    Token token;
    token.code = peek(addr++);
    if (token.code != C_newline) {
        ERROR("line " << line.line_num << " has no newline");
    }

    line.tokens.push_back(token);
}

void ZX81::decompile_vars() {
    addr = dpeek(VARS);
    int c = 0;
    while ((c = peek(addr)) != 0x80) {
        BasicVar var;
        var.addr = addr;
        int addr0 = addr;

        if ((c & 0xe0) == 0x60) {		// single letter variable
            addr++;
            c &= 0x3f;
            c |= 0x20;

            var.type = BasicVar::Type::Number;
            var.name = decode_zx81(c);
            var.value = fpeek(addr);
            addr += 5;
        }
        else if ((c & 0xe0) == 0xa0) {	// multiple-letter variable
            var.type = BasicVar::Type::Number;

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
                ERROR("invalid multi-letter variable" << fmt_hex(c, 2));
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

            var.type = BasicVar::Type::ArrayNumbers;
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

            var.type = BasicVar::Type::ForNextLoop;
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

            var.type = BasicVar::Type::String;
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

            var.type = BasicVar::Type::ArrayStrings;
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
            ERROR("invalid variable marker: $" << fmt_hex(c, 2));
            break;
        }

        var.size = addr - addr0;

        basic_vars.push_back(var);
    }
}

//-----------------------------------------------------------------------------
// Compile BASIC into ram
//-----------------------------------------------------------------------------

void ZX81::compile() {
    delete_empty_lines();
    compute_line_numbers();

    basic_labels.clear();
    int last_end_addr = 0;
    int pass = 1;
    int num_passes_ok = 0;
    while (true) {
        compile_basic(pass);

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
    int last_line = 0;
    for (auto& line : basic_lines) {
        if (line.line_num == 0) {
            line.line_num = line_num;
            line_num += auto_increment;
        }
        else {
            line_num = line.line_num + auto_increment;
        }

        if (line.line_num <= last_line) {
            ERROR("line " << line.line_num << " follows line " << last_line);
        }
        else if (line.line_num > MaxLineNum) {
            ERROR("line " << line.line_num << " above maximum of " << MaxLineNum);
        }
    }
}

void ZX81::delete_empty_lines() {
    for (size_t i = 0; i < basic_lines.size() - 1; i++) {
        if (basic_lines[i].tokens.size() == 1
                && basic_lines[i].tokens[0].code == C_newline) {
            if (!basic_lines[i].label.empty() && !basic_lines[i + 1].label.empty()) {
                ERROR("two labels on same line: " << basic_lines[i].label << " and " <<
                      basic_lines[i + 1].label);
            }
            basic_lines[i + 1].label = basic_lines[i].label;
            basic_lines[i + 1].line_num = basic_lines[i].line_num;
            basic_lines.erase(basic_lines.begin() + i);
            i--;
        }
    }
}

void ZX81::compile_basic(int pass) {
    addr = PROG;

    for (auto& line : basic_lines) {
        // define label
        if (pass == 1 && !line.label.empty()) {
            auto it = basic_labels.find(line.label);
            if (it != basic_labels.end()) {
                ERROR("label redefinition: " << line.label);
            }
            basic_labels[line.label] = &line;
        }

        vector<uint8_t> bytes;
        for (auto& token : line.tokens) {
            switch (token.code) {
            case T_none:
                break;
            case T_number:
                compile_number(bytes, token.num);
                break;
            case T_string:
                compile_string(bytes, token.str);
                break;
            case T_ident:
                compile_ident(bytes, token.ident);
                break;
            case T_rem_code:
                // TODO: run assembler
                break;
            case T_line_num_ref:
                if (pass == 1) {
                    compile_number(bytes, 0);
                }
                else {
                    auto it = basic_labels.find(token.ident);
                    if (it == basic_labels.end()) {
                        ERROR("label undefined: " << token.ident);
                    }
                    else {
                        compile_number(bytes, it->second->line_num);
                    }
                }
                break;
            case T_line_addr_ref:
                if (pass == 1) {
                    compile_number(bytes, 0);
                }
                else {
                    auto it = basic_labels.find(token.ident);
                    if (it == basic_labels.end()) {
                        ERROR("label undefined: " << token.ident);
                    }
                    else {
                        compile_number(bytes, it->second->addr);
                    }
                }
                break;
            default:
                assert(token.code < 0x100);
                bytes.push_back(token.code);
            }
        }

        // define addr and size
        line.addr = addr;
        line.size = static_cast<int>(bytes.size());

        dpoke_be(addr, line.line_num);
        addr += 2;
        dpoke(addr, line.size);
        addr += 2;
        addr += bytes_poke(addr, bytes);
        assert(addr == line.addr + 4 + line.size);
    }

    init_video_to_stkend(addr);

    if (autostart) {
        int line_addr = get_line_addr(autostart);
        dpoke(NXTLIN, line_addr);
    }
    else {
        dpoke(NXTLIN, dpeek(D_FILE));
    }
}

void ZX81::compile_vars() {
    addr = dpeek(VARS);

    vector<uint8_t> str_bytes;
    array<uint8_t, 5> fp_bytes{ 0 };
    int last_dimension = 0;
    int size = 0;

    for (auto& var : basic_vars) {
        const char* p = var.name.c_str();
        vector<uint8_t> name_bytes = encode_zx81(p);
        assert(name_bytes.size() > 0);

        switch (var.type) {
        case BasicVar::Type::Number:
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
            addr += bytes_poke(addr, fp_bytes);
            break;

        case BasicVar::Type::ArrayNumbers:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0x80);				// 100-letter

            // put dimensions
            size = 1
                   + 2 * static_cast<int>(var.dimensions.size())
                   + 5 * static_cast<int>(var.values.size());
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
                addr += bytes_poke(addr, fp_bytes);
            }
            break;

        case BasicVar::Type::ForNextLoop:
            // put name
            poke(addr++, (name_bytes[0] & 0x3f) | 0xe0);				// 111-letter

            // put values
            fp_bytes = float_to_zx81(var.value);
            addr += bytes_poke(addr, fp_bytes);

            fp_bytes = float_to_zx81(var.limit);
            addr += bytes_poke(addr, fp_bytes);

            fp_bytes = float_to_zx81(var.step);
            addr += bytes_poke(addr, fp_bytes);

            dpoke(addr, var.line_num);
            addr += 2;
            break;

        case BasicVar::Type::String:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0x40);				// 010-letter

            dpoke(addr, static_cast<int>(var.str.size()));
            addr += 2;

            p = var.str.c_str();
            str_bytes = encode_zx81(p);
            addr += bytes_poke(addr, str_bytes);
            break;

        case BasicVar::Type::ArrayStrings:
            // put name
            poke(addr++, (name_bytes[0] & 0x1f) | 0xc0);				// 110-letter

            // put dimensions
            last_dimension = var.dimensions.back();
            size = 1
                   + 2 * static_cast<int>(var.dimensions.size())
                   + last_dimension * static_cast<int>(var.strs.size());
            dpoke(addr, size);
            addr += 2;
            poke(addr++, static_cast<int>(var.dimensions.size()));

            for (auto& dimension : var.dimensions) {
                dpoke(addr, dimension);
                addr += 2;
            }

            // put strings
            for (auto& str : var.strs) {
                p = str.c_str();
                str_bytes = encode_zx81(p);
                addr += bytes_poke(addr, str_bytes);
            }
            break;

        default:
            assert(0);
        }
    }

    poke(addr++, 0x80);
    init_e_line_to_stkend(addr);
}

void ZX81::compile_number(vector<uint8_t>& bytes, double value) {
    ostringstream oss;

    oss << value;
    string value_str = oss.str();
    const char* p = value_str.c_str();
    vector<uint8_t> str_bytes = encode_zx81(p);
    array<uint8_t, 5> fp_bytes = float_to_zx81(value);
    bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
    bytes.push_back(C_number);
    bytes.insert(bytes.end(), fp_bytes.begin(), fp_bytes.end());
}

void ZX81::compile_string(vector<uint8_t>& bytes, const string& str) {
    const char* p = str.c_str();
    vector<uint8_t> str_bytes = encode_zx81(p);
    bytes.push_back(C_dquote);
    bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
    bytes.push_back(C_dquote);
}

void ZX81::compile_ident(vector<uint8_t>& bytes, const string& ident) {
    const char* p = ident.c_str();
    vector<uint8_t> str_bytes = encode_zx81(p);
    bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
}

//-----------------------------------------------------------------------------
// read/write binary files
//-----------------------------------------------------------------------------

void ZX81::read_p_file(const string& filename) {
    // open file
    ifstream ifs(filename, ios::binary);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        FATAL_ERROR("open file " << filename);
    }

    // get size
    ifs.seekg(0, ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, ios::beg);

    // read bytes
    ifs.read(reinterpret_cast<char*>(&mem[SAVE_ADDR]), size);
    size_t readed = ifs.gcount();
    if (readed != size) {
        perror(filename.c_str());
        FATAL_ERROR("read " << size << " bytes from file " << filename);
    }
}

void ZX81::write_p_file(const string& filename) const {
    // open file
    ofstream ofs(filename, ios::binary);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        FATAL_ERROR("open file " << filename);
    }

    size_t size = dpeek(E_LINE) - SAVE_ADDR;
    ofs.write(reinterpret_cast<const char*>(&mem[SAVE_ADDR]), size);
    size_t written = ofs.tellp();
    if (written != size) {
        perror(filename.c_str());
        FATAL_ERROR("write " << size << " bytes to file " << filename);
    }
}

//-----------------------------------------------------------------------------
// Read BASIC file
//-----------------------------------------------------------------------------

void ZX81::read_b81_file(const string& filename) {
    ifstream ifs(filename);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        FATAL_ERROR("read file " << filename);
    }

    string text;
    error_filename = filename;
    error_line_num = 0;

    while (getline(ifs, text)) {
        error_line_num++;
        while (!text.empty() && text.back() == '\\') {
            text.pop_back();
            text.push_back(' ');
            string cont;
            if (!getline(ifs, cont)) {
                break;
            }
            error_line_num++;
            text += cont;
        }
        parse_line(text.c_str());
    }

    error_filename.clear();
    error_line_num = 0;
}

void ZX81::skip_spaces(const char*& p) {
    while (*p != '\0' && isspace(*p)) {
        p++;
    }
}

bool ZX81::match(const char*& p, const string& compare) {
    skip_spaces(p);
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

bool ZX81::parse_integer(const char*& p, int& value) {
    skip_spaces(p);
    const char* p0 = p;
    if (*p == '$') {	// hex
        if (!isxdigit(p[1])) {
            return false;
        }
        p++;
        while (isxdigit(*p)) {
            p++;
        }
        value = static_cast<int>(strtol(p0 + 1, NULL, 16));
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

bool ZX81::parse_number(const char*& p, double& value, string& value_text) {
    skip_spaces(p);
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
        if (!parse_integer(p, exp)) {
            p = p0;
            return false;
        }
    }

    value = atof(p0);
    value_text = string(p0, p);
    return true;
}

bool ZX81::parse_string(const char*& p, string& str) {
    string out;
    skip_spaces(p);
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
        return false;
    }
    else {
        p++;
        str = out;
        return true;
    }
}

bool ZX81::parse_ident(const char*& p, string& ident) {
    skip_spaces(p);
    if (!isalpha(*p)) {
        return false;
    }
    while (isalnum(*p)) {
        ident.push_back(*p++);
    }
    return true;
}

bool ZX81::parse_label(const char*& p, string& ident) {
    skip_spaces(p);
    const char* p0 = p;
    if (!parse_line_num_ref(p, ident)) {
        p = p0;
        return false;
    }
    skip_spaces(p);
    if (*p != ':') {
        p = p0;
        return false;
    }
    p++;
    return true;
}

bool ZX81::parse_line_num_ref(const char*& p, string& ident) {
    skip_spaces(p);
    const char* p0 = p;
    if (*p != '@') {
        return false;
    }
    p++;
    if (!parse_ident(p, ident)) {
        p = p0;
        return false;
    }
    return true;
}

bool ZX81::parse_line_addr_ref(const char*& p, string& ident) {
    skip_spaces(p);
    const char* p0 = p;
    if (*p != '&') {
        return false;
    }
    p++;
    if (!parse_ident(p, ident)) {
        p = p0;
        return false;
    }
    return true;
}

bool ZX81::parse_end(const char*& p) {
    skip_spaces(p);
    if (*p == '\0' || *p == '#') {
        return true;
    }
    else if (in_asm && *p == ';') {
        return true;
    }
    else {
        return false;
    }
}

void ZX81::parse_line(const char* p) {
    const char* p0 = p;
    skip_spaces(p);
    if (*p == '\0') {
    }
    else if (*p == '#') {
        parse_meta_line(p + 1);
    }
    else if (in_asm) {
        parse_asm_line(p0);
    }
    else {
        parse_basic_line(p);
    }
}

void ZX81::parse_meta_line(const char* p) {
    int n = 0;
    if (match(p, "VARS")) {
        parse_basic_var(p);
    }
    else if (match(p, "SYSVARS") && match(p, "=")) {
        vector<uint8_t> bytes = encode_zx81(p);
        for (size_t i = 0; i < bytes.size()
                && SAVE_ADDR + static_cast<int>(i) < PROG; i++) {
            poke(SAVE_ADDR + static_cast<int>(i), bytes[i]);
        }
    }
    else if (match(p, "D_FILE") && match(p, "=")) {
        d_file_bytes = encode_zx81(p);
        int d_file = dpeek(D_FILE);
        int vars = dpeek(VARS);
        for (size_t i = 0; i < d_file_bytes.size()
                && d_file + static_cast<int>(i) < vars; i++) {
            poke(d_file + static_cast<int>(i), d_file_bytes[i]);
        }
    }
    else if (match(p, "WORKSPACE") && match(p, "=")) {
        e_line_bytes = encode_zx81(p);
        int e_line = dpeek(E_LINE);
        int stkend = dpeek(STKEND);
        for (size_t i = 0; i < e_line_bytes.size()
                && e_line + static_cast<int>(i) < stkend; i++) {
            poke(e_line + static_cast<int>(i), e_line_bytes[i]);
        }
    }
    else if (match(p, "AUTOSTART") && match(p, "=") && parse_integer(p, n)
             && parse_end(p)) {
        autostart = n;
    }
    else if (match(p, "FAST") && match(p, "=") && parse_integer(p, n)
             && parse_end(p)) {
        if (n) {
            poke(CDFLAG, peek(CDFLAG) & ~FlagSlow);
        }
        else {
            poke(CDFLAG, peek(CDFLAG) | FlagSlow);
        }
    }
    else if (match(p, "INCREMENT") && match(p, "=") && parse_integer(p, n)
             && parse_end(p)) {
        auto_increment = n;
    }
    else if (match(p, "ASM")) {
        in_asm = true;
    }
    else if (match(p, "ENDASM")) {
        in_asm = false;
    }
    else {
        // ignore, consider a comment
    }
}

void ZX81::parse_basic_line(const char* p) {
    BasicLine line;

    // get label and/or line number
    bool got_line_num = false;
    bool got_label = false;
    bool found_some = false;
    do {
        found_some = false;
        if (!got_line_num && parse_integer(p, line.line_num)) {
            got_line_num = true;
            found_some = true;
        }
        if (!got_label && parse_label(p, line.label)) {
            got_label = true;
            found_some = true;
        }
    }
    while (found_some);

    skip_spaces(p);
    while (*p != '\0' && *p != '#') {
        Token token;

        // decode first multi-char tokens
        for (int c = 0xff; c >= 0; c--) {
            string text = zx81_chars[c];
            if (text.size() == 1 && isalnum(text[0])) {
                continue;
            }
            else if (match(p, text)) {
                token.code = c;
                line.tokens.push_back(token);
                break;
            }
        }

        // then special tokens
        if (token.code == C_REM) {
            Token rem;
            rem.code = C_REM;
            line.tokens.push_back(rem);
        }
        else if (token.code == T_none) {		// not yet found
            if (match(p, "_")) {
                token.code = C_space;
                line.tokens.push_back(token);
            }
            else if (parse_number(p, token.num, token.str)) {
                token.code = T_number;
                line.tokens.push_back(token);
            }
            else if (parse_string(p, token.str)) {
                token.code = T_string;
                line.tokens.push_back(token);
            }
            else if (parse_ident(p, token.ident)) {
                token.code = T_ident;
                line.tokens.push_back(token);
            }
            else if (parse_line_addr_ref(p, token.ident)) {
                token.code = T_line_addr_ref;
                line.tokens.push_back(token);
            }
            else if (parse_line_num_ref(p, token.ident)) {
                token.code = T_line_num_ref;
                line.tokens.push_back(token);
            }
            else {
                ERROR("cannot parse: " << p);
                break;
            }
        }

        skip_spaces(p);
    }

    Token token;
    token.code = C_newline;
    line.tokens.push_back(token);

    basic_lines.push_back(line);
}

void ZX81::parse_basic_var(const char* p) {
    BasicVar var;
    bool is_string = false;
    bool is_array = false;
    int num_elements = 0;

    // get name
    if (!parse_ident(p, var.name)) {
        goto error;
    }

    // get string marker
    if (match(p, "$")) {
        is_string = true;
        if (var.name.size() > 1) {
            ERROR("name " << var.name << "too long");
            return;
        }
    }

    // get array marker and dimensions
    if (match(p, "(")) {
        is_array = true;
        if (var.name.size() > 1) {
            ERROR("name " << var.name << "too long");
            return;
        }

        num_elements = 1;
        do {
            int dimension = 0;
            if (!parse_integer(p, dimension)) {
                goto error;
            }
            num_elements *= dimension;
            var.dimensions.push_back(dimension);
        }
        while (match(p, ","));

        if (!match(p, ")")) {
            goto error;
        }
    }

    // get =
    if (!match(p, "=")) {
        goto error;
    }

    // get value
    if (is_string == false && is_array == false) {
        string str;

        var.type = BasicVar::Type::Number;
        if (!parse_number(p, var.value, str)) {
            goto error;
        }

        if (match(p, ",")) {
            var.type = BasicVar::Type::ForNextLoop;
            if (var.name.size() > 1) {
                ERROR("name " << var.name << "too long");
                return;
            }

            if (!parse_number(p, var.limit, str)) {
                goto error;
            }
            if (!match(p, ",")) {
                goto error;
            }
            if (!parse_number(p, var.step, str)) {
                goto error;
            }
            if (!match(p, ",")) {
                goto error;
            }
            if (!parse_integer(p, var.line_num)) {
                goto error;
            }
            if (!parse_end(p)) {
                goto error;
            }
        }
    }
    else if (is_string == false && is_array == true) {
        double value = 0.0;
        string str;

        var.type = BasicVar::Type::ArrayNumbers;

        for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
            if (i > 0) {
                if (!match(p, ",")) {
                    goto error;
                }
            }
            if (!parse_number(p, value, str)) {
                goto error;
            }
            var.values.push_back(value);
        }
        if (!parse_end(p)) {
            goto error;
        }
    }
    else if (is_string == true && is_array == false) {
        var.type = BasicVar::Type::String;

        if (!parse_string(p, var.str)) {
            goto error;
        }
        if (!parse_end(p)) {
            goto error;
        }
    }
    else if (is_string == true && is_array == true) {
        string str;

        var.type = BasicVar::Type::ArrayStrings;

        int last_dimension = var.dimensions.back();
        num_elements /= last_dimension;

        for (size_t i = 0; i < static_cast<size_t>(num_elements); i++) {
            if (i > 0) {
                if (!match(p, ",")) {
                    goto error;
                }
            }
            if (!parse_string(p, str)) {
                goto error;
            }
            if (str.size() != static_cast<size_t>(last_dimension)) {
                ERROR("string length should be " << last_dimension);
                return;
            }
            var.strs.push_back(str);
        }
        if (!parse_end(p)) {
            goto error;
        }
    }

    basic_vars.push_back(var);
    return;

error:
    ERROR("cannot parse: " << p);
}

//-----------------------------------------------------------------------------
// write disassembly
//-----------------------------------------------------------------------------

static string fmt_asm(int addr, const string& opcode, const string& comment) {
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

void ZX81::write_mem_info(ofstream& ofs, int start_addr, int end_addr) const {
    ofs << endl;			// newline after REM
    ofs << endl << "#ASM" << endl;

    for (int addr = start_addr; addr < end_addr; ) {
        MemInfo* m = mem_info[wrap_addr(addr)];
        if (m) {
            if (!m->header.empty()) {
                ofs << m->header << endl;
            }
        }

        string label;
        if (asm_labels.find(addr, label)) {
            ofs << label << ":" << endl;
        }

        if (m) {
            ofs << fmt_asm(addr, m->disass, m->comment) << endl;
            addr += m->size;
        }
        else {
            ofs << fmt_asm(addr, "DEFB\t$" + fmt_hex(peek(addr), 2), "") << endl;
            addr++;
        }
    }

    ofs << "#ENDASM" << endl;
}

//-----------------------------------------------------------------------------
// write BASIC files
//-----------------------------------------------------------------------------

void ZX81::write_b81_file(const string& filename) const {
    ofstream ofs(filename);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        FATAL_ERROR("write file " << filename);
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
}

void ZX81::write_sysvars(ofstream& ofs) const {
    ofs << "# [VERSN     =  " << peek(VERSN) << "]" << endl;
    ofs << "# [E_PPC     =  " << dpeek(E_PPC) << "]" << endl;
    ofs << "# [D_FILE    = $" << fmt_hex(dpeek(D_FILE), 4) << "]" << endl;
    ofs << "# [DF_CC     = $" << fmt_hex(dpeek(DF_CC), 4) << "]" << endl;
    ofs << "# [VARS      = $" << fmt_hex(dpeek(VARS), 4) << "]" << endl;
    ofs << "# [DEST      = $" << fmt_hex(dpeek(DEST), 4) << "]" << endl;
    ofs << "# [E_LINE    = $" << fmt_hex(dpeek(E_LINE), 4) << "]" << endl;
    ofs << "# [CH_ADD    = $" << fmt_hex(dpeek(CH_ADD), 4) << "]" << endl;
    ofs << "# [X_PTR     = $" << fmt_hex(dpeek(X_PTR), 4) << "]" << endl;
    ofs << "# [STKBOT    = $" << fmt_hex(dpeek(STKBOT), 4) << "]" << endl;
    ofs << "# [STKEND    = $" << fmt_hex(dpeek(STKEND), 4) << "]" << endl;
    ofs << "# [BREG      =  " << peek(BREG) << "]" << endl;
    ofs << "# [MEM       = $" << fmt_hex(dpeek(MEM), 4) << "]" << endl;
    ofs << "# [FREE1     =  " << peek(FREE1) << "]" << endl;
    ofs << "# [DF_SZ     =  " << peek(DF_SZ) << "]" << endl;
    ofs << "# [S_TOP     =  " << dpeek(S_TOP) << "]" << endl;
    ofs << "# [LAST_K    = $" << fmt_hex(dpeek(LAST_K), 4) << "]" << endl;
    ofs << "# [DEBOUNCE  =$" << fmt_hex(peek(DEBOUNCE), 2) << "]" << endl;
    ofs << "# [MARGIN    =  " << peek(MARGIN) << "]" << endl;
    ofs << "# [NXTLIN    = $" << fmt_hex(dpeek(NXTLIN), 4) << "]" << endl;
    ofs << "# [OLDPPC    =  " << dpeek(OLDPPC) << "]" << endl;
    ofs << "# [FLAGX     = $" << fmt_hex(peek(FLAGX), 2) << "]" << endl;
    ofs << "# [STRLEN    =  " << dpeek(STRLEN) << "]" << endl;
    ofs << "# [T_ADDR    = $" << fmt_hex(dpeek(T_ADDR), 4) << "]" << endl;
    ofs << "# [SEED      = $" << fmt_hex(dpeek(SEED), 4) << "]" << endl;
    ofs << "# [FRAMES    = $" << fmt_hex(dpeek(FRAMES), 4) << "]" << endl;
    ofs << "# [COORDS_X  =  " << peek(COORDS_X) << "]" << endl;
    ofs << "# [COORDS_Y  =  " << peek(COORDS_Y) << "]" << endl;
    ofs << "# [PR_CC     = $" << fmt_hex(peek(PR_CC), 2) << "]" << endl;
    ofs << "# [S_POSN_COL=  " << peek(S_POSN_COL) << "]" << endl;
    ofs << "# [S_POSN_ROW=  " << peek(S_POSN_ROW) << "]" << endl;
    ofs << "# [CDFLAG    = $" << fmt_hex(peek(CDFLAG), 2) << "]" << endl;

    ofs << "# [PRBUFF=";
    for (int i = 0; i < 33; i++) {
        ofs << "\\" << fmt_hex(peek(PRBUFF + i), 2);
    }
    ofs << "]" << endl;

    ofs << "# [MEMBOT=";
    for (int i = 0; i < 30; i++) {
        ofs << "\\" << fmt_hex(peek(MEMBOT + i), 2);
    }
    ofs << "]" << endl;

    ofs << "# [FREE2     = $" << fmt_hex(dpeek(FREE2), 4) << "]" << endl;
    ofs << endl;
}

void ZX81::write_basic_lines(ofstream& ofs) const {
    for (auto& line : basic_lines) {
        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [$" << fmt_hex(line.addr, 4) << "]" << endl;
        }

        if (!line.label.empty()) {
            ofs << "@" << line.label << ":" << endl;
        }

        ofs << fmt_line_number(line.line_num) << " ";

        for (auto& token : line.tokens) {
            switch (token.code) {
            case T_none:
                break;
            case T_number:
                ofs << token.str;
                break;
            case T_string:
                ofs << "\"" << token.str << "\"";
                break;
            case T_ident:
                ofs << token.ident;
                break;
            case T_rem_code:
                write_mem_info(ofs, line.addr + 5,
                               line.addr + 5 + static_cast<int>(token.bytes.size()));
                break;
            case C_space:
                ofs << "_";
                break;
            case C_newline:
                ofs << endl;
                break;
            default:
                ofs << decode_zx81(token.code);
            }
        }
    }

    if (!basic_lines.empty()) {
        ofs << endl;
    }
}

void ZX81::write_video(ofstream& ofs) const {
    int addr = dpeek(D_FILE);
    int vars = dpeek(VARS);
    while (addr < vars) {
        ofs << "# [$" << fmt_hex(addr, 4) << "] = \"";
        int c;
        do {
            c = peek(addr++);
            ofs << decode_zx81(c);
        }
        while (c != C_newline);
        ofs << "\"" << endl;
    }

    ofs << endl;
}

void ZX81::write_basic_vars(ofstream& ofs) const {
    int num_elements = 0;
    int last_dimension = 0;

    int addr = dpeek(VARS);
    for (auto& var : basic_vars) {
        assert(addr == var.addr);

        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [$" << fmt_hex(addr, 4) << "]" << endl;
        }

        switch (var.type) {
        case BasicVar::Type::Number:
            ofs << "#VARS " << var.name << "=" << var.value << endl;
            break;
        case BasicVar::Type::ArrayNumbers:
            num_elements = 1;
            ofs << "#VARS " << var.name << "(";

            for (size_t i = 0; i < var.dimensions.size(); i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.dimensions[i];
                num_elements *= var.dimensions[i];
            }

            ofs << ")=";

            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.values[i];
            }

            ofs << endl;
            break;
        case BasicVar::Type::ForNextLoop:
            ofs << "#VARS " << var.name << "=" << var.value
                << "," << var.limit << "," << var.step << "," << var.line_num << endl;
            break;
        case BasicVar::Type::String:
            ofs << "#VARS " << var.name << "$=\"" << var.str << "\"" << endl;
            break;
        case BasicVar::Type::ArrayStrings:
            num_elements = 1;
            ofs << "#VARS " << var.name << "$(";

            for (size_t i = 0; i < var.dimensions.size(); i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.dimensions[i];
                num_elements *= var.dimensions[i];
            }

            ofs << ")=";

            last_dimension = var.dimensions.back();
            num_elements /= last_dimension;
            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << "\"" << var.strs[i] << "\"";
            }

            ofs << endl;
            break;
        default:
            assert(0);
        }
        addr += var.size;
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        ofs << "# [$" << fmt_hex(addr, 4) << "] = $" << fmt_hex(peek(addr), 2) << endl;
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG || !basic_vars.empty()) {
        ofs << endl;
    }
}

void ZX81::write_basic_system(ofstream& ofs) const {
    ofs << "#SYSVARS=" << bytes_peek(SAVE_ADDR, PROG - SAVE_ADDR) << endl;
    ofs << "#D_FILE=" << bytes_peek(dpeek(D_FILE),
                                    dpeek(VARS) - dpeek(D_FILE)) << endl;
    ofs << "#WORKSPACE=" << bytes_peek(dpeek(E_LINE),
                                       dpeek(STKEND) - dpeek(E_LINE)) << endl;
    ofs << endl;

    // autostart
    int nxtlin = dpeek(NXTLIN);
    int autostart = 0;
    if (nxtlin >= dpeek(D_FILE)) {
        autostart = 0;
    }
    else {
        autostart = dpeek_be(nxtlin);
    }
    ofs << "#AUTOSTART=" << autostart << endl;

    // fast mode
    int fast = (peek(CDFLAG) & 0x40) == 0 ? 1 : 0;
    ofs << "#FAST=" << fast << endl;
}
//-----------------------------------------------------------------------------
// assemble asm code
//-----------------------------------------------------------------------------

void ZX81::parse_asm_line(const char* p) {
    if (basic_lines.empty()) {
        ERROR("#ASM block must follow an empty REM statement");
    }
    else {
        BasicLine* last_line = &basic_lines.back();
        if (last_line->tokens.size() != 3 ||
                last_line->tokens[0].code != C_REM ||
                last_line->tokens[1].code != T_rem_code ||
                last_line->tokens[2].code != C_newline) {
            ERROR("#ASM block must follow an empty REM statement");
        }
        else {
            Token& rem_code = last_line->tokens[1];

            rem_code.asm_lines.push_back(p);

            int n = 0;
            if (match(p, ";")) {	// comment
            }
            else if (match(p, "DEFB") && parse_integer(p, n)) {
                rem_code.bytes.push_back(n);
            }
            else {
                ERROR("cannot parse: " << p);
            }
        }
    }
}

