//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "consts.h"
#include "errors.h"
#include "utils.h"
#include "zx81encode.h"
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>

std::string decode_zx81(char c) {
    std::string code = zx81_chars[c & 0xff];
    if (code.size() > 1 && isalnum(code.front())) {
        code = " " + code + " ";
    }
    return code;
}

Bytes encode_zx81(const std::string& str, const SourceLoc& loc) {
    const char* p = str.c_str();
    return encode_zx81(p, loc);
}

Bytes encode_zx81(const char*& p, const SourceLoc& loc) {
    Bytes bytes;
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
                error(loc, "cannot encode " + std::string(p));
                break;
            }
        }
    }
    return bytes;
}

std::string encode_hex(const Bytes& bytes) {
    std::ostringstream oss;
    for (auto& b : bytes) {
        oss << "\\" << fmt_hex(b, 2, "");
    }
    return oss.str();
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
    for (auto& code : asm_code) {
        if (code) {
            delete code;
        }
        asm_code.clear();
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
    if (addr >= RAM_ADDR && addr < RAM_ADDR + static_cast<int>(ram.size())) {
        return ram[addr - RAM_ADDR];
    }
    else {
        return 0;
    }
}

int ZX81::speek(int addr) const {
    int s = peek(addr);
    if (s >= 128) {
        s -= 256;
    }
    return s;
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
    if (addr >= RAM_ADDR && addr < RAM_ADDR + static_cast<int>(ram.size())) {
        ram[addr - RAM_ADDR] = value & 0xff;
    }
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
                bytes.insert(bytes.end(), token.bytes.begin(), token.bytes.end());
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
    ifs.read(reinterpret_cast<char*>(&ram[SAVE_ADDR - RAM_ADDR]), size);
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
    ofs.write(reinterpret_cast<const char*>(&ram[SAVE_ADDR - RAM_ADDR]), size);
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
    else if (match(p, "STARTASM")) {
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
        if (match(p, "RND")) {
            token.code = C_RND;
        }
        else if (match(p, "INKEY$")) {
            token.code = C_INKEY_dollar;
        }
        else if (match(p, "PI")) {
            token.code = C_PI;
        }
        else if (match(p, "AT")) {
            token.code = C_AT;
        }
        else if (match(p, "TAB")) {
            token.code = C_TAB;
        }
        else if (match(p, "CODE")) {
            token.code = C_CODE;
        }
        else if (match(p, "VAL")) {
            token.code = C_VAL;
        }
        else if (match(p, "LEN")) {
            token.code = C_LEN;
        }
        else if (match(p, "SIN")) {
            token.code = C_SIN;
        }
        else if (match(p, "COS")) {
            token.code = C_COS;
        }
        else if (match(p, "TAN")) {
            token.code = C_TAN;
        }
        else if (match(p, "ASN")) {
            token.code = C_ASN;
        }
        else if (match(p, "ACS")) {
            token.code = C_ACS;
        }
        else if (match(p, "ATN")) {
            token.code = C_ATN;
        }
        else if (match(p, "LN")) {
            token.code = C_LN;
        }
        else if (match(p, "EXP")) {
            token.code = C_EXP;
        }
        else if (match(p, "INT")) {
            token.code = C_INT;
        }
        else if (match(p, "SQR")) {
            token.code = C_SQR;
        }
        else if (match(p, "SGN")) {
            token.code = C_SGN;
        }
        else if (match(p, "ABS")) {
            token.code = C_ABS;
        }
        else if (match(p, "PEEK")) {
            token.code = C_PEEK;
        }
        else if (match(p, "USR")) {
            token.code = C_USR;
        }
        else if (match(p, "STR$")) {
            token.code = C_STR_dollar;
        }
        else if (match(p, "CHR$")) {
            token.code = C_CHR_dollar;
        }
        else if (match(p, "NOT")) {
            token.code = C_NOT;
        }
        else if (match(p, "**")) {
            token.code = C_power;
        }
        else if (match(p, "OR")) {
            token.code = C_OR;
        }
        else if (match(p, "AND")) {
            token.code = C_AND;
        }
        else if (match(p, "<=")) {
            token.code = C_le;
        }
        else if (match(p, ">=")) {
            token.code = C_ge;
        }
        else if (match(p, "<>")) {
            token.code = C_ne;
        }
        else if (match(p, "THEN")) {
            token.code = C_THEN;
        }
        else if (match(p, "TO")) {
            token.code = C_TO;
        }
        else if (match(p, "STEP")) {
            token.code = C_STEP;
        }
        else if (match(p, "LPRINT")) {
            token.code = C_LPRINT;
        }
        else if (match(p, "LLIST")) {
            token.code = C_LLIST;
        }
        else if (match(p, "STOP")) {
            token.code = C_STOP;
        }
        else if (match(p, "SLOW")) {
            token.code = C_SLOW;
        }
        else if (match(p, "FAST")) {
            token.code = C_FAST;
        }
        else if (match(p, "NEW")) {
            token.code = C_NEW;
        }
        else if (match(p, "SCROLL")) {
            token.code = C_SCROLL;
        }
        else if (match(p, "CONT")) {
            token.code = C_CONT;
        }
        else if (match(p, "DIM")) {
            token.code = C_DIM;
        }
        else if (match(p, "REM")) {
            token.code = C_REM;
            line.tokens.push_back(token);

            skip_spaces(p);
            token.code = T_rem_code;
            token.bytes = encode_zx81(p);
        }
        else if (match(p, "FOR")) {
            token.code = C_FOR;
        }
        else if (match(p, "GOTO")) {
            token.code = C_GOTO;
        }
        else if (match(p, "GOSUB")) {
            token.code = C_GOSUB;
        }
        else if (match(p, "INPUT")) {
            token.code = C_INPUT;
        }
        else if (match(p, "LOAD")) {
            token.code = C_LOAD;
        }
        else if (match(p, "LIST")) {
            token.code = C_LIST;
        }
        else if (match(p, "LET")) {
            token.code = C_LET;
        }
        else if (match(p, "PAUSE")) {
            token.code = C_PAUSE;
        }
        else if (match(p, "NEXT")) {
            token.code = C_NEXT;
        }
        else if (match(p, "POKE")) {
            token.code = C_POKE;
        }
        else if (match(p, "PRINT")) {
            token.code = C_PRINT;
        }
        else if (match(p, "PLOT")) {
            token.code = C_PLOT;
        }
        else if (match(p, "RUN")) {
            token.code = C_RUN;
        }
        else if (match(p, "SAVE")) {
            token.code = C_SAVE;
        }
        else if (match(p, "RAND")) {
            token.code = C_RAND;
        }
        else if (match(p, "IF")) {
            token.code = C_IF;
        }
        else if (match(p, "CLS")) {
            token.code = C_CLS;
        }
        else if (match(p, "UNPLOT")) {
            token.code = C_UNPLOT;
        }
        else if (match(p, "CLEAR")) {
            token.code = C_CLEAR;
        }
        else if (match(p, "RETURN")) {
            token.code = C_RETURN;
        }
        else if (match(p, "COPY")) {
            token.code = C_COPY;
        }
        else if (match(p, "\\0c")) {
            token.code = C_pound;
        }
        else if (match(p, "$")) {
            token.code = C_dollar;
        }
        else if (match(p, ":")) {
            token.code = C_colon;
        }
        else if (match(p, "?")) {
            token.code = C_quest;
        }
        else if (match(p, "(")) {
            token.code = C_lparens;
        }
        else if (match(p, ")")) {
            token.code = C_rparens;
        }
        else if (match(p, ">")) {
            token.code = C_gt;
        }
        else if (match(p, "<")) {
            token.code = C_lt;
        }
        else if (match(p, "=")) {
            token.code = C_eq;
        }
        else if (match(p, "+")) {
            token.code = C_plus;
        }
        else if (match(p, "-")) {
            token.code = C_minus;
        }
        else if (match(p, "*")) {
            token.code = C_mult;
        }
        else if (match(p, "/")) {
            token.code = C_div;
        }
        else if (match(p, ";")) {
            token.code = C_semicolon;
        }
        else if (match(p, ",")) {
            token.code = C_comma;
        }
        else if (match(p, "_")) {
            token.code = C_space;
        }
        else if (parse_number(p, token.num, token.str)) {
            token.code = T_number;
        }
        else if (parse_string(p, token.str)) {
            token.code = T_string;
        }
        else if (parse_ident(p, token.ident)) {
            token.code = T_ident;
        }
        else if (parse_line_addr_ref(p, token.ident)) {
            token.code = T_line_addr_ref;
        }
        else if (parse_line_num_ref(p, token.ident)) {
            token.code = T_line_num_ref;
        }
        else {
            ERROR("line " << error_line_num << ": cannot parse: " << p);
            break;
        }

        line.tokens.push_back(token);
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
            ERROR("line " << error_line_num << ": name " << var.name << "too long");
            return;
        }
    }

    // get array marker and dimensions
    if (match(p, "(")) {
        is_array = true;
        if (var.name.size() > 1) {
            ERROR("line " << error_line_num << ": name " << var.name << "too long");
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
                ERROR("line " << error_line_num << ": name " << var.name << "too long");
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
                ERROR("line " << error_line_num << ": string length should be " <<
                      last_dimension);
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
    ERROR("line " << error_line_num << ": cannot parse: " << p);
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
                ofs << endl;
                write_asm_lines(ofs, line.addr + 5,
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

void ZX81::write_asm_lines(ofstream& ofs, int start_addr, int end_addr) const {
    ofs << "#STARTASM" << endl;
    for (int addr = start_addr; addr < end_addr; addr++) {
        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [$" << fmt_hex(addr, 4) << "]" << endl;
        }

        string name;
        if (asm_labels.find(addr, name)) {
            ofs << name << ":" << endl;
        }
        AsmLine* asm_line = get_asm_line(addr);
        if (asm_line) {
            ofs << "        " << asm_line->instr << endl;
        }
        else {
            ofs << "        defb $" << fmt_hex(peek(addr), 2) << endl;
        }
    }
    ofs << "#ENDASM" << endl;
}

//-----------------------------------------------------------------------------
// disassemble asm code
//-----------------------------------------------------------------------------

AsmLine* ZX81::get_asm_line(int addr) const {
    assert(addr >= RAM_ADDR);
    size_t idx = addr - RAM_ADDR;
    if (idx + 1 > asm_code.size()) {
        return nullptr;
    }
    else {
        return asm_code[idx];
    }
}

AsmLine* ZX81::make_asm_line(int addr) {
    assert(addr >= RAM_ADDR);
    size_t idx = addr - RAM_ADDR;
    if (idx + 1 > asm_code.size()) {
        asm_code.resize(idx + 1);
    }
    assert(asm_code[idx] == nullptr);
    asm_code[idx] = new AsmLine(addr);
    return asm_code[idx];
}

AsmLine* ZX81::disasm(int addr) {
    AsmLine* line = get_asm_line(addr);
    if (!line) {
        line = disasm1(addr);
    }
    return line;
}

AsmLine* ZX81::disasm1(int addr) {
    AsmLine* line = make_asm_line(addr);
    int n = 0;
    switch (peek(addr++)) {
    case 0x00:
        line->opcode = line->instr = "nop";
        break;
    case 0x01:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld bc,NN";
        line->instr = "ld bc,$" + fmt_hex(line->op1, 4);
        break;
    case 0x02:
        line->opcode = line->instr = "ld (bc),a";
        break;
    case 0x03:
        line->opcode = line->instr = "inc bc";
        break;
    case 0x04:
        line->opcode = line->instr = "inc b";
        break;
    case 0x05:
        line->opcode = line->instr = "dec b";
        break;
    case 0x06:
        line->op1 = peek(addr++);
        line->opcode = "ld b,N";
        line->instr = "ld b,$" + fmt_hex(line->op1, 2);
        break;
    case 0x07:
        line->opcode = line->instr = "rlca";
        break;
    case 0x08:
        line->opcode = line->instr = "ex af,af'";
        break;
    case 0x09:
        line->opcode = line->instr = "add hl,bc";
        break;
    case 0x0a:
        line->opcode = line->instr = "ld a,(bc)";
        break;
    case 0x0b:
        line->opcode = line->instr = "dec bc";
        break;
    case 0x0c:
        line->opcode = line->instr = "inc c";
        break;
    case 0x0d:
        line->opcode = line->instr = "dec c";
        break;
    case 0x0e:
        line->op1 = peek(addr++);
        line->opcode = "ld c,N";
        line->instr = "ld c,$" + fmt_hex(line->op1, 2);
        break;
    case 0x0f:
        line->opcode = line->instr = "rrca";
        break;
    case 0x10:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "djnz NN";
        line->instr = "djnz $" + fmt_hex(line->op1, 4);
        break;
    case 0x11:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld de,NN";
        line->instr = "ld de,$" + fmt_hex(line->op1, 4);
        break;
    case 0x12:
        line->opcode = line->instr = "ld (de),a";
        break;
    case 0x13:
        line->opcode = line->instr = "inc de";
        break;
    case 0x14:
        line->opcode = line->instr = "inc d";
        break;
    case 0x15:
        line->opcode = line->instr = "dec d";
        break;
    case 0x16:
        line->op1 = peek(addr++);
        line->opcode = "ld d,N";
        line->instr = "ld d,$" + fmt_hex(line->op1, 2);
        break;
    case 0x17:
        line->opcode = line->instr = "rla";
        break;
    case 0x18:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "jr NN";
        line->instr = "jr $" + fmt_hex(line->op1, 4);
        break;
    case 0x19:
        line->opcode = line->instr = "add hl,de";
        break;
    case 0x1a:
        line->opcode = line->instr = "ld a,(de)";
        break;
    case 0x1b:
        line->opcode = line->instr = "dec de";
        break;
    case 0x1c:
        line->opcode = line->instr = "inc e";
        break;
    case 0x1d:
        line->opcode = line->instr = "dec e";
        break;
    case 0x1e:
        line->op1 = peek(addr++);
        line->opcode = "ld e,N";
        line->instr = "ld e,$" + fmt_hex(line->op1, 2);
        break;
    case 0x1f:
        line->opcode = line->instr = "rra";
        break;
    case 0x20:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "jr nz,NN";
        line->instr = "jr nz,$" + fmt_hex(line->op1, 4);
        break;
    case 0x21:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld hl,NN";
        line->instr = "ld hl,$" + fmt_hex(line->op1, 4);
        break;
    case 0x22:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),hl";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),hl";
        break;
    case 0x23:
        line->opcode = line->instr = "inc hl";
        break;
    case 0x24:
        line->opcode = line->instr = "inc h";
        break;
    case 0x25:
        line->opcode = line->instr = "dec h";
        break;
    case 0x26:
        line->op1 = peek(addr++);
        line->opcode = "ld h,N";
        line->instr = "ld h,$" + fmt_hex(line->op1, 2);
        break;
    case 0x27:
        line->opcode = line->instr = "daa";
        break;
    case 0x28:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "jr z,NN";
        line->instr = "jr z,$" + fmt_hex(line->op1, 4);
        break;
    case 0x29:
        line->opcode = line->instr = "add hl,hl";
        break;
    case 0x2a:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld hl,(NN)";
        line->instr = "ld hl,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x2b:
        line->opcode = line->instr = "dec hl";
        break;
    case 0x2c:
        line->opcode = line->instr = "inc l";
        break;
    case 0x2d:
        line->opcode = line->instr = "dec l";
        break;
    case 0x2e:
        line->op1 = peek(addr++);
        line->opcode = "ld l,N";
        line->instr = "ld l,$" + fmt_hex(line->op1, 2);
        break;
    case 0x2f:
        line->opcode = line->instr = "cpl";
        break;
    case 0x30:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "jr nc,NN";
        line->instr = "jr nc,$" + fmt_hex(line->op1, 4);
        break;
    case 0x31:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld sp,NN";
        line->instr = "ld sp,$" + fmt_hex(line->op1, 4);
        break;
    case 0x32:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),a";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),a";
        break;
    case 0x33:
        line->opcode = line->instr = "inc sp";
        break;
    case 0x34:
        line->opcode = line->instr = "inc (hl)";
        break;
    case 0x35:
        line->opcode = line->instr = "dec (hl)";
        break;
    case 0x36:
        line->op1 = peek(addr++);
        line->opcode = "ld (hl),N";
        line->instr = "ld (hl),$" + fmt_hex(line->op1, 2);
        break;
    case 0x37:
        line->opcode = line->instr = "scf";
        break;
    case 0x38:
        n = speek(addr++);
        line->op1 = addr + n;
        line->opcode = "jr c,NN";
        line->instr = "jr c,$" + fmt_hex(line->op1, 4);
        break;
    case 0x39:
        line->opcode = line->instr = "add hl,sp";
        break;
    case 0x3a:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld a,(NN)";
        line->instr = "ld a,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x3b:
        line->opcode = line->instr = "dec sp";
        break;
    case 0x3c:
        line->opcode = line->instr = "inc a";
        break;
    case 0x3d:
        line->opcode = line->instr = "dec a";
        break;
    case 0x3e:
        line->op1 = peek(addr++);
        line->opcode = "ld a,N";
        line->instr = "ld a,$" + fmt_hex(line->op1, 2);
        break;
    case 0x3f:
        line->opcode = line->instr = "ccf";
        break;
    case 0x40:
        line->opcode = line->instr = "ld b,b";
        break;
    case 0x41:
        line->opcode = line->instr = "ld b,c";
        break;
    case 0x42:
        line->opcode = line->instr = "ld b,d";
        break;
    case 0x43:
        line->opcode = line->instr = "ld b,e";
        break;
    case 0x44:
        line->opcode = line->instr = "ld b,h";
        break;
    case 0x45:
        line->opcode = line->instr = "ld b,l";
        break;
    case 0x46:
        line->opcode = line->instr = "ld b,(hl)";
        break;
    case 0x47:
        line->opcode = line->instr = "ld b,a";
        break;
    case 0x48:
        line->opcode = line->instr = "ld c,b";
        break;
    case 0x49:
        line->opcode = line->instr = "ld c,c";
        break;
    case 0x4a:
        line->opcode = line->instr = "ld c,d";
        break;
    case 0x4b:
        line->opcode = line->instr = "ld c,e";
        break;
    case 0x4c:
        line->opcode = line->instr = "ld c,h";
        break;
    case 0x4d:
        line->opcode = line->instr = "ld c,l";
        break;
    case 0x4e:
        line->opcode = line->instr = "ld c,(hl)";
        break;
    case 0x4f:
        line->opcode = line->instr = "ld c,a";
        break;
    case 0x50:
        line->opcode = line->instr = "ld d,b";
        break;
    case 0x51:
        line->opcode = line->instr = "ld d,c";
        break;
    case 0x52:
        line->opcode = line->instr = "ld d,d";
        break;
    case 0x53:
        line->opcode = line->instr = "ld d,e";
        break;
    case 0x54:
        line->opcode = line->instr = "ld d,h";
        break;
    case 0x55:
        line->opcode = line->instr = "ld d,l";
        break;
    case 0x56:
        line->opcode = line->instr = "ld d,(hl)";
        break;
    case 0x57:
        line->opcode = line->instr = "ld d,a";
        break;
    case 0x58:
        line->opcode = line->instr = "ld e,b";
        break;
    case 0x59:
        line->opcode = line->instr = "ld e,c";
        break;
    case 0x5a:
        line->opcode = line->instr = "ld e,d";
        break;
    case 0x5b:
        line->opcode = line->instr = "ld e,e";
        break;
    case 0x5c:
        line->opcode = line->instr = "ld e,h";
        break;
    case 0x5d:
        line->opcode = line->instr = "ld e,l";
        break;
    case 0x5e:
        line->opcode = line->instr = "ld e,(hl)";
        break;
    case 0x5f:
        line->opcode = line->instr = "ld e,a";
        break;
    case 0x60:
        line->opcode = line->instr = "ld h,b";
        break;
    case 0x61:
        line->opcode = line->instr = "ld h,c";
        break;
    case 0x62:
        line->opcode = line->instr = "ld h,d";
        break;
    case 0x63:
        line->opcode = line->instr = "ld h,e";
        break;
    case 0x64:
        line->opcode = line->instr = "ld h,h";
        break;
    case 0x65:
        line->opcode = line->instr = "ld h,l";
        break;
    case 0x66:
        line->opcode = line->instr = "ld h,(hl)";
        break;
    case 0x67:
        line->opcode = line->instr = "ld h,a";
        break;
    case 0x68:
        line->opcode = line->instr = "ld l,b";
        break;
    case 0x69:
        line->opcode = line->instr = "ld l,c";
        break;
    case 0x6a:
        line->opcode = line->instr = "ld l,d";
        break;
    case 0x6b:
        line->opcode = line->instr = "ld l,e";
        break;
    case 0x6c:
        line->opcode = line->instr = "ld l,h";
        break;
    case 0x6d:
        line->opcode = line->instr = "ld l,l";
        break;
    case 0x6e:
        line->opcode = line->instr = "ld l,(hl)";
        break;
    case 0x6f:
        line->opcode = line->instr = "ld l,a";
        break;
    case 0x70:
        line->opcode = line->instr = "ld (hl),b";
        break;
    case 0x71:
        line->opcode = line->instr = "ld (hl),c";
        break;
    case 0x72:
        line->opcode = line->instr = "ld (hl),d";
        break;
    case 0x73:
        line->opcode = line->instr = "ld (hl),e";
        break;
    case 0x74:
        line->opcode = line->instr = "ld (hl),h";
        break;
    case 0x75:
        line->opcode = line->instr = "ld (hl),l";
        break;
    case 0x76:
        line->opcode = line->instr = "halt";
        break;
    case 0x77:
        line->opcode = line->instr = "ld (hl),a";
        break;
    case 0x78:
        line->opcode = line->instr = "ld a,b";
        break;
    case 0x79:
        line->opcode = line->instr = "ld a,c";
        break;
    case 0x7a:
        line->opcode = line->instr = "ld a,d";
        break;
    case 0x7b:
        line->opcode = line->instr = "ld a,e";
        break;
    case 0x7c:
        line->opcode = line->instr = "ld a,h";
        break;
    case 0x7d:
        line->opcode = line->instr = "ld a,l";
        break;
    case 0x7e:
        line->opcode = line->instr = "ld a,(hl)";
        break;
    case 0x7f:
        line->opcode = line->instr = "ld a,a";
        break;
    case 0x80:
        line->opcode = line->instr = "add a,b";
        break;
    case 0x81:
        line->opcode = line->instr = "add a,c";
        break;
    case 0x82:
        line->opcode = line->instr = "add a,d";
        break;
    case 0x83:
        line->opcode = line->instr = "add a,e";
        break;
    case 0x84:
        line->opcode = line->instr = "add a,h";
        break;
    case 0x85:
        line->opcode = line->instr = "add a,l";
        break;
    case 0x86:
        line->opcode = line->instr = "add a,(hl)";
        break;
    case 0x87:
        line->opcode = line->instr = "add a,a";
        break;
    case 0x88:
        line->opcode = line->instr = "adc a,b";
        break;
    case 0x89:
        line->opcode = line->instr = "adc a,c";
        break;
    case 0x8a:
        line->opcode = line->instr = "adc a,d";
        break;
    case 0x8b:
        line->opcode = line->instr = "adc a,e";
        break;
    case 0x8c:
        line->opcode = line->instr = "adc a,h";
        break;
    case 0x8d:
        line->opcode = line->instr = "adc a,l";
        break;
    case 0x8e:
        line->opcode = line->instr = "adc a,(hl)";
        break;
    case 0x8f:
        line->opcode = line->instr = "adc a,a";
        break;
    case 0x90:
        line->opcode = line->instr = "sub b";
        break;
    case 0x91:
        line->opcode = line->instr = "sub c";
        break;
    case 0x92:
        line->opcode = line->instr = "sub d";
        break;
    case 0x93:
        line->opcode = line->instr = "sub e";
        break;
    case 0x94:
        line->opcode = line->instr = "sub h";
        break;
    case 0x95:
        line->opcode = line->instr = "sub l";
        break;
    case 0x96:
        line->opcode = line->instr = "sub (hl)";
        break;
    case 0x97:
        line->opcode = line->instr = "sub a";
        break;
    case 0x98:
        line->opcode = line->instr = "sbc a,b";
        break;
    case 0x99:
        line->opcode = line->instr = "sbc a,c";
        break;
    case 0x9a:
        line->opcode = line->instr = "sbc a,d";
        break;
    case 0x9b:
        line->opcode = line->instr = "sbc a,e";
        break;
    case 0x9c:
        line->opcode = line->instr = "sbc a,h";
        break;
    case 0x9d:
        line->opcode = line->instr = "sbc a,l";
        break;
    case 0x9e:
        line->opcode = line->instr = "sbc a,(hl)";
        break;
    case 0x9f:
        line->opcode = line->instr = "sbc a,a";
        break;
    case 0xa0:
        line->opcode = line->instr = "and b";
        break;
    case 0xa1:
        line->opcode = line->instr = "and c";
        break;
    case 0xa2:
        line->opcode = line->instr = "and d";
        break;
    case 0xa3:
        line->opcode = line->instr = "and e";
        break;
    case 0xa4:
        line->opcode = line->instr = "and h";
        break;
    case 0xa5:
        line->opcode = line->instr = "and l";
        break;
    case 0xa6:
        line->opcode = line->instr = "and (hl)";
        break;
    case 0xa7:
        line->opcode = line->instr = "and a";
        break;
    case 0xa8:
        line->opcode = line->instr = "xor b";
        break;
    case 0xa9:
        line->opcode = line->instr = "xor c";
        break;
    case 0xaa:
        line->opcode = line->instr = "xor d";
        break;
    case 0xab:
        line->opcode = line->instr = "xor e";
        break;
    case 0xac:
        line->opcode = line->instr = "xor h";
        break;
    case 0xad:
        line->opcode = line->instr = "xor l";
        break;
    case 0xae:
        line->opcode = line->instr = "xor (hl)";
        break;
    case 0xaf:
        line->opcode = line->instr = "xor a";
        break;
    case 0xb0:
        line->opcode = line->instr = "or b";
        break;
    case 0xb1:
        line->opcode = line->instr = "or c";
        break;
    case 0xb2:
        line->opcode = line->instr = "or d";
        break;
    case 0xb3:
        line->opcode = line->instr = "or e";
        break;
    case 0xb4:
        line->opcode = line->instr = "or h";
        break;
    case 0xb5:
        line->opcode = line->instr = "or l";
        break;
    case 0xb6:
        line->opcode = line->instr = "or (hl)";
        break;
    case 0xb7:
        line->opcode = line->instr = "or a";
        break;
    case 0xb8:
        line->opcode = line->instr = "cp b";
        break;
    case 0xb9:
        line->opcode = line->instr = "cp c";
        break;
    case 0xba:
        line->opcode = line->instr = "cp d";
        break;
    case 0xbb:
        line->opcode = line->instr = "cp e";
        break;
    case 0xbc:
        line->opcode = line->instr = "cp h";
        break;
    case 0xbd:
        line->opcode = line->instr = "cp l";
        break;
    case 0xbe:
        line->opcode = line->instr = "cp (hl)";
        break;
    case 0xbf:
        line->opcode = line->instr = "cp a";
        break;
    case 0xc0:
        line->opcode = line->instr = "ret nz";
        break;
    case 0xc1:
        line->opcode = line->instr = "pop bc";
        break;
    case 0xc2:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp nz,NN";
        line->instr = "jp nz,$" + fmt_hex(line->op1, 4);
        break;
    case 0xc3:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp NN";
        line->instr = "jp $" + fmt_hex(line->op1, 4);
        break;
    case 0xc4:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call nz,NN";
        line->instr = "call nz,$" + fmt_hex(line->op1, 4);
        break;
    case 0xc5:
        line->opcode = line->instr = "push bc";
        break;
    case 0xc6:
        line->op1 = peek(addr++);
        line->opcode = "add a,N";
        line->instr = "add a,$" + fmt_hex(line->op1, 2);
        break;
    case 0xc7:
        line->opcode = line->instr = "rst 0";
        break;
    case 0xc8:
        line->opcode = line->instr = "ret z";
        break;
    case 0xc9:
        line->opcode = line->instr = "ret";
        break;
    case 0xca:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp z,NN";
        line->instr = "jp z,$" + fmt_hex(line->op1, 4);
        break;
    case 0xcb:
        disasm_cb(addr, line);
        break;
    case 0xcc:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call z,NN";
        line->instr = "call z,$" + fmt_hex(line->op1, 4);
        break;
    case 0xcd:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call NN";
        line->instr = "call $" + fmt_hex(line->op1, 4);
        break;
    case 0xce:
        line->op1 = peek(addr++);
        line->opcode = "adc a,N";
        line->instr = "adc a,$" + fmt_hex(line->op1, 2);
        break;
    case 0xcf:
        line->opcode = line->instr = "rst 8";
        break;
    case 0xd0:
        line->opcode = line->instr = "ret nc";
        break;
    case 0xd1:
        line->opcode = line->instr = "pop de";
        break;
    case 0xd2:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp nc,NN";
        line->instr = "jp nc,$" + fmt_hex(line->op1, 4);
        break;
    case 0xd3:
        line->op1 = peek(addr++);
        line->opcode = "out N,a";
        line->instr = "out $" + fmt_hex(line->op1, 2) + ",a";
        break;
    case 0xd4:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call nc,NN";
        line->instr = "call nc,$" + fmt_hex(line->op1, 4);
        break;
    case 0xd5:
        line->opcode = line->instr = "push de";
        break;
    case 0xd6:
        line->op1 = peek(addr++);
        line->opcode = "sub N";
        line->instr = "sub $" + fmt_hex(line->op1, 2);
        break;
    case 0xd7:
        line->opcode = line->instr = "rst $10";
        break;
    case 0xd8:
        line->opcode = line->instr = "ret c";
        break;
    case 0xd9:
        line->opcode = line->instr = "exx";
        break;
    case 0xda:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp c,NN";
        line->instr = "jp c,$" + fmt_hex(line->op1, 4);
        break;
    case 0xdb:
        line->op1 = peek(addr++);
        line->opcode = "in a,N";
        line->instr = "in a,$" + fmt_hex(line->op1, 2);
        break;
    case 0xdc:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call c,NN";
        line->instr = "call c,$" + fmt_hex(line->op1, 4);
        break;
    case 0xdd:
        disasm_x(addr, line, "ix");
        break;
    case 0xde:
        line->op1 = peek(addr++);
        line->opcode = "sbc a,N";
        line->instr = "sbc a,$" + fmt_hex(line->op1, 2);
        break;
    case 0xdf:
        line->opcode = line->instr = "rst $18";
        break;
    case 0xe0:
        line->opcode = line->instr = "ret po";
        break;
    case 0xe1:
        line->opcode = line->instr = "pop hl";
        break;
    case 0xe2:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp po,NN";
        line->instr = "jp po,$" + fmt_hex(line->op1, 4);
        break;
    case 0xe3:
        line->opcode = line->instr = "ex (sp),hl";
        break;
    case 0xe4:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call po,NN";
        line->instr = "call po,$" + fmt_hex(line->op1, 4);
        break;
    case 0xe5:
        line->opcode = line->instr = "push hl";
        break;
    case 0xe6:
        line->op1 = peek(addr++);
        line->opcode = "and N";
        line->instr = "and $" + fmt_hex(line->op1, 2);
        break;
    case 0xe7:
        line->opcode = line->instr = "rst $20";
        break;
    case 0xe8:
        line->opcode = line->instr = "ret pe";
        break;
    case 0xe9:
        line->opcode = line->instr = "jp (hl)";
        break;
    case 0xea:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp pe,NN";
        line->instr = "jp pe,$" + fmt_hex(line->op1, 4);
        break;
    case 0xeb:
        line->opcode = line->instr = "ex de,hl";
        break;
    case 0xec:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call pe,NN";
        line->instr = "call pe,$" + fmt_hex(line->op1, 4);
        break;
    case 0xed:
        disasm_ed(addr, line);
        break;
    case 0xee:
        line->op1 = peek(addr++);
        line->opcode = "xor N";
        line->instr = "xor $" + fmt_hex(line->op1, 2);
        break;
    case 0xef:
        line->opcode = line->instr = "rst $28";
        break;
    case 0xf0:
        line->opcode = line->instr = "ret p";
        break;
    case 0xf1:
        line->opcode = line->instr = "pop af";
        break;
    case 0xf2:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp p,NN";
        line->instr = "jp p,$" + fmt_hex(line->op1, 4);
        break;
    case 0xf3:
        line->opcode = line->instr = "di";
        break;
    case 0xf4:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call p,NN";
        line->instr = "call p,$" + fmt_hex(line->op1, 4);
        break;
    case 0xf5:
        line->opcode = line->instr = "push af";
        break;
    case 0xf6:
        line->op1 = peek(addr++);
        line->opcode = "or N";
        line->instr = "or $" + fmt_hex(line->op1, 2);
        break;
    case 0xf7:
        line->opcode = line->instr = "rst $30";
        break;
    case 0xf8:
        line->opcode = line->instr = "ret m";
        break;
    case 0xf9:
        line->opcode = line->instr = "ld sp,hl";
        break;
    case 0xfa:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "jp m,NN";
        line->instr = "jp m,$" + fmt_hex(line->op1, 4);
        break;
    case 0xfb:
        line->opcode = line->instr = "ei";
        break;
    case 0xfc:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "call m,NN";
        line->instr = "call m,$" + fmt_hex(line->op1, 4);
        break;
    case 0xfd:
        disasm_x(addr, line, "iy");
        break;
    case 0xfe:
        line->op1 = peek(addr++);
        line->opcode = "cp N";
        line->instr = "cp $" + fmt_hex(line->op1, 2);
        break;
    case 0xff:
        line->opcode = line->instr = "rst $38";
        break;
    default:
        assert(0);
    }

    line->size = addr - line->addr;
    return line;
}

void ZX81::disasm_cb(int& addr, AsmLine* line) {
    switch (peek(addr++)) {
    case 0x00:
        line->opcode = line->instr = "rlc b";
        break;
    case 0x01:
        line->opcode = line->instr = "rlc c";
        break;
    case 0x02:
        line->opcode = line->instr = "rlc d";
        break;
    case 0x03:
        line->opcode = line->instr = "rlc e";
        break;
    case 0x04:
        line->opcode = line->instr = "rlc h";
        break;
    case 0x05:
        line->opcode = line->instr = "rlc l";
        break;
    case 0x06:
        line->opcode = line->instr = "rlc (hl)";
        break;
    case 0x07:
        line->opcode = line->instr = "rlc a";
        break;
    case 0x08:
        line->opcode = line->instr = "rrc b";
        break;
    case 0x09:
        line->opcode = line->instr = "rrc c";
        break;
    case 0x0a:
        line->opcode = line->instr = "rrc d";
        break;
    case 0x0b:
        line->opcode = line->instr = "rrc e";
        break;
    case 0x0c:
        line->opcode = line->instr = "rrc h";
        break;
    case 0x0d:
        line->opcode = line->instr = "rrc l";
        break;
    case 0x0e:
        line->opcode = line->instr = "rrc (hl)";
        break;
    case 0x0f:
        line->opcode = line->instr = "rrc a";
        break;
    case 0x10:
        line->opcode = line->instr = "rl b";
        break;
    case 0x11:
        line->opcode = line->instr = "rl c";
        break;
    case 0x12:
        line->opcode = line->instr = "rl d";
        break;
    case 0x13:
        line->opcode = line->instr = "rl e";
        break;
    case 0x14:
        line->opcode = line->instr = "rl h";
        break;
    case 0x15:
        line->opcode = line->instr = "rl l";
        break;
    case 0x16:
        line->opcode = line->instr = "rl (hl)";
        break;
    case 0x17:
        line->opcode = line->instr = "rl a";
        break;
    case 0x18:
        line->opcode = line->instr = "rr b";
        break;
    case 0x19:
        line->opcode = line->instr = "rr c";
        break;
    case 0x1a:
        line->opcode = line->instr = "rr d";
        break;
    case 0x1b:
        line->opcode = line->instr = "rr e";
        break;
    case 0x1c:
        line->opcode = line->instr = "rr h";
        break;
    case 0x1d:
        line->opcode = line->instr = "rr l";
        break;
    case 0x1e:
        line->opcode = line->instr = "rr (hl)";
        break;
    case 0x1f:
        line->opcode = line->instr = "rr a";
        break;
    case 0x20:
        line->opcode = line->instr = "sla b";
        break;
    case 0x21:
        line->opcode = line->instr = "sla c";
        break;
    case 0x22:
        line->opcode = line->instr = "sla d";
        break;
    case 0x23:
        line->opcode = line->instr = "sla e";
        break;
    case 0x24:
        line->opcode = line->instr = "sla h";
        break;
    case 0x25:
        line->opcode = line->instr = "sla l";
        break;
    case 0x26:
        line->opcode = line->instr = "sla (hl)";
        break;
    case 0x27:
        line->opcode = line->instr = "sla a";
        break;
    case 0x28:
        line->opcode = line->instr = "sra b";
        break;
    case 0x29:
        line->opcode = line->instr = "sra c";
        break;
    case 0x2a:
        line->opcode = line->instr = "sra d";
        break;
    case 0x2b:
        line->opcode = line->instr = "sra e";
        break;
    case 0x2c:
        line->opcode = line->instr = "sra h";
        break;
    case 0x2d:
        line->opcode = line->instr = "sra l";
        break;
    case 0x2e:
        line->opcode = line->instr = "sra (hl)";
        break;
    case 0x2f:
        line->opcode = line->instr = "sra a";
        break;
    case 0x30:
        goto error;
    case 0x31:
        goto error;
    case 0x32:
        goto error;
    case 0x33:
        goto error;
    case 0x34:
        goto error;
    case 0x35:
        goto error;
    case 0x36:
        goto error;
    case 0x37:
        goto error;
    case 0x38:
        line->opcode = line->instr = "srl b";
        break;
    case 0x39:
        line->opcode = line->instr = "srl c";
        break;
    case 0x3a:
        line->opcode = line->instr = "srl d";
        break;
    case 0x3b:
        line->opcode = line->instr = "srl e";
        break;
    case 0x3c:
        line->opcode = line->instr = "srl h";
        break;
    case 0x3d:
        line->opcode = line->instr = "srl l";
        break;
    case 0x3e:
        line->opcode = line->instr = "srl (hl)";
        break;
    case 0x3f:
        line->opcode = line->instr = "srl a";
        break;
    case 0x40:
        line->opcode = line->instr = "bit 0,b";
        break;
    case 0x41:
        line->opcode = line->instr = "bit 0,c";
        break;
    case 0x42:
        line->opcode = line->instr = "bit 0,d";
        break;
    case 0x43:
        line->opcode = line->instr = "bit 0,e";
        break;
    case 0x44:
        line->opcode = line->instr = "bit 0,h";
        break;
    case 0x45:
        line->opcode = line->instr = "bit 0,l";
        break;
    case 0x46:
        line->opcode = line->instr = "bit 0,(hl)";
        break;
    case 0x47:
        line->opcode = line->instr = "bit 0,a";
        break;
    case 0x48:
        line->opcode = line->instr = "bit 1,b";
        break;
    case 0x49:
        line->opcode = line->instr = "bit 1,c";
        break;
    case 0x4a:
        line->opcode = line->instr = "bit 1,d";
        break;
    case 0x4b:
        line->opcode = line->instr = "bit 1,e";
        break;
    case 0x4c:
        line->opcode = line->instr = "bit 1,h";
        break;
    case 0x4d:
        line->opcode = line->instr = "bit 1,l";
        break;
    case 0x4e:
        line->opcode = line->instr = "bit 1,(hl)";
        break;
    case 0x4f:
        line->opcode = line->instr = "bit 1,a";
        break;
    case 0x50:
        line->opcode = line->instr = "bit 2,b";
        break;
    case 0x51:
        line->opcode = line->instr = "bit 2,c";
        break;
    case 0x52:
        line->opcode = line->instr = "bit 2,d";
        break;
    case 0x53:
        line->opcode = line->instr = "bit 2,e";
        break;
    case 0x54:
        line->opcode = line->instr = "bit 2,h";
        break;
    case 0x55:
        line->opcode = line->instr = "bit 2,l";
        break;
    case 0x56:
        line->opcode = line->instr = "bit 2,(hl)";
        break;
    case 0x57:
        line->opcode = line->instr = "bit 2,a";
        break;
    case 0x58:
        line->opcode = line->instr = "bit 3,b";
        break;
    case 0x59:
        line->opcode = line->instr = "bit 3,c";
        break;
    case 0x5a:
        line->opcode = line->instr = "bit 3,d";
        break;
    case 0x5b:
        line->opcode = line->instr = "bit 3,e";
        break;
    case 0x5c:
        line->opcode = line->instr = "bit 3,h";
        break;
    case 0x5d:
        line->opcode = line->instr = "bit 3,l";
        break;
    case 0x5e:
        line->opcode = line->instr = "bit 3,(hl)";
        break;
    case 0x5f:
        line->opcode = line->instr = "bit 3,a";
        break;
    case 0x60:
        line->opcode = line->instr = "bit 4,b";
        break;
    case 0x61:
        line->opcode = line->instr = "bit 4,c";
        break;
    case 0x62:
        line->opcode = line->instr = "bit 4,d";
        break;
    case 0x63:
        line->opcode = line->instr = "bit 4,e";
        break;
    case 0x64:
        line->opcode = line->instr = "bit 4,h";
        break;
    case 0x65:
        line->opcode = line->instr = "bit 4,l";
        break;
    case 0x66:
        line->opcode = line->instr = "bit 4,(hl)";
        break;
    case 0x67:
        line->opcode = line->instr = "bit 4,a";
        break;
    case 0x68:
        line->opcode = line->instr = "bit 5,b";
        break;
    case 0x69:
        line->opcode = line->instr = "bit 5,c";
        break;
    case 0x6a:
        line->opcode = line->instr = "bit 5,d";
        break;
    case 0x6b:
        line->opcode = line->instr = "bit 5,e";
        break;
    case 0x6c:
        line->opcode = line->instr = "bit 5,h";
        break;
    case 0x6d:
        line->opcode = line->instr = "bit 5,l";
        break;
    case 0x6e:
        line->opcode = line->instr = "bit 5,(hl)";
        break;
    case 0x6f:
        line->opcode = line->instr = "bit 5,a";
        break;
    case 0x70:
        line->opcode = line->instr = "bit 6,b";
        break;
    case 0x71:
        line->opcode = line->instr = "bit 6,c";
        break;
    case 0x72:
        line->opcode = line->instr = "bit 6,d";
        break;
    case 0x73:
        line->opcode = line->instr = "bit 6,e";
        break;
    case 0x74:
        line->opcode = line->instr = "bit 6,h";
        break;
    case 0x75:
        line->opcode = line->instr = "bit 6,l";
        break;
    case 0x76:
        line->opcode = line->instr = "bit 6,(hl)";
        break;
    case 0x77:
        line->opcode = line->instr = "bit 6,a";
        break;
    case 0x78:
        line->opcode = line->instr = "bit 7,b";
        break;
    case 0x79:
        line->opcode = line->instr = "bit 7,c";
        break;
    case 0x7a:
        line->opcode = line->instr = "bit 7,d";
        break;
    case 0x7b:
        line->opcode = line->instr = "bit 7,e";
        break;
    case 0x7c:
        line->opcode = line->instr = "bit 7,h";
        break;
    case 0x7d:
        line->opcode = line->instr = "bit 7,l";
        break;
    case 0x7e:
        line->opcode = line->instr = "bit 7,(hl)";
        break;
    case 0x7f:
        line->opcode = line->instr = "bit 7,a";
        break;
    case 0x80:
        line->opcode = line->instr = "res 0,b";
        break;
    case 0x81:
        line->opcode = line->instr = "res 0,c";
        break;
    case 0x82:
        line->opcode = line->instr = "res 0,d";
        break;
    case 0x83:
        line->opcode = line->instr = "res 0,e";
        break;
    case 0x84:
        line->opcode = line->instr = "res 0,h";
        break;
    case 0x85:
        line->opcode = line->instr = "res 0,l";
        break;
    case 0x86:
        line->opcode = line->instr = "res 0,(hl)";
        break;
    case 0x87:
        line->opcode = line->instr = "res 0,a";
        break;
    case 0x88:
        line->opcode = line->instr = "res 1,b";
        break;
    case 0x89:
        line->opcode = line->instr = "res 1,c";
        break;
    case 0x8a:
        line->opcode = line->instr = "res 1,d";
        break;
    case 0x8b:
        line->opcode = line->instr = "res 1,e";
        break;
    case 0x8c:
        line->opcode = line->instr = "res 1,h";
        break;
    case 0x8d:
        line->opcode = line->instr = "res 1,l";
        break;
    case 0x8e:
        line->opcode = line->instr = "res 1,(hl)";
        break;
    case 0x8f:
        line->opcode = line->instr = "res 1,a";
        break;
    case 0x90:
        line->opcode = line->instr = "res 2,b";
        break;
    case 0x91:
        line->opcode = line->instr = "res 2,c";
        break;
    case 0x92:
        line->opcode = line->instr = "res 2,d";
        break;
    case 0x93:
        line->opcode = line->instr = "res 2,e";
        break;
    case 0x94:
        line->opcode = line->instr = "res 2,h";
        break;
    case 0x95:
        line->opcode = line->instr = "res 2,l";
        break;
    case 0x96:
        line->opcode = line->instr = "res 2,(hl)";
        break;
    case 0x97:
        line->opcode = line->instr = "res 2,a";
        break;
    case 0x98:
        line->opcode = line->instr = "res 3,b";
        break;
    case 0x99:
        line->opcode = line->instr = "res 3,c";
        break;
    case 0x9a:
        line->opcode = line->instr = "res 3,d";
        break;
    case 0x9b:
        line->opcode = line->instr = "res 3,e";
        break;
    case 0x9c:
        line->opcode = line->instr = "res 3,h";
        break;
    case 0x9d:
        line->opcode = line->instr = "res 3,l";
        break;
    case 0x9e:
        line->opcode = line->instr = "res 3,(hl)";
        break;
    case 0x9f:
        line->opcode = line->instr = "res 3,a";
        break;
    case 0xa0:
        line->opcode = line->instr = "res 4,b";
        break;
    case 0xa1:
        line->opcode = line->instr = "res 4,c";
        break;
    case 0xa2:
        line->opcode = line->instr = "res 4,d";
        break;
    case 0xa3:
        line->opcode = line->instr = "res 4,e";
        break;
    case 0xa4:
        line->opcode = line->instr = "res 4,h";
        break;
    case 0xa5:
        line->opcode = line->instr = "res 4,l";
        break;
    case 0xa6:
        line->opcode = line->instr = "res 4,(hl)";
        break;
    case 0xa7:
        line->opcode = line->instr = "res 4,a";
        break;
    case 0xa8:
        line->opcode = line->instr = "res 5,b";
        break;
    case 0xa9:
        line->opcode = line->instr = "res 5,c";
        break;
    case 0xaa:
        line->opcode = line->instr = "res 5,d";
        break;
    case 0xab:
        line->opcode = line->instr = "res 5,e";
        break;
    case 0xac:
        line->opcode = line->instr = "res 5,h";
        break;
    case 0xad:
        line->opcode = line->instr = "res 5,l";
        break;
    case 0xae:
        line->opcode = line->instr = "res 5,(hl)";
        break;
    case 0xaf:
        line->opcode = line->instr = "res 5,a";
        break;
    case 0xb0:
        line->opcode = line->instr = "res 6,b";
        break;
    case 0xb1:
        line->opcode = line->instr = "res 6,c";
        break;
    case 0xb2:
        line->opcode = line->instr = "res 6,d";
        break;
    case 0xb3:
        line->opcode = line->instr = "res 6,e";
        break;
    case 0xb4:
        line->opcode = line->instr = "res 6,h";
        break;
    case 0xb5:
        line->opcode = line->instr = "res 6,l";
        break;
    case 0xb6:
        line->opcode = line->instr = "res 6,(hl)";
        break;
    case 0xb7:
        line->opcode = line->instr = "res 6,a";
        break;
    case 0xb8:
        line->opcode = line->instr = "res 7,b";
        break;
    case 0xb9:
        line->opcode = line->instr = "res 7,c";
        break;
    case 0xba:
        line->opcode = line->instr = "res 7,d";
        break;
    case 0xbb:
        line->opcode = line->instr = "res 7,e";
        break;
    case 0xbc:
        line->opcode = line->instr = "res 7,h";
        break;
    case 0xbd:
        line->opcode = line->instr = "res 7,l";
        break;
    case 0xbe:
        line->opcode = line->instr = "res 7,(hl)";
        break;
    case 0xbf:
        line->opcode = line->instr = "res 7,a";
        break;
    case 0xc0:
        line->opcode = line->instr = "set 0,b";
        break;
    case 0xc1:
        line->opcode = line->instr = "set 0,c";
        break;
    case 0xc2:
        line->opcode = line->instr = "set 0,d";
        break;
    case 0xc3:
        line->opcode = line->instr = "set 0,e";
        break;
    case 0xc4:
        line->opcode = line->instr = "set 0,h";
        break;
    case 0xc5:
        line->opcode = line->instr = "set 0,l";
        break;
    case 0xc6:
        line->opcode = line->instr = "set 0,(hl)";
        break;
    case 0xc7:
        line->opcode = line->instr = "set 0,a";
        break;
    case 0xc8:
        line->opcode = line->instr = "set 1,b";
        break;
    case 0xc9:
        line->opcode = line->instr = "set 1,c";
        break;
    case 0xca:
        line->opcode = line->instr = "set 1,d";
        break;
    case 0xcb:
        line->opcode = line->instr = "set 1,e";
        break;
    case 0xcc:
        line->opcode = line->instr = "set 1,h";
        break;
    case 0xcd:
        line->opcode = line->instr = "set 1,l";
        break;
    case 0xce:
        line->opcode = line->instr = "set 1,(hl)";
        break;
    case 0xcf:
        line->opcode = line->instr = "set 1,a";
        break;
    case 0xd0:
        line->opcode = line->instr = "set 2,b";
        break;
    case 0xd1:
        line->opcode = line->instr = "set 2,c";
        break;
    case 0xd2:
        line->opcode = line->instr = "set 2,d";
        break;
    case 0xd3:
        line->opcode = line->instr = "set 2,e";
        break;
    case 0xd4:
        line->opcode = line->instr = "set 2,h";
        break;
    case 0xd5:
        line->opcode = line->instr = "set 2,l";
        break;
    case 0xd6:
        line->opcode = line->instr = "set 2,(hl)";
        break;
    case 0xd7:
        line->opcode = line->instr = "set 2,a";
        break;
    case 0xd8:
        line->opcode = line->instr = "set 3,b";
        break;
    case 0xd9:
        line->opcode = line->instr = "set 3,c";
        break;
    case 0xda:
        line->opcode = line->instr = "set 3,d";
        break;
    case 0xdb:
        line->opcode = line->instr = "set 3,e";
        break;
    case 0xdc:
        line->opcode = line->instr = "set 3,h";
        break;
    case 0xdd:
        line->opcode = line->instr = "set 3,l";
        break;
    case 0xde:
        line->opcode = line->instr = "set 3,(hl)";
        break;
    case 0xdf:
        line->opcode = line->instr = "set 3,a";
        break;
    case 0xe0:
        line->opcode = line->instr = "set 4,b";
        break;
    case 0xe1:
        line->opcode = line->instr = "set 4,c";
        break;
    case 0xe2:
        line->opcode = line->instr = "set 4,d";
        break;
    case 0xe3:
        line->opcode = line->instr = "set 4,e";
        break;
    case 0xe4:
        line->opcode = line->instr = "set 4,h";
        break;
    case 0xe5:
        line->opcode = line->instr = "set 4,l";
        break;
    case 0xe6:
        line->opcode = line->instr = "set 4,(hl)";
        break;
    case 0xe7:
        line->opcode = line->instr = "set 4,a";
        break;
    case 0xe8:
        line->opcode = line->instr = "set 5,b";
        break;
    case 0xe9:
        line->opcode = line->instr = "set 5,c";
        break;
    case 0xea:
        line->opcode = line->instr = "set 5,d";
        break;
    case 0xeb:
        line->opcode = line->instr = "set 5,e";
        break;
    case 0xec:
        line->opcode = line->instr = "set 5,h";
        break;
    case 0xed:
        line->opcode = line->instr = "set 5,l";
        break;
    case 0xee:
        line->opcode = line->instr = "set 5,(hl)";
        break;
    case 0xef:
        line->opcode = line->instr = "set 5,a";
        break;
    case 0xf0:
        line->opcode = line->instr = "set 6,b";
        break;
    case 0xf1:
        line->opcode = line->instr = "set 6,c";
        break;
    case 0xf2:
        line->opcode = line->instr = "set 6,d";
        break;
    case 0xf3:
        line->opcode = line->instr = "set 6,e";
        break;
    case 0xf4:
        line->opcode = line->instr = "set 6,h";
        break;
    case 0xf5:
        line->opcode = line->instr = "set 6,l";
        break;
    case 0xf6:
        line->opcode = line->instr = "set 6,(hl)";
        break;
    case 0xf7:
        line->opcode = line->instr = "set 6,a";
        break;
    case 0xf8:
        line->opcode = line->instr = "set 7,b";
        break;
    case 0xf9:
        line->opcode = line->instr = "set 7,c";
        break;
    case 0xfa:
        line->opcode = line->instr = "set 7,d";
        break;
    case 0xfb:
        line->opcode = line->instr = "set 7,e";
        break;
    case 0xfc:
        line->opcode = line->instr = "set 7,h";
        break;
    case 0xfd:
        line->opcode = line->instr = "set 7,l";
        break;
    case 0xfe:
        line->opcode = line->instr = "set 7,(hl)";
        break;
    case 0xff:
        line->opcode = line->instr = "set 7,a";
        break;
    default:
        assert(0);
    }
    return;

error:
    ERROR("unknown opcode at $" << fmt_hex(addr - 2));
}

void ZX81::disasm_ed(int& addr, AsmLine* line) {
    switch (peek(addr++)) {
    case 0x00:
        goto error;
    case 0x01:
        goto error;
    case 0x02:
        goto error;
    case 0x03:
        goto error;
    case 0x04:
        goto error;
    case 0x05:
        goto error;
    case 0x06:
        goto error;
    case 0x07:
        goto error;
    case 0x08:
        goto error;
    case 0x09:
        goto error;
    case 0x0a:
        goto error;
    case 0x0b:
        goto error;
    case 0x0c:
        goto error;
    case 0x0d:
        goto error;
    case 0x0e:
        goto error;
    case 0x0f:
        goto error;
    case 0x10:
        goto error;
    case 0x11:
        goto error;
    case 0x12:
        goto error;
    case 0x13:
        goto error;
    case 0x14:
        goto error;
    case 0x15:
        goto error;
    case 0x16:
        goto error;
    case 0x17:
        goto error;
    case 0x18:
        goto error;
    case 0x19:
        goto error;
    case 0x1a:
        goto error;
    case 0x1b:
        goto error;
    case 0x1c:
        goto error;
    case 0x1d:
        goto error;
    case 0x1e:
        goto error;
    case 0x1f:
        goto error;
    case 0x20:
        goto error;
    case 0x21:
        goto error;
    case 0x22:
        goto error;
    case 0x23:
        goto error;
    case 0x24:
        goto error;
    case 0x25:
        goto error;
    case 0x26:
        goto error;
    case 0x27:
        goto error;
    case 0x28:
        goto error;
    case 0x29:
        goto error;
    case 0x2a:
        goto error;
    case 0x2b:
        goto error;
    case 0x2c:
        goto error;
    case 0x2d:
        goto error;
    case 0x2e:
        goto error;
    case 0x2f:
        goto error;
    case 0x30:
        goto error;
    case 0x31:
        goto error;
    case 0x32:
        goto error;
    case 0x33:
        goto error;
    case 0x34:
        goto error;
    case 0x35:
        goto error;
    case 0x36:
        goto error;
    case 0x37:
        goto error;
    case 0x38:
        goto error;
    case 0x39:
        goto error;
    case 0x3a:
        goto error;
    case 0x3b:
        goto error;
    case 0x3c:
        goto error;
    case 0x3d:
        goto error;
    case 0x3e:
        goto error;
    case 0x3f:
        goto error;
    case 0x40:
        line->opcode = line->instr = "in b,(c)";
        break;
    case 0x41:
        line->opcode = line->instr = "out (c),b";
        break;
    case 0x42:
        line->opcode = line->instr = "sbc hl,bc";
        break;
    case 0x43:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),bc";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),bc";
        break;
    case 0x44:
        line->opcode = line->instr = "neg";
        break;
    case 0x45:
        line->opcode = line->instr = "retn";
        break;
    case 0x46:
        line->opcode = line->instr = "im 0";
        break;
    case 0x47:
        line->opcode = line->instr = "ld i,a";
        break;
    case 0x48:
        line->opcode = line->instr = "in c,(c)";
        break;
    case 0x49:
        line->opcode = line->instr = "out (c),c";
        break;
    case 0x4a:
        line->opcode = line->instr = "adc hl,bc";
        break;
    case 0x4b:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld bc,(NN)";
        line->instr = "ld bc,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x4c:
        goto error;
    case 0x4d:
        line->opcode = line->instr = "reti";
        break;
    case 0x4e:
        goto error;
    case 0x4f:
        line->opcode = line->instr = "ld r,a";
        break;
    case 0x50:
        line->opcode = line->instr = "in d,(c)";
        break;
    case 0x51:
        line->opcode = line->instr = "out (c),d";
        break;
    case 0x52:
        line->opcode = line->instr = "sbc hl,de";
        break;
    case 0x53:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),de";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),de";
        break;
    case 0x54:
        goto error;
    case 0x55:
        goto error;
    case 0x56:
        line->opcode = line->instr = "im 1";
        break;
    case 0x57:
        line->opcode = line->instr = "ld a,i";
        break;
    case 0x58:
        line->opcode = line->instr = "in e,(c)";
        break;
    case 0x59:
        line->opcode = line->instr = "out (c),e";
        break;
    case 0x5a:
        line->opcode = line->instr = "adc hl,de";
        break;
    case 0x5b:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld de,(NN)";
        line->instr = "ld de,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x5c:
        goto error;
    case 0x5d:
        goto error;
    case 0x5e:
        line->opcode = line->instr = "im 2";
        break;
    case 0x5f:
        line->opcode = line->instr = "ld a,r";
        break;
    case 0x60:
        line->opcode = line->instr = "in h,(c)";
        break;
    case 0x61:
        line->opcode = line->instr = "out (c),h";
        break;
    case 0x62:
        line->opcode = line->instr = "sbc hl,hl";
        break;
    case 0x63:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),hl";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),hl";
        break;
    case 0x64:
        goto error;
    case 0x65:
        goto error;
    case 0x66:
        goto error;
    case 0x67:
        line->opcode = line->instr = "rrd";
        break;
    case 0x68:
        line->opcode = line->instr = "in l,(c)";
        break;
    case 0x69:
        line->opcode = line->instr = "out (c),l";
        break;
    case 0x6a:
        line->opcode = line->instr = "adc hl,hl";
        break;
    case 0x6b:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld hl,(NN)";
        line->instr = "ld hl,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x6c:
        goto error;
    case 0x6d:
        goto error;
    case 0x6e:
        goto error;
    case 0x6f:
        line->opcode = line->instr = "rld";
        break;
    case 0x70:
        goto error;
    case 0x71:
        goto error;
    case 0x72:
        line->opcode = line->instr = "sbc hl,sp";
        break;
    case 0x73:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN),sp";
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + "),sp";
        break;
    case 0x74:
        goto error;
    case 0x75:
        goto error;
    case 0x76:
        goto error;
    case 0x77:
        goto error;
    case 0x78:
        line->opcode = line->instr = "in a,(c)";
        break;
    case 0x79:
        line->opcode = line->instr = "out (c),a";
        break;
    case 0x7a:
        line->opcode = line->instr = "adc hl,sp";
        break;
    case 0x7b:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld sp,(NN)";
        line->instr = "ld sp,($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x7c:
        goto error;
    case 0x7d:
        goto error;
    case 0x7e:
        goto error;
    case 0x7f:
        goto error;
    case 0x80:
        goto error;
    case 0x81:
        goto error;
    case 0x82:
        goto error;
    case 0x83:
        goto error;
    case 0x84:
        goto error;
    case 0x85:
        goto error;
    case 0x86:
        goto error;
    case 0x87:
        goto error;
    case 0x88:
        goto error;
    case 0x89:
        goto error;
    case 0x8a:
        goto error;
    case 0x8b:
        goto error;
    case 0x8c:
        goto error;
    case 0x8d:
        goto error;
    case 0x8e:
        goto error;
    case 0x8f:
        goto error;
    case 0x90:
        goto error;
    case 0x91:
        goto error;
    case 0x92:
        goto error;
    case 0x93:
        goto error;
    case 0x94:
        goto error;
    case 0x95:
        goto error;
    case 0x96:
        goto error;
    case 0x97:
        goto error;
    case 0x98:
        goto error;
    case 0x99:
        goto error;
    case 0x9a:
        goto error;
    case 0x9b:
        goto error;
    case 0x9c:
        goto error;
    case 0x9d:
        goto error;
    case 0x9e:
        goto error;
    case 0x9f:
        goto error;
    case 0xa0:
        line->opcode = line->instr = "ldi";
        break;
    case 0xa1:
        line->opcode = line->instr = "cpi";
        break;
    case 0xa2:
        line->opcode = line->instr = "ini";
        break;
    case 0xa3:
        line->opcode = line->instr = "outi";
        break;
    case 0xa4:
        goto error;
    case 0xa5:
        goto error;
    case 0xa6:
        goto error;
    case 0xa7:
        goto error;
    case 0xa8:
        line->opcode = line->instr = "ldd";
        break;
    case 0xa9:
        line->opcode = line->instr = "cpd";
        break;
    case 0xaa:
        line->opcode = line->instr = "ind";
        break;
    case 0xab:
        line->opcode = line->instr = "outd";
        break;
    case 0xac:
        goto error;
    case 0xad:
        goto error;
    case 0xae:
        goto error;
    case 0xaf:
        goto error;
    case 0xb0:
        line->opcode = line->instr = "ldir";
        break;
    case 0xb1:
        line->opcode = line->instr = "cpir";
        break;
    case 0xb2:
        line->opcode = line->instr = "inir";
        break;
    case 0xb3:
        line->opcode = line->instr = "otir";
        break;
    case 0xb4:
        goto error;
    case 0xb5:
        goto error;
    case 0xb6:
        goto error;
    case 0xb7:
        goto error;
    case 0xb8:
        line->opcode = line->instr = "lddr";
        break;
    case 0xb9:
        line->opcode = line->instr = "cpdr";
        break;
    case 0xba:
        line->opcode = line->instr = "indr";
        break;
    case 0xbb:
        line->opcode = line->instr = "otdr";
        break;
    case 0xbc:
        goto error;
    case 0xbd:
        goto error;
    case 0xbe:
        goto error;
    case 0xbf:
        goto error;
    case 0xc0:
        goto error;
    case 0xc1:
        goto error;
    case 0xc2:
        goto error;
    case 0xc3:
        goto error;
    case 0xc4:
        goto error;
    case 0xc5:
        goto error;
    case 0xc6:
        goto error;
    case 0xc7:
        goto error;
    case 0xc8:
        goto error;
    case 0xc9:
        goto error;
    case 0xca:
        goto error;
    case 0xcb:
        goto error;
    case 0xcc:
        goto error;
    case 0xcd:
        goto error;
    case 0xce:
        goto error;
    case 0xcf:
        goto error;
    case 0xd0:
        goto error;
    case 0xd1:
        goto error;
    case 0xd2:
        goto error;
    case 0xd3:
        goto error;
    case 0xd4:
        goto error;
    case 0xd5:
        goto error;
    case 0xd6:
        goto error;
    case 0xd7:
        goto error;
    case 0xd8:
        goto error;
    case 0xd9:
        goto error;
    case 0xda:
        goto error;
    case 0xdb:
        goto error;
    case 0xdc:
        goto error;
    case 0xdd:
        goto error;
    case 0xde:
        goto error;
    case 0xdf:
        goto error;
    case 0xe0:
        goto error;
    case 0xe1:
        goto error;
    case 0xe2:
        goto error;
    case 0xe3:
        goto error;
    case 0xe4:
        goto error;
    case 0xe5:
        goto error;
    case 0xe6:
        goto error;
    case 0xe7:
        goto error;
    case 0xe8:
        goto error;
    case 0xe9:
        goto error;
    case 0xea:
        goto error;
    case 0xeb:
        goto error;
    case 0xec:
        goto error;
    case 0xed:
        goto error;
    case 0xee:
        goto error;
    case 0xef:
        goto error;
    case 0xf0:
        goto error;
    case 0xf1:
        goto error;
    case 0xf2:
        goto error;
    case 0xf3:
        goto error;
    case 0xf4:
        goto error;
    case 0xf5:
        goto error;
    case 0xf6:
        goto error;
    case 0xf7:
        goto error;
    case 0xf8:
        goto error;
    case 0xf9:
        goto error;
    case 0xfa:
        goto error;
    case 0xfb:
        goto error;
    case 0xfc:
        goto error;
    case 0xfd:
        goto error;
    case 0xfe:
        goto error;
    case 0xff:
        goto error;
    default:
        assert(0);
    }
    return;

