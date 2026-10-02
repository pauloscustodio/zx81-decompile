//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "dis_decode.h"
#include "errors.h"
#include "lexer.h"
#include "model.h"
#include "sysvars.h"
#include "utils.h"
#include "zfloat.h"
#include "zx81chars.h"
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

static void append_token(std::vector<Token>& tokens, const Token& token) {
    std::string prev_token = tokens.empty() ? "" : tokens.back().text;
    bool need_space = false;
    if (!prev_token.empty() && !token.text.empty()) {
        char last_char = prev_token.back();
        char first_char = token.text.front();
        if (std::isalnum(last_char) && std::isalnum(first_char)) {
            need_space = true;
        }
    }
    tokens.push_back(token);
    if (need_space) {
        tokens.back().ws_before = " ";
    }
}

static Token collect_ident(int& addr, Memory& mem) {
    std::string ident;
    while (true) {
        uint8_t byte = mem.peek(addr);
        if (byte >= CH_0 && byte <= CH_Z) {
            ident += decode_zx81_char(byte);
            addr++;
        }
        else {
            break;
        }
    }
    return Token(TokenType::Identifier, ident);
}

static double decode_zx81_number(int& addr, Memory& mem) {
    std::array<uint8_t, 5> encoded_number = { 0 };
    for (int i = 0; i < 5; ++i) {
        encoded_number[i] = mem.peek(addr + i);
    }
    addr += 5;
    return zx81_to_float(encoded_number);
}

static Token collect_number(int& addr, Memory& mem) {
    std::string number;
    bool has_dot = false;
    bool has_e = false;

    while (true) {
        uint8_t byte = mem.peek(addr);
        std::string ch = decode_zx81_char(byte);

        if (!has_dot && byte == CH_DOT) {
            has_dot = true;
            number += ch;
            addr++;
            continue;
        }

        if (!has_e && byte == CH_E) {
            has_e = true;
            number += ch;
            addr++;

            uint8_t sign_byte = mem.peek(addr);
            if (sign_byte == CH_PLUS || sign_byte == CH_MINUS) {
                number += decode_zx81_char(sign_byte);
                addr++;
            }
            continue;
        }

        if (byte >= CH_0 && byte <= CH_9) {
            number += ch;
            addr++;
            continue;
        }

        // must have a CH_NUMBER byte
        if (byte != CH_NUMBER) {
            fatal("Found '" + ch + "' when parsing a number at address " +
                  int16_to_hex(addr));
        }
        addr++;
        break;
    }

    // compute value
    double value1 = std::stold(number);

    // get encoded number
    double value2 = decode_zx81_number(addr, mem);

    if (fabs(value2 - value1) > 1e-6) {
        fatal("Mismatch between parsed number and encoded number at address " +
              int16_to_hex(addr));
    }

    Token token = Token(TokenType::Float, value2);
    token.text = number;
    return token;
}

static Token collect_string(int& addr, Memory& mem) {
    std::string str;
    addr++;     // skip start quote
    while (true) {
        uint8_t byte = mem.peek(addr);
        if (byte == CH_QUOTE) {
            addr++;     // skip end quote
            break;
        }
        if (byte == CH_NL) {
            fatal("Found NEWLINE when parsing a string at address " + int16_to_hex(
                      addr));
        }
        str += decode_zx81_char(byte);
        addr++;
    }
    Token string_token(TokenType::StringLiteral, "\"" + str + "\"");
    string_token.svalue = str;
    return string_token;
}

static bool has_binary_data(Memory& mem, int addr, int length) {
    int end_addr = addr + length;
    for (; addr < end_addr; ++addr) {
        uint8_t byte = mem.peek(addr);
        if ((byte & 0x40) != 0) {
            return true;
        }
    }
    return false;
}

static void decode_rem_line(int length, Line& line) {
    Token rem_token(TokenType::Identifier, "REM");
    append_token(line.tokens, rem_token);
    line.asm_length = length;
}

