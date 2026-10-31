//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "parser.h"
#include "scan.h"
#include "types.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

static void check_label_line_num(const std::string& text,
                                 bool& has_label, bool& has_line_num, bool& has_text) {
    Scanner scan(Scanner::Type::Basic, text);
    has_label = false;
    has_line_num = false;
    has_text = false;

    // collect label and line number in any order
    while (true) {
        const char* p0 = scan.pos();
        std::string label;
        if (!has_label && scan.match("@") && scan.parse_ident(label)
                && scan.match(":")) {
            has_label = true;
            continue;
        }
        scan.set_pos(p0);
        int line_num;
        if (!has_line_num && scan.parse_integer(line_num)) {
            has_line_num = true;
            continue;
        }
        break; // not found, break the loop
    }

    // check for text
    scan.skip_spaces();
    has_text = !scan.at_end();
}

static bool is_label_line_num_continuation(const std::string& l0,
        const std::string& l1) {
    bool has_label0, has_line_num0, has_text0;
    check_label_line_num(l0, has_label0, has_line_num0, has_text0);
    bool has_label1, has_line_num1, has_text1;
    check_label_line_num(l1, has_label1, has_line_num1, has_text1);
    int num_labels = has_label0 + has_label1;
    int num_line_nums = has_line_num0 + has_line_num1;
    if (num_labels <= 1 && num_line_nums <= 1 && !has_text0) {
        return true;
    }
    return false;
}

void Parser::parse_bas_file(const std::string& filename) {
    in_asm = false;

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        fatal_error("open file: " + filename);
    }

    // load all lines to a vector of pair<loc, text>
    std::vector<std::pair<SourceLoc, std::string>> lines;
    std::string text;
    int line_num = 0;
    while (getline(ifs, text)) {
        line_num++;
        loc = SourceLoc(filename, line_num);
        lines.emplace_back(std::make_pair(loc, text));
    }

    // join continuation lines
    for (size_t i = 0; i + 1 < lines.size(); i++) {
        auto& l0 = lines[i];
        auto& l1 = lines[i + 1];
        if (!l0.second.empty() && l0.second.back() == '\\') {
            l0.second.pop_back();
            l0.second += l1.second;
            lines.erase(lines.begin() + i + 1);
            i--;
        }
    }

    // join line with only label and or line number with next line if
    // both lines have max one label and one line number
    for (size_t i = 0; i + 1 < lines.size(); i++) {
        auto& l0 = lines[i];
        auto& l1 = lines[i + 1];
        if (is_label_line_num_continuation(l0.second, l1.second)) {
            l0.second += l1.second;
            lines.erase(lines.begin() + i + 1);
            i--;
        }
    }
    // parse each line
    for (size_t i = 0; i < lines.size(); i++) {
        auto& l0 = lines[i];
        loc = l0.first;
        scan.set_text(in_asm ? Scanner::Type::Asm : Scanner::Type::Basic, l0.second);
        parse_line();
    }

    // join continuation lines with @label:/line_num and no text
    // TODO
}

void Parser::parse_line() {
    const char* p0 = scan.pos();

    scan.skip_spaces();
    if (scan.at_end()) {
        return;
    }

    if (scan.match("#")) {
        parse_pragma_line();
        return;
    }

    if (in_asm) {
        // assembly is space sensitive, go back to the beginning of the line
        scan.set_pos(p0);

        SourceLine source_line(SourceLine::Type::Asm, loc);
        if (parse_asm_line(source_line)) {
            basic->source_lines.push_back(source_line);
        }
    }
    else {
        SourceLine source_line(SourceLine::Type::Basic, loc);
        if (parse_basic_line(source_line)) {
            basic->source_lines.push_back(source_line);
        }
    }
}