error:
    ERROR("unknown opcode at $" << fmt_hex(addr - 2));
}

void ZX81::disasm_x(int& addr, AsmLine* line, const string& x) {
    switch (peek(addr++)) {
    case 0x00:
        goto error;
    case 0x01:
        goto error;
    case 0x02:
        goto error;
    case 0x03:
        goto error;
    case 0x04:
        goto error;
    case 0x05:
        goto error;
    case 0x06:
        goto error;
    case 0x07:
        goto error;
    case 0x08:
        goto error;
    case 0x09:
        line->opcode = line->instr = "add " + x + ",bc";
        break;
    case 0x0a:
        goto error;
    case 0x0b:
        goto error;
    case 0x0c:
        goto error;
    case 0x0d:
        goto error;
    case 0x0e:
        goto error;
    case 0x0f:
        goto error;
    case 0x10:
        goto error;
    case 0x11:
        goto error;
    case 0x12:
        goto error;
    case 0x13:
        goto error;
    case 0x14:
        goto error;
    case 0x15:
        goto error;
    case 0x16:
        goto error;
    case 0x17:
        goto error;
    case 0x18:
        goto error;
    case 0x19:
        line->opcode = line->instr = "add " + x + ",de";
        break;
    case 0x1a:
        goto error;
    case 0x1b:
        goto error;
    case 0x1c:
        goto error;
    case 0x1d:
        goto error;
    case 0x1e:
        goto error;
    case 0x1f:
        goto error;
    case 0x20:
        goto error;
    case 0x21:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld " + x + ",NN";
        line->instr = "ld " + x + ",$" + fmt_hex(line->op1, 4);
        break;
    case 0x22:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld (NN)," + x;
        line->instr = "ld ($" + fmt_hex(line->op1, 4) + ")," + x;
        break;
    case 0x23:
        line->opcode = line->instr = "inc " + x;
        break;
    case 0x24:
        goto error;
    case 0x25:
        goto error;
    case 0x26:
        goto error;
    case 0x27:
        goto error;
    case 0x28:
        goto error;
    case 0x29:
        line->opcode = line->instr = "add " + x + "," + x;
        break;
    case 0x2a:
        line->op1 = dpeek(addr);
        addr += 2;
        line->opcode = "ld " + x + ",(NN)";
        line->instr = "ld " + x + ",($" + fmt_hex(line->op1, 4) + ")";
        break;
    case 0x2b:
        line->opcode = line->instr = "dec " + x;
        break;
    case 0x2c:
        goto error;
    case 0x2d:
        goto error;
    case 0x2e:
        goto error;
    case 0x2f:
        goto error;
    case 0x30:
        goto error;
    case 0x31:
        goto error;
    case 0x32:
        goto error;
    case 0x33:
        goto error;
    case 0x34:
        line->op1 = speek(addr++);
        line->opcode = "inc (" + x + "+d)";
        line->instr = "inc (" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x35:
        line->op1 = speek(addr++);
        line->opcode = "dec (" + x + "+d)";
        line->instr = "dec (" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x36:
        line->op1 = speek(addr++);
        line->op2 = peek(addr++);
        line->opcode = "ld (" + x + "+d),N";
        line->instr = "ld (" + x + "+" + to_string(line->op1) + "),$" + fmt_hex(
                          line->op2, 2);
        break;
    case 0x37:
        goto error;
    case 0x38:
        goto error;
    case 0x39:
        line->opcode = line->instr = "add " + x + ",sp";
        break;
    case 0x3a:
        goto error;
    case 0x3b:
        goto error;
    case 0x3c:
        goto error;
    case 0x3d:
        goto error;
    case 0x3e:
        goto error;
    case 0x3f:
        goto error;
    case 0x40:
        goto error;
    case 0x41:
        goto error;
    case 0x42:
        goto error;
    case 0x43:
        goto error;
    case 0x44:
        goto error;
    case 0x45:
        goto error;
    case 0x46:
        line->op1 = speek(addr++);
        line->opcode = "ld b,(" + x + "+d)";
        line->instr = "ld b,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x47:
        goto error;
    case 0x48:
        goto error;
    case 0x49:
        goto error;
    case 0x4a:
        goto error;
    case 0x4b:
        goto error;
    case 0x4c:
        goto error;
    case 0x4d:
        goto error;
    case 0x4e:
        line->op1 = speek(addr++);
        line->opcode = "ld c,(" + x + "+d)";
        line->instr = "ld c,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x4f:
        goto error;
    case 0x50:
        goto error;
    case 0x51:
        goto error;
    case 0x52:
        goto error;
    case 0x53:
        goto error;
    case 0x54:
        goto error;
    case 0x55:
        goto error;
    case 0x56:
        line->op1 = speek(addr++);
        line->opcode = "ld d,(" + x + "+d)";
        line->instr = "ld d,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x57:
        goto error;
    case 0x58:
        goto error;
    case 0x59:
        goto error;
    case 0x5a:
        goto error;
    case 0x5b:
        goto error;
    case 0x5c:
        goto error;
    case 0x5d:
        goto error;
    case 0x5e:
        line->op1 = speek(addr++);
        line->opcode = "ld e,(" + x + "+d)";
        line->instr = "ld e,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x5f:
        goto error;
    case 0x60:
        goto error;
    case 0x61:
        goto error;
    case 0x62:
        goto error;
    case 0x63:
        goto error;
    case 0x64:
        goto error;
    case 0x65:
        goto error;
    case 0x66:
        line->op1 = speek(addr++);
        line->opcode = "ld h,(" + x + "+d)";
        line->instr = "ld h,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x67:
        goto error;
    case 0x68:
        goto error;
    case 0x69:
        goto error;
    case 0x6a:
        goto error;
    case 0x6b:
        goto error;
    case 0x6c:
        goto error;
    case 0x6d:
        goto error;
    case 0x6e:
        line->op1 = speek(addr++);
        line->opcode = "ld l,(" + x + "+d)";
        line->instr = "ld l,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x6f:
        goto error;
    case 0x70:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),b";
        line->instr = "ld (" + x + "+d),b";
        break;
    case 0x71:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),c";
        line->instr = "ld (" + x + "+d),c";
        break;
    case 0x72:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),d";
        line->instr = "ld (" + x + "+d),d";
        break;
    case 0x73:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),e";
        line->instr = "ld (" + x + "+d),e";
        break;
    case 0x74:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),h";
        line->instr = "ld (" + x + "+d),h";
        break;
    case 0x75:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),l";
        line->instr = "ld (" + x + "+d),l";
        break;
    case 0x76:
        goto error;
    case 0x77:
        line->op1 = speek(addr++);
        line->opcode = "ld (" + x + "+d),a";
        line->instr = "ld (" + x + "+d),a";
        break;
    case 0x78:
        goto error;
    case 0x79:
        goto error;
    case 0x7a:
        goto error;
    case 0x7b:
        goto error;
    case 0x7c:
        goto error;
    case 0x7d:
        goto error;
    case 0x7e:
        line->op1 = speek(addr++);
        line->opcode = "ld a,(" + x + "+d)";
        line->instr = "ld a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x7f:
        goto error;
    case 0x80:
        goto error;
    case 0x81:
        goto error;
    case 0x82:
        goto error;
    case 0x83:
        goto error;
    case 0x84:
        goto error;
    case 0x85:
        goto error;
    case 0x86:
        line->op1 = speek(addr++);
        line->opcode = "add a,(" + x + "+d)";
        line->instr = "add a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x87:
        goto error;
    case 0x88:
        goto error;
    case 0x89:
        goto error;
    case 0x8a:
        goto error;
    case 0x8b:
        goto error;
    case 0x8c:
        goto error;
    case 0x8d:
        goto error;
    case 0x8e:
        line->op1 = speek(addr++);
        line->opcode = "adc a,(" + x + "+d)";
        line->instr = "adc a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x8f:
        goto error;
    case 0x90:
        goto error;
    case 0x91:
        goto error;
    case 0x92:
        goto error;
    case 0x93:
        goto error;
    case 0x94:
        goto error;
    case 0x95:
        goto error;
    case 0x96:
        line->op1 = speek(addr++);
        line->opcode = "sub (" + x + "+d)";
        line->instr = "sub (" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x97:
        goto error;
    case 0x98:
        goto error;
    case 0x99:
        goto error;
    case 0x9a:
        goto error;
    case 0x9b:
        goto error;
    case 0x9c:
        goto error;
    case 0x9d:
        goto error;
    case 0x9e:
        line->op1 = speek(addr++);
        line->opcode = "sbc a,(" + x + "+d)";
        line->instr = "sbc a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0x9f:
        goto error;
    case 0xa0:
        goto error;
    case 0xa1:
        goto error;
    case 0xa2:
        goto error;
    case 0xa3:
        goto error;
    case 0xa4:
        goto error;
    case 0xa5:
        goto error;
    case 0xa6:
        line->op1 = speek(addr++);
        line->opcode = "and a,(" + x + "+d)";
        line->instr = "and a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0xa7:
        goto error;
    case 0xa8:
        goto error;
    case 0xa9:
        goto error;
    case 0xaa:
        goto error;
    case 0xab:
        goto error;
    case 0xac:
        goto error;
    case 0xad:
        goto error;
    case 0xae:
        line->op1 = speek(addr++);
        line->opcode = "xor a,(" + x + "+d)";
        line->instr = "xor a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0xaf:
        goto error;
    case 0xb0:
        goto error;
    case 0xb1:
        goto error;
    case 0xb2:
        goto error;
    case 0xb3:
        goto error;
    case 0xb4:
        goto error;
    case 0xb5:
        goto error;
    case 0xb6:
        line->op1 = speek(addr++);
        line->opcode = "or a,(" + x + "+d)";
        line->instr = "or a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0xb7:
        goto error;
    case 0xb8:
        goto error;
    case 0xb9:
        goto error;
    case 0xba:
        goto error;
    case 0xbb:
        goto error;
    case 0xbc:
        goto error;
    case 0xbd:
        goto error;
    case 0xbe:
        line->op1 = speek(addr++);
        line->opcode = "cp a,(" + x + "+d)";
        line->instr = "cp a,(" + x + "+" + to_string(line->op1) + ")";
        break;
    case 0xbf:
        goto error;
    case 0xc0:
        goto error;
    case 0xc1:
        goto error;
    case 0xc2:
        goto error;
    case 0xc3:
        goto error;
    case 0xc4:
        goto error;
    case 0xc5:
        goto error;
    case 0xc6:
        goto error;
    case 0xc7:
        goto error;
    case 0xc8:
        goto error;
    case 0xc9:
        goto error;
    case 0xca:
        goto error;
    case 0xcb:
        disasm_x_cb(addr, line, x, speek(addr++));
        break;
    case 0xcc:
        goto error;
    case 0xcd:
        goto error;
    case 0xce:
        goto error;
    case 0xcf:
        goto error;
    case 0xd0:
        goto error;
    case 0xd1:
        goto error;
    case 0xd2:
        goto error;
    case 0xd3:
        goto error;
    case 0xd4:
        goto error;
    case 0xd5:
        goto error;
    case 0xd6:
        goto error;
    case 0xd7:
        goto error;
    case 0xd8:
        goto error;
    case 0xd9:
        goto error;
    case 0xda:
        goto error;
    case 0xdb:
        goto error;
    case 0xdc:
        goto error;
    case 0xdd:
        disasm_x(addr, line, "ix");
        break;
    case 0xde:
        goto error;
    case 0xdf:
        goto error;
    case 0xe0:
        goto error;
    case 0xe1:
        line->opcode = line->instr = "pop " + x;
        break;
    case 0xe2:
        goto error;
    case 0xe3:
        line->opcode = line->instr = "ex (sp)," + x;
        break;
    case 0xe4:
        goto error;
    case 0xe5:
        line->opcode = line->instr = "push " + x;
        break;
    case 0xe6:
        goto error;
    case 0xe7:
        goto error;
    case 0xe8:
        goto error;
    case 0xe9:
        goto error;
    case 0xea:
        goto error;
    case 0xeb:
        line->opcode = line->instr = "ex de," + x;
        break;
    case 0xec:
        goto error;
    case 0xed:
        goto error;
    case 0xee:
        goto error;
    case 0xef:
        goto error;
    case 0xf0:
        goto error;
    case 0xf1:
        goto error;
    case 0xf2:
        goto error;
    case 0xf3:
        goto error;
    case 0xf4:
        goto error;
    case 0xf5:
        goto error;
    case 0xf6:
        goto error;
    case 0xf7:
        goto error;
    case 0xf8:
        goto error;
    case 0xf9:
        line->opcode = line->instr = "ld sp," + x;
        break;
    case 0xfa:
        goto error;
    case 0xfb:
        goto error;
    case 0xfc:
        goto error;
    case 0xfd:
        disasm_x(addr, line, "iy");
        break;
    case 0xfe:
        goto error;
    case 0xff:
        goto error;
    default:
        assert(0);
    }
    return;