static void decode_basic_line(Memory& mem, int addr, int length,
                              Line& line) {
    int end_addr = addr + length;
    int start_addr = addr;
    while (addr + 1 < end_addr) {
        uint8_t byte = mem.peek(addr);
        std::string ch = decode_zx81_char(byte);

        // REM statement
        if (addr == start_addr && byte == CH_REM
                && has_binary_data(mem, addr + 1, end_addr - 2)) {
            decode_rem_line(end_addr - addr - 2, line);
            return;
        }

        // identifiers
        if (byte >= CH_A && byte <= CH_Z) {
            Token ident_token = collect_ident(addr, mem);
            append_token(line.tokens, ident_token);
            continue;
        }

        // keywords
        if (ch.size() > 1 && std::isalpha(ch[0])) {
            Keyword keyword = lookup_keyword(ch);
            if (keyword != Keyword::None) {
                Token keyword_token(TokenType::Identifier, ch);
                append_token(line.tokens, keyword_token);
                addr++;
                continue;
            }
        }

        // numbers
        if (byte == CH_DOT || (byte >= CH_0 && byte <= CH_9)) {
            Token number_token = collect_number(addr, mem);
            append_token(line.tokens, number_token);
            continue;
        }

        // strings
        if (byte == CH_QUOTE) {
            Token string_token = collect_string(addr, mem);
            append_token(line.tokens, string_token);
            continue;
        }

        // literal space
        if (byte == CH_SP) {
            Token space_token(TokenType::Punctuation, "\\00");
            append_token(line.tokens, space_token);
            addr++;
            continue;
        }

        // charcater cannot be binary
        if (!ch.empty() && ch[0] == '\\') {
            fatal("Found binary character at address " + int16_to_hex(addr));
        }

        // other characters
        Token punct(TokenType::Punctuation, ch);
        append_token(line.tokens, punct);
        addr++;
    }

    // last byte muts be CH_NL
    uint8_t byte = mem.peek(addr);
    if (byte != CH_NL) {
        fatal("Expected NEWLINE at end of BASIC line, found '" + decode_zx81_char(
                  byte) + "' at address " + int16_to_hex(addr));
    }
    addr++;
}

static void decode_basic(Prog& prog) {
    int d_file_addr = prog.mem.peek_word(D_FILE);
    for (int addr = PROG; addr < d_file_addr; ) {
        Line line;
        line.start_addr = addr;
        line.line_num = prog.mem.peek_word_be(addr);
        addr += 2;
        int line_length = prog.mem.peek_word(addr);
        addr += 2;
        decode_basic_line(prog.mem, addr, line_length, line);
        addr += line_length;

        prog.lines.push_back(line);
    }
}

static void decode_display(Prog& prog) {
    int d_file = prog.mem.peek_word(D_FILE);
    int vars = prog.mem.peek_word(VARS);

    if (vars - d_file < 1 + 24 * 33) {
        prog.dfile_colapsed = true;
    }

    for (int addr = d_file + 1, i = 0; addr < vars && i < 24; i++) {
        std::string text;
        for (int j = 0; j < 33; j++) {
            uint8_t byte = prog.mem.peek(addr);
            if (byte == CH_NL) {
                addr++;
                break;
            }
            text += decode_zx81_char(byte);
            addr++;
        }
        prog.dfile.push_back(str_trim(text));
    }

    // remove empty lines at the end of dfile
    while (!prog.dfile.empty() && prog.dfile.back().empty()) {
        prog.dfile.pop_back();
    }
}

