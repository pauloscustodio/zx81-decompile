//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "parser.h"
#include "scan.h"
#include "token.h"
#include "types.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <cmath>
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
    in_asm = false;
    for (size_t i = 0; i < lines.size(); i++) {
        auto& l0 = lines[i];
        loc = l0.first;
        scan.set_text(in_asm ? Scanner::Type::Asm : Scanner::Type::Basic, l0.second);
        parse_line();
    }
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
        if (parse_asm_line(source_line.asm_line)) {
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
        BasicVar basic_var(BasicVar::Type::Number, loc);
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
    var = BasicVar(BasicVar::Type::Number, loc);

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

        int n = scan.match_one_of(-1, keywords);
        if (n != -1) {
            token.code = static_cast<ZX81char>(n);
            source_line.tokens.push_back(token);
            continue;
        }

        if (scan.parse_ident(token.ident)) {
            token.code = T_ident;
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

bool Parser::parse_primary_expr(Expr& expr) {
    Token token;
    const char* p0 = scan.pos();

    // at end
    scan.set_pos(p0);
    if (scan.at_end()) {
        return false;
    }

    // number
    scan.set_pos(p0);
    if (scan.parse_integer(token.ivalue)) {
        token.code = T_integer;
        expr.rpn.push_back(token);
        return true;
    }
    scan.set_pos(p0);
    if (scan.parse_number(token.fvalue, token.svalue)
            && floor(token.fvalue) == token.fvalue) {
        token.code = T_integer;
        token.ivalue = INT(token.fvalue);
        expr.rpn.push_back(token);
        return true;
    }

    // ASMPC
    scan.set_pos(p0);
    if (scan.match("$")) {
        token.code = T_ASMPC;
        expr.rpn.push_back(token);
        return true;
    }
    scan.set_pos(p0);
    if (scan.match("ASMPC")) {
        token.code = T_ASMPC;
        expr.rpn.push_back(token);
        return true;
    }

    // symbol reference
    scan.set_pos(p0);
    if (scan.parse_ident(token.ident)) {
        token.code = T_symbol;
        expr.rpn.push_back(token);
        return true;
    }

    // subexpression
    scan.set_pos(p0);
    if (scan.match("(") && parse_expr(expr) && scan.match(")")) {
        return true;
    }

    // character literal
    scan.set_pos(p0);
    const char* p = scan.pos();
    if (*p == '\'') {
        p++;
        Byte b = encode_zx81_char(p, loc);
        if (*p != '\'') {
            scan.set_pos(p0);
            return false;
        }
        scan.set_pos(p + 1);

        token.code = T_integer;
        token.ivalue = b;
        expr.rpn.push_back(token);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_unary_expr(Expr& expr) {
    Token token;
    const char* p0 = scan.pos();

    // unary minus
    scan.set_pos(p0);
    if (scan.match("-") && parse_unary_expr(expr)) {
        token.code = T_unary_minus;
        expr.rpn.push_back(token);
        return true;
    }

    // unary plus
    scan.set_pos(p0);
    if (scan.match("+") && parse_unary_expr(expr)) {
        return true;
    }

    // logical not
    scan.set_pos(p0);
    if (scan.match("!") && parse_unary_expr(expr)) {
        token.code = T_log_not;
        expr.rpn.push_back(token);
        return true;
    }

    // binary not
    scan.set_pos(p0);
    if (scan.match("~") && parse_unary_expr(expr)) {
        token.code = T_bin_not;
        expr.rpn.push_back(token);
        return true;
    }

    return parse_primary_expr(expr);
}

bool Parser::parse_mult_expr(Expr& expr) {
    if (!parse_unary_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("*") && parse_unary_expr(expr)) {
            token.code = C_mult;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("/") && parse_unary_expr(expr)) {
            token.code = C_div;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("%") && parse_unary_expr(expr)) {
            token.code = T_mod;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_add_expr(Expr& expr) {
    if (!parse_mult_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("+") && parse_mult_expr(expr)) {
            token.code = C_plus;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("-") && parse_mult_expr(expr)) {
            token.code = C_minus;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_shift_expr(Expr& expr) {
    if (!parse_add_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("<<") && parse_add_expr(expr)) {
            token.code = T_left_shift;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match(">>") && parse_add_expr(expr)) {
            token.code = T_right_shift;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_rel_expr(Expr& expr) {
    if (!parse_shift_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("<") && parse_shift_expr(expr)) {
            token.code = C_lt;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("<=") && parse_shift_expr(expr)) {
            token.code = C_le;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match(">") && parse_shift_expr(expr)) {
            token.code = C_gt;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match(">=") && parse_shift_expr(expr)) {
            token.code = C_ge;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_eq_expr(Expr& expr) {
    if (!parse_rel_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("==") && parse_rel_expr(expr)) {
            token.code = C_eq;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("=") && parse_rel_expr(expr)) {
            token.code = C_eq;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("<>") && parse_rel_expr(expr)) {
            token.code = C_ne;
            expr.rpn.push_back(token);
            continue;
        }
        if (scan.match("!=") && parse_rel_expr(expr)) {
            token.code = C_ne;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_bin_and_expr(Expr& expr) {
    if (!parse_eq_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("&") && *scan.pos() != '&' && parse_eq_expr(expr)) {
            token.code = T_bin_and;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_bin_xor_expr(Expr& expr) {
    if (!parse_bin_and_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("^") && parse_bin_and_expr(expr)) {
            token.code = T_bin_xor;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_bin_or_expr(Expr& expr) {
    if (!parse_bin_xor_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("|") && *scan.pos() != '|' && parse_bin_xor_expr(expr)) {
            token.code = T_bin_or;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_log_and_expr(Expr& expr) {
    if (!parse_bin_or_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("&&") && parse_bin_or_expr(expr)) {
            token.code = T_log_and;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_log_or_expr(Expr& expr) {
    if (!parse_log_and_expr(expr)) {
        return false;
    }
    while (true) {
        Token token;
        if (scan.match("||") && parse_log_and_expr(expr)) {
            token.code = T_log_or;
            expr.rpn.push_back(token);
            continue;
        }
        break;
    }
    return true;
}

bool Parser::parse_conditional_expr(Expr& expr) {
    if (!parse_log_or_expr(expr)) {
        return false;
    }
    if (scan.match("?")) {
        if (!parse_log_or_expr(expr)) {
            return false;
        }
        if (!scan.match(":")) {
            return false;
        }
        if (!parse_conditional_expr(expr)) {
            return false;
        }
        Token token;
        token.code = T_conditional;
        expr.rpn.push_back(token);
    }
    return true;
}

bool Parser::parse_expr(Expr& expr) {
    return parse_conditional_expr(expr);
}

bool Parser::parse_asm_line(AsmLine& asm_line) {
    if (parse_asm_label(asm_line.label)) {
        // is it EQU?
        scan.skip_spaces();
        const char* p0 = scan.pos();
        if (scan.match("EQU")) {
            if (parse_expr(asm_line.equ_expr) && scan.at_end()) {
                return true;
            }
            else {
                return false;
            }
        }
        scan.set_pos(p0);
    }

    if (parse_defb(asm_line) && scan.at_end()) {
        return true;
    }
    if (parse_defw(asm_line) && scan.at_end()) {
        return true;
    }
    if (parse_defs(asm_line) && scan.at_end()) {
        return true;
    }
    if (parse_z80_opcode(asm_line)) {
        return true;
    }
    if (scan.at_end()) {
        return true;
    }

    error(loc, "cannot parse: " + std::string(scan.pos()));
    return false;
}

// identifier at column 0
bool Parser::parse_asm_label(std::string& ident) {
    const char* p = scan.pos();
    if (isalpha_safe(*p) || *p == '_') {
        scan.parse_ident(ident);
        scan.skip_spaces();
        scan.match(":");						// optional colon
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_defb(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (scan.match("DEFB") || scan.match("DEFM")) {
        while (true) {
            std::string str;
            if (scan.parse_string(str)) {
                Bytes bytes = encode_zx81(str, loc);
                for (auto& b : bytes) {
                    asm_line.bytes.push_back(b);
                }

                if (scan.match(",")) {
                    continue;
                }
                else {
                    break;
                }
            }

            Expr expr(loc);
            if (parse_expr(expr)) {
                add_byte_patch(asm_line, expr);

                if (scan.match(",")) {
                    continue;
                }
                else {
                    break;
                }
            }

            return false;
        }
        return true;
    }
    scan.set_pos(p0);
    return false;
}

bool Parser::parse_defw(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (scan.match("DEFW")) {
        while (true) {
            Expr expr(loc);
            if (!parse_expr(expr)) {
                return false;
            }
            add_word_patch(asm_line, expr);

            if (scan.match(",")) {
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }
    scan.set_pos(p0);
    return false;
}

bool Parser::parse_defs(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (scan.match("DEFS")) {
        Expr expr(loc);
        if (parse_expr(expr) && scan.at_end()) {
            int value = eval_const_expr(expr);
            for (int i = 0; i < value; i++) {
                asm_line.bytes.push_back(0);
            }
            return true;
        }
    }
    scan.set_pos(p0);
    return false;
}

bool Parser::parse_z80_opcode(AsmLine& asm_line) {
    if (scan.at_end()) {
        return true;
    }
    if (parse_single_opcode(asm_line)) {
        return true;
    }
    else if (parse_ld(asm_line)) {
        return true;
    }
    if (parse_alu(asm_line)) {
        return true;
    }
    if (parse_ex(asm_line)) {
        return true;
    }
    if (parse_bit(asm_line)) {
        return true;
    }
    if (parse_call_jp(asm_line)) {
        return true;
    }
    if (parse_rst(asm_line)) {
        return true;
    }
    if (parse_ret(asm_line)) {
        return true;
    }
    if (parse_jr(asm_line)) {
        return true;
    }
    if (parse_inc_dec(asm_line)) {
        return true;
    }
    if (parse_in(asm_line)) {
        return true;
    }
    if (parse_out(asm_line)) {
        return true;
    }
    if (parse_im(asm_line)) {
        return true;
    }
    if (parse_push_pop(asm_line)) {
        return true;
    }
    if (parse_rotate(asm_line)) {
        return true;
    }

    return false;
}

bool Parser::parse_single_opcode(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> single_opcode = {
        std::make_pair("NOP", 0x00),
        std::make_pair("RLCA", 0x07),
        std::make_pair("RRCA", 0x0f),
        std::make_pair("RLA", 0x17),
        std::make_pair("RRA", 0x1f),
        std::make_pair("DAA", 0x27),
        std::make_pair("CPL", 0x2f),
        std::make_pair("SCF", 0x37),
        std::make_pair("CCF", 0x3f),
        std::make_pair("HALT", 0x76),
        std::make_pair("RETN", 0xed45),
        std::make_pair("RETI", 0xed4d),
        std::make_pair("RET", 0xc9),
        std::make_pair("EXX", 0xd9),
        std::make_pair("DI", 0xf3),
        std::make_pair("EI", 0xfb),
        std::make_pair("NEG", 0xed44),
        std::make_pair("RRD", 0xed67),
        std::make_pair("RLD", 0xed6f),
        std::make_pair("LDIR", 0xedb0),
        std::make_pair("CPIR", 0xedb1),
        std::make_pair("INIR", 0xedb2),
        std::make_pair("OTIR", 0xedb3),
        std::make_pair("LDDR", 0xedb8),
        std::make_pair("CPDR", 0xedb9),
        std::make_pair("INDR", 0xedba),
        std::make_pair("OTDR", 0xedbb),
        std::make_pair("LDI", 0xeda0),
        std::make_pair("CPI", 0xeda1),
        std::make_pair("INI", 0xeda2),
        std::make_pair("OUTI", 0xeda3),
        std::make_pair("LDD", 0xeda8),
        std::make_pair("CPD", 0xeda9),
        std::make_pair("IND", 0xedaa),
        std::make_pair("OUTD", 0xedab),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();

    int n = scan.match_one_of(-1, single_opcode);
    if (n != -1 && scan.at_end()) {
        if ((n & 0xff00) != 0) {
            asm_line.bytes.push_back((n >> 8) & 0xff);
        }
        asm_line.bytes.push_back(n & 0xff);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_ld(AsmLine& asm_line) {
    if (!scan.match("LD")) {
        return false;
    }

    scan.skip_spaces();
    const char* p0 = scan.pos();

    int r1, r2, pfx;
    Expr expr1(loc), expr2(loc);

    // ld (bc|de), a
    scan.set_pos(p0);
    if (scan.match("(") && parse_bc_de(r1) && scan.match(")")
            && scan.match(",") && scan.match("A") && scan.at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1);
        return true;
    }

    // ld a, (bc|de)
    scan.set_pos(p0);
    if (scan.match("A") && scan.match(",")
            && scan.match("(") && parse_bc_de(r1) && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1);
        return true;
    }

    // ld r1, r2
    scan.set_pos(p0);
    if (parse_r1(r1) && scan.match(",") && parse_r1(r2) && scan.at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld (hl), r
    scan.set_pos(p0);
    if (scan.match("(") && scan.match("HL") && scan.match(")") && scan.match(",")
            && parse_r1(r1)
            && scan.at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        return true;
    }

    // ld r, (hl)
    scan.set_pos(p0);
    if (parse_r1(r1) && scan.match(",") && scan.match("(") && scan.match("HL")
            && scan.match(")")
            && scan.at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        return true;
    }

    // ld i, a
    scan.set_pos(p0);
    if (scan.match("I") && scan.match(",") && scan.match("A") && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x47);
        return true;
    }

    // ld r, a
    scan.set_pos(p0);
    if (scan.match("R") && scan.match(",") && scan.match("A") && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4f);
        return true;
    }

    // ld a, i
    scan.set_pos(p0);
    if (scan.match("A") && scan.match(",") && scan.match("I") && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x57);
        return true;
    }

    // ld a, r
    scan.set_pos(p0);
    if (scan.match("A") && scan.match(",") && scan.match("R") && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x5f);
        return true;
    }

    // ld (hl), N
    scan.set_pos(p0);
    if (scan.match("(") && scan.match("HL") && scan.match(")")
            && scan.match(",") && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld (x+d), r
    scan.set_pos(p0);
    if (parse_x_plus_ind(pfx, expr1) && scan.match(",") && parse_r1(r1)
            && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // ld r, (x+d)
    scan.set_pos(p0);
    if (parse_r1(r1) && scan.match(",") && parse_x_plus_ind(pfx, expr1)
            && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld (x+d), N
    scan.set_pos(p0);
    if (parse_x_plus_ind(pfx, expr1)
            && scan.match(",") && parse_expr(expr2) && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_sbyte_patch(asm_line, expr1);
        add_byte_patch(asm_line, expr2);
        return true;
    }

    // ld sp, x
    scan.set_pos(p0);
    if (scan.match("SP") && scan.match(",") && parse_hl_ix_iy(pfx)
            && scan.at_end()) {
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xf9);
        return true;
    }

    // ld dd, (NN)
    scan.set_pos(p0);
    if (parse_dd_sp(r1) && scan.match(",")
            && scan.match("(") && parse_expr(expr1) && scan.match(")") && scan.at_end()) {
        if (r1 == 2) {
            asm_line.bytes.push_back(0x2a);
        }
        else if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
            asm_line.bytes.push_back(0x2a);
        }
        else {
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x4b + 16 * (r1 & 0xff));
        }
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld dd, NN
    scan.set_pos(p0);
    if (parse_dd_sp(r1) && scan.match(",") && parse_expr(expr1) && scan.at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
        }
        asm_line.bytes.push_back(0x01 + 16 * (r1 & 0xff));
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld (NN), dd
    scan.set_pos(p0);
    if (scan.match("(") && parse_expr(expr1) && scan.match(")")
            && scan.match(",") && parse_dd_sp(r1) && scan.at_end()) {
        if (r1 == 2) {
            asm_line.bytes.push_back(0x22);
        }
        else if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
            asm_line.bytes.push_back(0x22);
        }
        else {
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x43 + 16 * (r1 & 0xff));
        }
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld a, (NN)
    scan.set_pos(p0);
    if (scan.match("A") && scan.match(",")
            && scan.match("(") && parse_expr(expr1) && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0x3a);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld (NN), a
    scan.set_pos(p0);
    if (scan.match("(") && parse_expr(expr1) && scan.match(")")
            && scan.match(",") && scan.match("A") && scan.at_end()) {
        asm_line.bytes.push_back(0x32);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld r1, N
    scan.set_pos(p0);
    if (parse_r1(r1) && scan.match(",") && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * r1);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_alu(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> alu1 = {
        std::make_pair("ADD", 0),
        std::make_pair("ADC", 1),
        std::make_pair("SUB", 2),
        std::make_pair("SBC", 3),
        std::make_pair("AND", 4),
        std::make_pair("XOR", 5),
        std::make_pair("OR", 6),
        std::make_pair("CP", 7),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1, r2, pfx;
    Expr expr1(loc), expr2(loc);

    int op = scan.match_one_of(-1, alu1);
    if (op == -1) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // add hl|ix|iy, dd
    scan.set_pos(p1);
    if (op == 0 && parse_dd_sp(r1) && (r1 & 0xff) == 2 && scan.match(",")
            && parse_dd_sp(r2) && scan.at_end()) {
        if ((r2 & 0xff) == 2 && (r1 & 0xff00) != (r2 & 0xff00)) {
            scan.set_pos(p0);
            return false;
        }
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x09 + 16 * (r2 & 0xff));
        return true;
    }

    // adc hl, dd
    scan.set_pos(p1);
    if (op == 1 && parse_dd_sp(r1) && r1 == 2 && scan.match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4a + 16 * (r2 & 0xff));
        return true;
    }

    // sbc hl, dd
    scan.set_pos(p1);
    if (op == 3 && parse_dd_sp(r1) && r1 == 2 && scan.match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x42 + 16 * (r2 & 0xff));
        return true;
    }

    // {alu} a, r
    scan.set_pos(p1);
    if (scan.match("A") && scan.match(",") && parse_r1(r1) && scan.at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} r
    scan.set_pos(p1);
    if (parse_r1(r1) && scan.at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} a, (hl)
    scan.set_pos(p1);
    if (scan.match("A") && scan.match(",") && scan.match("(") && scan.match("HL")
            && scan.match(")")
            && scan.at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} (hl)
    scan.set_pos(p1);
    if (scan.match("(") && scan.match("HL") && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} a, (x+DIS)
    scan.set_pos(p1);
    if (scan.match("A") && scan.match(",") && parse_x_plus_ind(pfx, expr1)
            && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // {alu} (x+DIS)
    scan.set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // {alu} a, N
    scan.set_pos(p1);
    if (scan.match("A") && scan.match(",") && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // {alu} N
    scan.set_pos(p1);
    if (parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_ex(AsmLine& asm_line) {
    int pfx;

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();

    if (!scan.match("EX")) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // ex (sp), x
    scan.set_pos(p1);
    if (scan.match("(") && scan.match("SP") && scan.match(")") && scan.match(",")
            && parse_hl_ix_iy(pfx) && scan.at_end()) {
        if (pfx) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe3);
        return true;
    }

    // ex af, af'
    scan.set_pos(p1);
    if (scan.match("AF") && scan.match(",") && scan.match("AF") && scan.match("'")
            && scan.at_end()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex af, af
    scan.set_pos(p1);
    if (scan.match("AF") && scan.match(",") && scan.match("AF") && scan.at_end()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex de, hl
    scan.set_pos(p1);
    if (scan.match("DE") && scan.match(",") && scan.match("HL") && scan.at_end()) {
        asm_line.bytes.push_back(0xeb);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_bit(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> bit1 = {
        std::make_pair("BIT", 0),
        std::make_pair("RES", 1),
        std::make_pair("SET", 2),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1, pfx;
    Expr bit_num(loc), expr1(loc);

    int op = scan.match_one_of(-1, bit1);
    if (op == -1) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // {bit} N, r
    scan.set_pos(p1);
    if (parse_expr(bit_num) && scan.match(",") && parse_r1(r1) && scan.at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            scan.set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + r1);
        return true;
    }

    // {bit} N, (hl)
    scan.set_pos(p1);
    if (parse_expr(bit_num) && scan.match(",") && scan.match("(")
            && scan.match("HL")
            && scan.match(")") && scan.at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            scan.set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

    // {bit} N, (x+DIS)
    scan.set_pos(p1);
    if (parse_expr(bit_num) && scan.match(",")
            && parse_x_plus_ind(pfx, expr1) && scan.at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            scan.set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_call_jp(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> jpcall1 = {
        std::make_pair("JP", 0),
        std::make_pair("CALL", 1),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int f1, pfx;
    Expr expr1(loc), expr2(loc);

    int op = scan.match_one_of(-1, jpcall1);
    if (op == -1) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // jp (hl|ix|iy)
    scan.set_pos(p1);
    if (op == 0 && scan.match("(") && parse_hl_ix_iy(pfx) && scan.match(")")
            && scan.at_end()) {
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe9);
        return true;
    }

    // jp|call flag, NN
    scan.set_pos(p1);
    if (parse_flags_jp(f1) && scan.match(",") && parse_expr(expr1)
            && scan.at_end()) {
        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * f1);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // jp|call NN
    scan.set_pos(p1);
    if (parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0xc3 + 10 * op);
        add_word_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_rst(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    Expr expr1(loc);

    if (scan.match("RST") && parse_expr(expr1) && scan.at_end()) {
        int n = eval_const_expr(expr1);
        switch (n) {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            asm_line.bytes.push_back(0xc7 + n);
            return true;
        }
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_ret(AsmLine& asm_line) {
    int f1;

    scan.skip_spaces();
    const char* p0 = scan.pos();

    if (scan.match("RET") && parse_flags_jp(f1) && scan.at_end()) {
        asm_line.bytes.push_back(0xc0 + 8 * f1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_jr(AsmLine& asm_line) {
    Expr expr1(loc);
    int f1;

    scan.skip_spaces();
    const char* p0 = scan.pos();

    // djnz NN
    scan.set_pos(p0);
    if (scan.match("DJNZ") && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0x10);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr f, NN
    scan.set_pos(p0);
    if (scan.match("JR") && parse_flags_jr(f1) && scan.match(",")
            && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0x20 + 8 * f1);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr NN
    scan.set_pos(p0);
    if (scan.match("JR") && parse_expr(expr1) && scan.at_end()) {
        asm_line.bytes.push_back(0x18);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    return false;
}

bool Parser::parse_inc_dec(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> incdec1 = {
        std::make_pair("INC", 0),
        std::make_pair("DEC", 1),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1, pfx;
    Expr expr1(loc);

    int op = scan.match_one_of(-1, incdec1);
    if (op == -1) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // inc|dec dd
    scan.set_pos(p1);
    if (parse_dd_sp(r1) && scan.at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x03 + 8 * op + 16 * (r1 & 0xff));
        return true;
    }

    // inc|dec r
    scan.set_pos(p1);
    if (parse_r1(r1) && scan.at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * r1);
        return true;
    }

    // inc|dec (hl)
    scan.set_pos(p1);
    if (scan.match("(") && scan.match("HL") && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        return true;
    }

    // inc|dec (x+DIS)
    scan.set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_in(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1;
    Expr expr1(loc);

    if (!scan.match("IN")) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // in r, (c)
    scan.set_pos(p1);
    if (parse_r1(r1) && scan.match(",")
            && scan.match("(") && scan.match("C") && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x40 + 8 * r1);
        return true;
    }

    // in a, (n)
    scan.set_pos(p1);
    if (scan.match("A") && scan.match(",")
            && scan.match("(") && parse_expr(expr1) && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0xdb);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_out(AsmLine& asm_line) {
    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1;
    Expr expr1(loc);

    if (!scan.match("OUT")) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // out (c), r
    scan.set_pos(p1);
    if (scan.match("(") && scan.match("C") && scan.match(")") &&
            scan.match(",") && parse_r1(r1) && scan.at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x41 + 8 * r1);
        return true;
    }

    // out (n), a
    scan.set_pos(p1);
    if (scan.match("(") && parse_expr(expr1) && scan.match(")")
            && scan.match(",") && scan.match("A") && scan.at_end()) {
        asm_line.bytes.push_back(0xd3);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_im(AsmLine& asm_line) {
    Expr expr(loc);

    scan.skip_spaces();
    const char* p0 = scan.pos();

    if (scan.match("IM") && parse_expr(expr) && scan.at_end()) {
        int n = eval_const_expr(expr);
        switch (n) {
        case 0:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x46);
            return true;
        case 1:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x56);
            return true;
        case 2:
            asm_line.bytes.push_back(0xed);
            asm_line.bytes.push_back(0x5e);
            return true;
        default:
            ;
        }
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_push_pop(AsmLine& asm_line) {
    int r1;

    scan.skip_spaces();
    const char* p0 = scan.pos();

    // push dd
    scan.set_pos(p0);
    if (scan.match("PUSH") && parse_dd_af(r1) && scan.at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc5 + 16 * (r1 & 0xff));
        return true;
    }

    // pop dd
    scan.set_pos(p0);
    if (scan.match("POP") && parse_dd_af(r1) && scan.at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc1 + 16 * (r1 & 0xff));
        return true;
    }

    return false;
}

bool Parser::parse_rotate(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> rot1 = {
        std::make_pair("RLC", 0),
        std::make_pair("RRC", 1),
        std::make_pair("RL", 2),
        std::make_pair("RR", 3),
        std::make_pair("SLA", 4),
        std::make_pair("SRA", 5),
        std::make_pair("SLL", 6),
        std::make_pair("SRL", 7),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    const char* p1 = scan.pos();
    int r1, pfx;
    Expr expr1(loc);

    int op = scan.match_one_of(-1, rot1);
    if (op == -1) {
        scan.set_pos(p0);
        return false;
    }

    p1 = scan.pos();

    // {rot} r
    scan.set_pos(p1);
    if (parse_r1(r1) && scan.at_end()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + r1);
        return true;
    }

    // {rot} (hl)
    scan.set_pos(p1);
    if (scan.match("(") && scan.match("HL") && scan.match(")") && scan.at_end()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

    // {rot} (x+DIS)
    scan.set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && scan.at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

    scan.set_pos(p0);
    return false;
}

bool Parser::parse_dd_sp(int& n) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
        std::make_pair("HL", 2),
        std::make_pair("SP", 3),
        std::make_pair("IX", 0xdd02),
        std::make_pair("IY", 0xfd02),
    };
    n = scan.match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_dd_af(int& n) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
        std::make_pair("HL", 2),
        std::make_pair("AF", 3),
        std::make_pair("IX", 0xdd02),
        std::make_pair("IY", 0xfd02),
    };
    n = scan.match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_bc_de(int& n) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("BC", 0),
        std::make_pair("DE", 1),
    };
    n = scan.match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_hl_ix_iy(int& pfx) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("HL", 0),
        std::make_pair("IX", 0xdd),
        std::make_pair("IY", 0xfd),
    };
    pfx = scan.match_one_of(-1, regs);
    if (pfx != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_r1(int& n) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("B", 0),
        std::make_pair("C", 1),
        std::make_pair("D", 2),
        std::make_pair("E", 3),
        std::make_pair("H", 4),
        std::make_pair("L", 5),
        std::make_pair("A", 7),
    };
    n = scan.match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_flags_jr(int& n) {
    static const std::vector<std::pair<std::string, int>> flags = {
        std::make_pair("NZ", 0),
        std::make_pair("Z", 1),
        std::make_pair("NC", 2),
        std::make_pair("C", 3),
    };
    n = scan.match_one_of(-1, flags);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_flags_jp(int& n) {
    static const std::vector<std::pair<std::string, int>> flags = {
        std::make_pair("NZ", 0),
        std::make_pair("Z", 1),
        std::make_pair("NC", 2),
        std::make_pair("C", 3),
        std::make_pair("PO", 4),
        std::make_pair("PE", 5),
        std::make_pair("P", 6),
        std::make_pair("M", 7),
    };
    n = scan.match_one_of(-1, flags);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_x_plus_ind(int& pfx, Expr& offset) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("IX", 0xdd),
        std::make_pair("IY", 0xfd),
    };

    scan.skip_spaces();
    const char* p0 = scan.pos();
    if (!scan.match("(")) {
        scan.set_pos(p0);
        return false;
    }

    pfx = scan.match_one_of(-1, regs);
    if (pfx == -1) {
        scan.set_pos(p0);
        return false;
    }

    if (scan.match(")")) {			// no offset
        Token token;
        token.code = T_integer;
        token.ivalue = 0;
        offset.rpn.push_back(token);
        return true;
    }
    else if (parse_expr(offset) && scan.match(")")) {
        return true;
    }
    else {
        scan.set_pos(p0);
        return false;
    }
}

void Parser::add_sbyte_patch(AsmLine& asm_line, const Expr& expr) {
    Patch patch(Patch::Type::SByte, expr.loc);
    patch.expr = expr;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}

void Parser::add_byte_patch(AsmLine& asm_line, const Expr& expr) {
    Patch patch(Patch::Type::Byte, expr.loc);
    patch.expr = expr;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}

void Parser::add_word_patch(AsmLine& asm_line, const Expr& expr) {
    Patch patch(Patch::Type::Word, expr.loc);
    patch.expr = expr;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
    asm_line.bytes.push_back(0);
}

void Parser::add_jr_patch(AsmLine& asm_line, const Expr& expr) {
    Patch patch(Patch::Type::JrOffset, expr.loc);
    patch.expr = expr;
    patch.offset = INT(asm_line.bytes.size());
    asm_line.patches.push_back(patch);
    asm_line.bytes.push_back(0);
}