error:
    ERROR("unknown opcode at $" << fmt_hex(addr - 2));
}

void ZX81::disasm_x_cb(int& addr, AsmLine* line, const string& x, int offset) {
    switch (peek(addr++)) {
    case 0x00:
        goto error;
    case 0x01:
        goto error;
    case 0x02:
        goto error;
    case 0x03:
        goto error;
    case 0x04:
        goto error;
    case 0x05:
        goto error;
    case 0x06:
        line->opcode = "rlc (" + x + "+d)";
        line->instr = "rlc (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x07:
        goto error;
    case 0x08:
        goto error;
    case 0x09:
        goto error;
    case 0x0a:
        goto error;
    case 0x0b:
        goto error;
    case 0x0c:
        goto error;
    case 0x0d:
        goto error;
    case 0x0e:
        line->opcode = "rrc (" + x + "+d)";
        line->instr = "rrc (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x0f:
        goto error;
    case 0x10:
        goto error;
    case 0x11:
        goto error;
    case 0x12:
        goto error;
    case 0x13:
        goto error;
    case 0x14:
        goto error;
    case 0x15:
        goto error;
    case 0x16:
        line->opcode = "rl (" + x + "+d)";
        line->instr = "rl (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x17:
        goto error;
    case 0x18:
        goto error;
    case 0x19:
        goto error;
    case 0x1a:
        goto error;
    case 0x1b:
        goto error;
    case 0x1c:
        goto error;
    case 0x1d:
        goto error;
    case 0x1e:
        line->opcode = "rr (" + x + "+d)";
        line->instr = "rr (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x1f:
        goto error;
    case 0x20:
        goto error;
    case 0x21:
        goto error;
    case 0x22:
        goto error;
    case 0x23:
        goto error;
    case 0x24:
        goto error;
    case 0x25:
        goto error;
    case 0x26:
        line->opcode = "sla (" + x + "+d)";
        line->instr = "sla (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x27:
        goto error;
    case 0x28:
        goto error;
    case 0x29:
        goto error;
    case 0x2a:
        goto error;
    case 0x2b:
        goto error;
    case 0x2c:
        goto error;
    case 0x2d:
        goto error;
    case 0x2e:
        line->opcode = "sra (" + x + "+d)";
        line->instr = "sra (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x2f:
        goto error;
    case 0x30:
        goto error;
    case 0x31:
        goto error;
    case 0x32:
        goto error;
    case 0x33:
        goto error;
    case 0x34:
        goto error;
    case 0x35:
        goto error;
    case 0x36:
        goto error;
    case 0x37:
        goto error;
    case 0x38:
        goto error;
    case 0x39:
        goto error;
    case 0x3a:
        goto error;
    case 0x3b:
        goto error;
    case 0x3c:
        goto error;
    case 0x3d:
        goto error;
    case 0x3e:
        line->opcode = "srl (" + x + "+d)";
        line->instr = "srl (" + x + "+" + to_string(offset) + ")";
        break;
    case 0x3f:
        goto error;
    case 0x40:
        goto error;
    case 0x41:
        goto error;
    case 0x42:
        goto error;
    case 0x43:
        goto error;
    case 0x44:
        goto error;
    case 0x45:
        goto error;
    case 0x46:
        line->opcode = "bit 0,(" + x + "+d)";
        line->instr = "bit 0,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x47:
        goto error;
    case 0x48:
        goto error;
    case 0x49:
        goto error;
    case 0x4a:
        goto error;
    case 0x4b:
        goto error;
    case 0x4c:
        goto error;
    case 0x4d:
        goto error;
    case 0x4e:
        line->opcode = "bit 1,(" + x + "+d)";
        line->instr = "bit 1,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x4f:
        goto error;
    case 0x50:
        goto error;
    case 0x51:
        goto error;
    case 0x52:
        goto error;
    case 0x53:
        goto error;
    case 0x54:
        goto error;
    case 0x55:
        goto error;
    case 0x56:
        line->opcode = "bit 2,(" + x + "+d)";
        line->instr = "bit 2,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x57:
        goto error;
    case 0x58:
        goto error;
    case 0x59:
        goto error;
    case 0x5a:
        goto error;
    case 0x5b:
        goto error;
    case 0x5c:
        goto error;
    case 0x5d:
        goto error;
    case 0x5e:
        line->opcode = "bit 3,(" + x + "+d)";
        line->instr = "bit 3,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x5f:
        goto error;
    case 0x60:
        goto error;
    case 0x61:
        goto error;
    case 0x62:
        goto error;
    case 0x63:
        goto error;
    case 0x64:
        goto error;
    case 0x65:
        goto error;
    case 0x66:
        line->opcode = "bit 4,(" + x + "+d)";
        line->instr = "bit 4,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x67:
        goto error;
    case 0x68:
        goto error;
    case 0x69:
        goto error;
    case 0x6a:
        goto error;
    case 0x6b:
        goto error;
    case 0x6c:
        goto error;
    case 0x6d:
        goto error;
    case 0x6e:
        line->opcode = "bit 5,(" + x + "+d)";
        line->instr = "bit 5,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x6f:
        goto error;
    case 0x70:
        goto error;
    case 0x71:
        goto error;
    case 0x72:
        goto error;
    case 0x73:
        goto error;
    case 0x74:
        goto error;
    case 0x75:
        goto error;
    case 0x76:
        line->opcode = "bit 6,(" + x + "+d)";
        line->instr = "bit 6,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x77:
        goto error;
    case 0x78:
        goto error;
    case 0x79:
        goto error;
    case 0x7a:
        goto error;
    case 0x7b:
        goto error;
    case 0x7c:
        goto error;
    case 0x7d:
        goto error;
    case 0x7e:
        line->opcode = "bit 7,(" + x + "+d)";
        line->instr = "bit 7,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x7f:
        goto error;
    case 0x80:
        goto error;
    case 0x81:
        goto error;
    case 0x82:
        goto error;
    case 0x83:
        goto error;
    case 0x84:
        goto error;
    case 0x85:
        goto error;
    case 0x86:
        line->opcode = "res 0,(" + x + "+d)";
        line->instr = "res 0,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x87:
        goto error;
    case 0x88:
        goto error;
    case 0x89:
        goto error;
    case 0x8a:
        goto error;
    case 0x8b:
        goto error;
    case 0x8c:
        goto error;
    case 0x8d:
        goto error;
    case 0x8e:
        line->opcode = "res 1,(" + x + "+d)";
        line->instr = "res 1,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x8f:
        goto error;
    case 0x90:
        goto error;
    case 0x91:
        goto error;
    case 0x92:
        goto error;
    case 0x93:
        goto error;
    case 0x94:
        goto error;
    case 0x95:
        goto error;
    case 0x96:
        line->opcode = "res 2,(" + x + "+d)";
        line->instr = "res 2,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x97:
        goto error;
    case 0x98:
        goto error;
    case 0x99:
        goto error;
    case 0x9a:
        goto error;
    case 0x9b:
        goto error;
    case 0x9c:
        goto error;
    case 0x9d:
        goto error;
    case 0x9e:
        line->opcode = "res 3,(" + x + "+d)";
        line->instr = "res 3,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0x9f:
        goto error;
    case 0xa0:
        goto error;
    case 0xa1:
        goto error;
    case 0xa2:
        goto error;
    case 0xa3:
        goto error;
    case 0xa4:
        goto error;
    case 0xa5:
        goto error;
    case 0xa6:
        line->opcode = "res 4,(" + x + "+d)";
        line->instr = "res 4,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xa7:
        goto error;
    case 0xa8:
        goto error;
    case 0xa9:
        goto error;
    case 0xaa:
        goto error;
    case 0xab:
        goto error;
    case 0xac:
        goto error;
    case 0xad:
        goto error;
    case 0xae:
        line->opcode = "res 5,(" + x + "+d)";
        line->instr = "res 5,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xaf:
        goto error;
    case 0xb0:
        goto error;
    case 0xb1:
        goto error;
    case 0xb2:
        goto error;
    case 0xb3:
        goto error;
    case 0xb4:
        goto error;
    case 0xb5:
        goto error;
    case 0xb6:
        line->opcode = "res 6,(" + x + "+d)";
        line->instr = "res 6,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xb7:
        goto error;
    case 0xb8:
        goto error;
    case 0xb9:
        goto error;
    case 0xba:
        goto error;
    case 0xbb:
        goto error;
    case 0xbc:
        goto error;
    case 0xbd:
        goto error;
    case 0xbe:
        line->opcode = "res 7,(" + x + "+d)";
        line->instr = "res 7,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xbf:
        goto error;
    case 0xc0:
        goto error;
    case 0xc1:
        goto error;
    case 0xc2:
        goto error;
    case 0xc3:
        goto error;
    case 0xc4:
        goto error;
    case 0xc5:
        goto error;
    case 0xc6:
        line->opcode = "set 0,(" + x + "+d)";
        line->instr = "set 0,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xc7:
        goto error;
    case 0xc8:
        goto error;
    case 0xc9:
        goto error;
    case 0xca:
        goto error;
    case 0xcb:
        goto error;
    case 0xcc:
        goto error;
    case 0xcd:
        goto error;
    case 0xce:
        line->opcode = "set 1,(" + x + "+d)";
        line->instr = "set 1,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xcf:
        goto error;
    case 0xd0:
        goto error;
    case 0xd1:
        goto error;
    case 0xd2:
        goto error;
    case 0xd3:
        goto error;
    case 0xd4:
        goto error;
    case 0xd5:
        goto error;
    case 0xd6:
        line->opcode = "set 2,(" + x + "+d)";
        line->instr = "set 2,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xd7:
        goto error;
    case 0xd8:
        goto error;
    case 0xd9:
        goto error;
    case 0xda:
        goto error;
    case 0xdb:
        goto error;
    case 0xdc:
        goto error;
    case 0xdd:
        goto error;
    case 0xde:
        line->opcode = "set 3,(" + x + "+d)";
        line->instr = "set 3,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xdf:
        goto error;
    case 0xe0:
        goto error;
    case 0xe1:
        goto error;
    case 0xe2:
        goto error;
    case 0xe3:
        goto error;
    case 0xe4:
        goto error;
    case 0xe5:
        goto error;
    case 0xe6:
        line->opcode = "set 4,(" + x + "+d)";
        line->instr = "set 4,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xe7:
        goto error;
    case 0xe8:
        goto error;
    case 0xe9:
        goto error;
    case 0xea:
        goto error;
    case 0xeb:
        goto error;
    case 0xec:
        goto error;
    case 0xed:
        goto error;
    case 0xee:
        line->opcode = "set 5,(" + x + "+d)";
        line->instr = "set 5,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xef:
        goto error;
    case 0xf0:
        goto error;
    case 0xf1:
        goto error;
    case 0xf2:
        goto error;
    case 0xf3:
        goto error;
    case 0xf4:
        goto error;
    case 0xf5:
        goto error;
    case 0xf6:
        line->opcode = "set 6,(" + x + "+d)";
        line->instr = "set 6,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xf7:
        goto error;
    case 0xf8:
        goto error;
    case 0xf9:
        goto error;
    case 0xfa:
        goto error;
    case 0xfb:
        goto error;
    case 0xfc:
        goto error;
    case 0xfd:
        goto error;
    case 0xfe:
        line->opcode = "set 7,(" + x + "+d)";
        line->instr = "set 7,(" + x + "+" + to_string(offset) + ")";
        break;
    case 0xff:
        goto error;
    default:
        assert(0);
    }
    return;

error:
    ERROR("unknown opcode at $" << fmt_hex(addr - 2));
}