void Parser::parse_pragma_line() {
    int n = 0;
    std::string str;
    std::string keyword;
    if (!scan.parse_ident(keyword)) {
        return; // ignore
    }

    if (keyword == "VARS") {
        BasicVar basic_var;
        basic_var.loc = loc;
        if (parse_basic_var(basic_var)) {
            basic->basic_vars.push_back(basic_var);
        }
        return;
    }

    if (keyword == "SYSVARS" && scan.match("=")) {
        basic->sysvars = parse_byte_list();
        return;
    }

    if (keyword == "DFILE" && scan.match("=") && scan.parse_string(str)
            && scan.at_end()) {
        basic->dfile_lines.push_back(str);
        return;
    }

    if (keyword == "DFILE_COLLAPSED" && scan.match("=") && scan.parse_integer(n)
            && scan.at_end()) {
        basic->dfile_collapsed = n ? true : false;
        return;
    }

    if (keyword == "AUTOSTART" && scan.match("=") && scan.parse_integer(n)
            && scan.at_end()) {
        basic->autostart = n ? true : false;
        return;
    }

    if (keyword == "AUTOSTART_LINE" && scan.match("=") && scan.parse_integer(n)
            && scan.at_end()) {
        basic->autostart_line = n;
        return;
    }

    if (keyword == "FAST" && scan.match("=") && scan.parse_integer(n)
            && scan.at_end()) {
        basic->fast = n ? true : false;
        return;
    }

    if (keyword == "INCREMENT" && scan.match("=") && scan.parse_integer(n)
            && scan.at_end()) {
        basic->increment = n;
        return;
    }

    if (keyword == "ASM") {
        in_asm = true;
        return;
    }

    if (keyword == "ENDASM") {
        in_asm = false;
        return;
    }

    // ignore, consider a comment
}

bool Parser::parse_basic_var(BasicVar& var) {
    var = BasicVar();

    bool is_string = false;
    bool is_array = false;
    int num_elems = 0;

    // get name
    if (!scan.parse_ident(var.name)) {
        error(loc, "identifier expected: " + std::string(scan.pos()));
        return false;
    }

    // get string marker
    if (scan.match("$")) {
        is_string = true;
        if (var.name.size() > 1) {
            error(loc, "name too long: " + var.name);
            return false;
        }
    }

    // get array marker and dimensions
    if (scan.match("(")) {
        is_array = true;
        if (var.name.size() > 1) {
            error(loc, "name too long: " + var.name);
            return false;
        }

        num_elems = 1;
        do {
            int dim = 0;
            if (!scan.parse_integer(dim)) {
                error(loc, "array dim expected: " + std::string(scan.pos()));
                return false;
            }
            num_elems *= dim;
            var.dims.push_back(dim);
        }
        while (scan.match(","));

        if (!scan.match(")")) {
            error(loc, "array dim closing ')' expected: " + std::string(scan.pos()));
            return false;
        }
    }

    // get =
    if (!scan.match("=")) {
        error(loc, "'=' expected: " + std::string(scan.pos()));
        return false;
    }

    // get value
    if (is_string == false && is_array == false) {
        std::string str;

        var.type = BasicVar::Type::Number;
        if (!scan.parse_number(var.value, str)) {
            error(loc, "number expected: " + std::string(scan.pos()));
            return false;
        }

        if (scan.match(",")) {
            var.type = BasicVar::Type::ForNextLoop;
            if (var.name.size() > 1) {
                error(loc, "name too long: " + var.name);
                return false;
            }

            if (!scan.parse_number(var.limit, str)) {
                error(loc, "number expected: " + std::string(scan.pos()));
                return false;
            }
            if (!scan.match(",")) {
                error(loc, "',' expected: " + std::string(scan.pos()));
                return false;
            }
            if (!scan.parse_number(var.step, str)) {
                error(loc, "number expected: " + std::string(scan.pos()));
                return false;
            }
            if (!scan.match(",")) {
                error(loc, "',' expected: " + std::string(scan.pos()));
                return false;
            }
            if (!scan.parse_integer(var.line_num)) {
                error(loc, "integer expected: " + std::string(scan.pos()));
                return false;
            }
            if (!scan.at_end()) {
                error(loc, "unexpected characters at end: " + std::string(scan.pos()));
                return false;
            }
        }
    }
    else if (is_string == false && is_array == true) {
        double value = 0.0;
        std::string str;

        var.type = BasicVar::Type::ArrayNumbers;

        for (size_t i = 0; i < static_cast<size_t>(num_elems); i++) {
            if (i > 0) {
                if (!scan.match(",")) {
                    error(loc, "',' expected: " + std::string(scan.pos()));
                    return false;
                }
            }
            if (!scan.parse_number(value, str)) {
                error(loc, "number expected: " + std::string(scan.pos()));
                return false;
            }
            var.values.push_back(value);
        }
        if (!scan.at_end()) {
            error(loc, "unexpected characters at end: " + std::string(scan.pos()));
            return false;
        }
    }
    else if (is_string == true && is_array == false) {
        var.type = BasicVar::Type::String;

        if (!scan.parse_string(var.str)) {
            error(loc, "string expected: " + std::string(scan.pos()));
            return false;
        }
        if (!scan.at_end()) {
            error(loc, "unexpected characters at end: " + std::string(scan.pos()));
            return false;
        }
    }
    else if (is_string == true && is_array == true) {
        std::string str;

        var.type = BasicVar::Type::ArrayStrings;

        int last_dim = var.dims.back();
        num_elems /= last_dim;

        for (size_t i = 0; i < static_cast<size_t>(num_elems); i++) {
            if (i > 0) {
                if (!scan.match(",")) {
                    error(loc, "',' expected: " + std::string(scan.pos()));
                    return false;
                }
            }
            if (!scan.parse_string(str)) {
                error(loc, "string expected: " + std::string(scan.pos()));
                return false;
            }
            if (str.size() != static_cast<size_t>(last_dim)) {
                error(loc, "string length should be " + std::to_string(last_dim));
                return false;
            }
            var.strs.push_back(str);
        }
        if (!scan.at_end()) {
            error(loc, "unexpected characters at end: " + std::string(scan.pos()));
            return false;
        }
    }

    return true;
}