static void decode_vars(Prog& prog) {
    int vars = prog.mem.peek_word(VARS);
    int e_line = prog.mem.peek_word(E_LINE);
    for (int addr = vars; addr < e_line; ) {
        if (prog.mem.peek(addr) == 0x80) {
            break; // end of variables
        }

        Variable var;
        // 011-letter - numeric variable
        if ((prog.mem.peek(addr) & 0xE0) == 0x60) {
            var.is_array_var = false;
            var.is_string_var = false;
            var.is_loop_var = false;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0x40);
            addr++;

            double value = decode_zx81_number(addr, prog.mem);
            var.nvalues.push_back(value);

            prog.variables.push_back(std::move(var));
            continue;
        }

        // 101-letter, 001-letter, 101-letter - numeric variable
        if ((prog.mem.peek(addr) & 0xE0) == 0xA0) {
            var.is_array_var = false;
            var.is_string_var = false;
            var.is_loop_var = false;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0x80);
            addr++;
            while ((prog.mem.peek(addr) & 0xE0) == 0x20) {
                var.name += decode_zx81_char(prog.mem.peek(addr));
                addr++;
            }
            var.name += decode_zx81_char(prog.mem.peek(addr) - 0x80);
            addr++;

            double value = decode_zx81_number(addr, prog.mem);
            var.nvalues.push_back(value);

            prog.variables.push_back(std::move(var));
            continue;
        }

        // 010-letter - string variable
        if ((prog.mem.peek(addr) & 0xE0) == 0x40) {
            var.is_array_var = false;
            var.is_string_var = true;
            var.is_loop_var = false;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0x20) + "$";
            addr++;

            int str_length = prog.mem.peek_word(addr);
            addr += 2;

            std::string str_value;
            for (int i = 0; i < str_length; ++i) {
                str_value += decode_zx81_char(prog.mem.peek(addr));
                addr++;
            }
            var.svalues.push_back(str_value);

            prog.variables.push_back(std::move(var));
            continue;
        }

        // 100-letter - numeric array variable
        if ((prog.mem.peek(addr) & 0xE0) == 0x80) {
            var.is_array_var = true;
            var.is_string_var = false;
            var.is_loop_var = false;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0x60);
            addr++;

            /*int array_length =*/ prog.mem.peek_word(addr);
            addr += 2;

            int num_dims = prog.mem.peek(addr);
            addr++;

            int num_values = 1;
            for (int i = 0; i < num_dims; ++i) {
                int dim_size = prog.mem.peek_word(addr);
                addr += 2;
                var.dims.push_back(dim_size);
                num_values *= dim_size;
            }

            for (int i = 0; i < num_values; ++i) {
                double value = decode_zx81_number(addr, prog.mem);
                var.nvalues.push_back(value);
            }

            prog.variables.push_back(std::move(var));
            continue;
        }

        // 110-letter - string array variable
        if ((prog.mem.peek(addr) & 0xE0) == 0xC0) {
            var.is_array_var = true;
            var.is_string_var = true;
            var.is_loop_var = false;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0xA0) + "$";
            addr++;

            /*int array_length =*/ prog.mem.peek_word(addr);
            addr += 2;

            int num_dims = prog.mem.peek(addr);
            addr++;

            int num_values = 1;
            int last_dimension = 1;
            for (int i = 0; i < num_dims; ++i) {
                int dim_size = prog.mem.peek_word(addr);
                addr += 2;
                var.dims.push_back(dim_size);
                num_values *= dim_size;
                last_dimension = dim_size;
            }

            for (int i = 0; i < num_values / last_dimension; ++i) {
                std::string svalue;
                for (int j = 0; j < last_dimension; j++) {
                    svalue += decode_zx81_char(prog.mem.peek(addr));
                    addr++;
                }
                var.svalues.push_back(svalue);
            }

            prog.variables.push_back(std::move(var));
            continue;
        }

        // 111-letter - loop variable
        if ((prog.mem.peek(addr) & 0xE0) == 0xE0) {
            var.is_array_var = false;
            var.is_string_var = false;
            var.is_loop_var = true;

            var.name = decode_zx81_char(prog.mem.peek(addr) - 0xC0);
            addr++;

            double value = decode_zx81_number(addr, prog.mem);
            var.nvalues.push_back(value);
            var.limit = decode_zx81_number(addr, prog.mem);
            var.step = decode_zx81_number(addr, prog.mem);
            var.line_num = prog.mem.peek_word(addr);
            addr += 2;

            prog.variables.push_back(std::move(var));
            continue;
        }

        error("Unknown variable type at address " + int16_to_hex(addr));
        break;
    }
}

static void decode_trailer(Prog& prog) {
    int e_line = prog.mem.peek_word(E_LINE);
    for (int addr = e_line; addr < prog.mem.end_of_prog; addr++) {
        prog.trailer.push_back(prog.mem.peek(addr));
    }
}

void dis_decode_prog(Prog& prog, const std::string& p_filename) {
    prog.p_filename = p_filename;
    prog.mem.load_file(p_filename, VERSN);
    decode_basic(prog);
    decode_display(prog);
    decode_vars(prog);
    decode_trailer(prog);
}