//-----------------------------------------------------------------------------
// assemble asm code
//-----------------------------------------------------------------------------

void ZX81::parse_asm_line(const char* p) {
    BasicLine* last_line = &basic_lines.back();
    vector<uint8_t>& bytes = last_line->tokens[1].bytes;
    int addr = last_line->addr + 5 + static_cast<int>
               (bytes.size());	// 0=REM, 1=bytes

    if (isalpha(*p)) {		// column 0 = label
        string name;
        parse_ident(p, name);
        skip_spaces(p);
        match(p, ":");		// optional ':'

        asm_labels.add(name, addr);
    }

    if (match(p, ";")) {	// comment
    }
    else if (match(p, "DEFB")) {
        parse_asm_defb(p, addr, bytes);
    }
    else {
        ERROR("line " << error_line_num << ": cannot parse: " << p);
    }
}

void ZX81::parse_asm_defb(const char* p, int /*addr*/, vector<uint8_t>& bytes) {
    while (true) {
        int value;
        if (!parse_integer(p, value)) {
            goto error;
        }
        bytes.push_back(value & 0xff);
        if (match(p, ",")) {
            continue;
        }
        else if (parse_end(p)) {
            break;
        }
        else {
            goto error;
        }
    }
    return;

error:
    ERROR("line " << error_line_num << ": cannot parse: " << p);
}