Bytes Parser::parse_byte_list() {
    if (scan.at_end()) {
        return Bytes{};
    }

    Bytes out;
    while (true) {
        int value = 0;
        if (!scan.parse_integer(value)) {
            error(loc, "integer expected: " + std::string(scan.pos()));
            return Bytes{};
        }
        out.push_back(value);
        if (!scan.match(",")) {
            break;
        }
    }
    if (!scan.at_end()) {
        error(loc, "end of line expected: " + std::string(scan.pos()));
        return Bytes{};
    }

    return out;
}

bool Parser::parse_basic_line(SourceLine& source_line) {
    static const std::vector<std::pair<std::string, int>> keywords = {
        std::make_pair("RND", C_RND),
        std::make_pair("INKEY$", C_INKEY_dollar),
        std::make_pair("PI", C_PI),
        std::make_pair("AT", C_AT),
        std::make_pair("TAB", C_TAB),
        std::make_pair("CODE", C_CODE),
        std::make_pair("VAL", C_VAL),
        std::make_pair("LEN", C_LEN),
        std::make_pair("SIN", C_SIN),
        std::make_pair("COS", C_COS),
        std::make_pair("TAN", C_TAN),
        std::make_pair("ASN", C_ASN),
        std::make_pair("ACS", C_ACS),
        std::make_pair("ATN", C_ATN),
        std::make_pair("LN", C_LN),
        std::make_pair("EXP", C_EXP),
        std::make_pair("INT", C_INT),
        std::make_pair("SQR", C_SQR),
        std::make_pair("SGN", C_SGN),
        std::make_pair("ABS", C_ABS),
        std::make_pair("PEEK", C_PEEK),
        std::make_pair("USR", C_USR),
        std::make_pair("STR$", C_STR_dollar),
        std::make_pair("CHR$", C_CHR_dollar),
        std::make_pair("NOT", C_NOT),
        std::make_pair("**", C_power),
        std::make_pair("OR", C_OR),
        std::make_pair("AND", C_AND),
        std::make_pair("<=", C_le),
        std::make_pair(">=", C_ge),
        std::make_pair("<>", C_ne),
        std::make_pair("THEN", C_THEN),
        std::make_pair("TO", C_TO),
        std::make_pair("STEP", C_STEP),
        std::make_pair("LPRINT", C_LPRINT),
        std::make_pair("LLIST", C_LLIST),
        std::make_pair("STOP", C_STOP),
        std::make_pair("SLOW", C_SLOW),
        std::make_pair("FAST", C_FAST),
        std::make_pair("NEW", C_NEW),
        std::make_pair("SCROLL", C_SCROLL),
        std::make_pair("CONT", C_CONT),
        std::make_pair("DIM", C_DIM),
        std::make_pair("FOR", C_FOR),
        std::make_pair("GOTO", C_GOTO),
        std::make_pair("GOSUB", C_GOSUB),
        std::make_pair("INPUT", C_INPUT),
        std::make_pair("LOAD", C_LOAD),
        std::make_pair("LIST", C_LIST),
        std::make_pair("LET", C_LET),
        std::make_pair("PAUSE", C_PAUSE),
        std::make_pair("NEXT", C_NEXT),
        std::make_pair("POKE", C_POKE),
        std::make_pair("PRINT", C_PRINT),
        std::make_pair("PLOT", C_PLOT),
        std::make_pair("RUN", C_RUN),
        std::make_pair("SAVE", C_SAVE),
        std::make_pair("RAND", C_RAND),
        std::make_pair("IF", C_IF),
        std::make_pair("CLS", C_CLS),
        std::make_pair("UNPLOT", C_UNPLOT),
        std::make_pair("CLEAR", C_CLEAR),
        std::make_pair("RETURN", C_RETURN),
        std::make_pair("COPY", C_COPY),
        std::make_pair("\\0c", C_pound),
        std::make_pair("$", C_dollar),
        std::make_pair(":", C_colon),
        std::make_pair("?", C_quest),
        std::make_pair("(", C_lparens),
        std::make_pair(")", C_rparens),
        std::make_pair(">", C_gt),
        std::make_pair("<", C_lt),
        std::make_pair("=", C_eq),
        std::make_pair("+", C_plus),
        std::make_pair("-", C_minus),
        std::make_pair("*", C_mult),
        std::make_pair("/", C_div),
        std::make_pair(";", C_semicolon),
        std::make_pair(",", C_comma),
        std::make_pair("_", C_space),
    };

    // get label and/or line number
    parse_label_line_num(source_line);

    // assign line number
    if (source_line.line_num < 0) {
        source_line.line_num = last_line_num < 0 ? basic->increment : last_line_num +
                               basic->increment;
    }

    if (source_line.line_num <= last_line_num) {
        error(source_line.loc, "line " + std::to_string(source_line.line_num) +
              " follows line " +
              std::to_string(last_line_num));
    }
    else if (source_line.line_num > MaxLineNum) {
        error(source_line.loc, "line " + std::to_string(source_line.line_num) +
              " above maximum of "
              + std::to_string(MaxLineNum));
    }

    last_line_num = source_line.line_num;

    // get tokens
    while (true) {
        scan.skip_spaces();
        if (scan.at_end()) {
            break;
        }

        Token token;
        const char* p = scan.pos();
        if (p[0] == '\\' && isxdigit_safe(p[1]) && isxdigit_safe(p[2])) {
            char hex[3] = { p[1], p[2], 0 };
            token.code = static_cast<ZX81char>(strtol(hex, NULL, 16));
            scan.set_pos(p + 3);
            source_line.tokens.push_back(token);
            continue;
        }

        int n = scan.match_one_of(-1, keywords);
        if (n != -1) {
            token.code = static_cast<ZX81char>(n);
            source_line.tokens.push_back(token);
            continue;
        }

        if (scan.match("REM")) {
            token.code = C_REM;
            source_line.tokens.push_back(token);

            // skip only one space following REM, the other are part of the comment
            const char* p = scan.pos();
            if(isspace_safe(*p)) {
                scan.set_pos(p + 1);
            }

            token.code = T_rem_code;
            p = scan.pos();
            token.bytes = encode_zx81(p, loc); // advances p past the bytes
            scan.set_pos(p);
            source_line.tokens.push_back(token);
            continue;
        }

        if (scan.parse_number(token.fvalue, token.svalue)) {
            token.code = T_float;
            source_line.tokens.push_back(token);
            continue;
        }

        if (scan.parse_string(token.svalue)) {
            token.code = T_string;
            source_line.tokens.push_back(token);
            continue;
        }

        if (scan.parse_ident(token.ident)) {
            token.code = T_ident;
            source_line.tokens.push_back(token);
            continue;
        }

        if (parse_line_num_ref(token.ident)) {
            token.code = T_line_num_ref;
            source_line.tokens.push_back(token);
            continue;
        }

        if (parse_line_addr_ref(token.ident)) {
            token.code = T_line_addr_ref;
            source_line.tokens.push_back(token);
            continue;
        }

        error(loc, "cannot parse: " + std::string(scan.pos()));
        break;
    }

    Token token;
    token.code = C_newline;
    source_line.tokens.push_back(token);

    return true;
}

bool Parser::parse_ref(const std::string& prefix, std::string& ident) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (scan.match(prefix) && scan.parse_ident(ident)) {
        return true;
    }
    else {
        scan.set_pos(p0);
        return false;
    }
}

bool Parser::parse_line_num_ref(std::string& ident) {
    return parse_ref("@", ident);
}

bool Parser::parse_line_addr_ref(std::string& ident) {
    return parse_ref("&", ident);
}

bool Parser::parse_label(std::string& ident) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (parse_line_num_ref(ident) && scan.match(":")) {
        return true;
    }
    else {
        scan.set_pos(p0);
        return false;
    }
}

void Parser::parse_label_line_num(SourceLine& source_line) {
    // get label and/or line number
    while (true) {
        if (source_line.line_num < 0 && scan.parse_integer(source_line.line_num)) {
            continue;
        }
        if (source_line.label.empty() && parse_label(source_line.label)) {
            continue;
        }
        break;
    }
}

bool Parser::parse_asm_line(SourceLine& source_line) {
    (void)source_line;
    return false;
}
