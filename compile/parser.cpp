//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "expr.h"
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
        set_text(in_asm ? Scanner::Type::Asm : Scanner::Type::Basic, l0.second);
        parse_line();
    }
}

void Parser::parse_line() {
    const char* p0 = pos();

    skip_spaces();
    if (at_end()) {
        return;
    }

    if (match("#")) {
        parse_pragma_line();
        return;
    }

    if (in_asm) {
        // assembly is space sensitive, go back to the beginning of the line
        set_pos(p0);

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
    if (!parse_ident(keyword)) {
        return; // ignore
    }

    if (keyword == "VARS") {
        BasicVar basic_var(BasicVar::Type::Number, loc);
        if (parse_basic_var(basic_var)) {
            basic->basic_vars.push_back(basic_var);
        }
        return;
    }

    if (keyword == "SYSVARS" && match("=")) {
        basic->sysvars = parse_byte_list();
        return;
    }

    if (keyword == "DFILE" && match("=") && parse_string(str) && at_end()) {
        basic->dfile_lines.push_back(str);
        return;
    }

    if (keyword == "DFILE_COLLAPSED" && match("=") && parse_integer(n)
            && at_end()) {
        basic->dfile_collapsed = n ? true : false;
        return;
    }

    if (keyword == "AUTOSTART" && match("=") && parse_integer(n) && at_end()) {
        basic->autostart = n ? true : false;
        return;
    }

    if (keyword == "AUTOSTART_LINE" && match("=") && parse_integer(n) && at_end()) {
        basic->autostart_line = n;
        return;
    }

    if (keyword == "FAST" && match("=") && parse_integer(n) && at_end()) {
        basic->fast = n ? true : false;
        return;
    }

    if (keyword == "INCREMENT" && match("=") && parse_integer(n) && at_end()) {
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
    if (!parse_ident(var.name)) {
        error(loc, "identifier expected: " + std::string(pos()));
        return false;
    }

    // get string marker
    if (match("$")) {
        is_string = true;
        if (var.name.size() > 1) {
            error(loc, "name too long: " + var.name);
            return false;
        }
    }

    // get array marker and dimensions
    if (match("(")) {
        is_array = true;
        if (var.name.size() > 1) {
            error(loc, "name too long: " + var.name);
            return false;
        }

        num_elems = 1;
        do {
            int dim = 0;
            if (!parse_integer(dim)) {
                error(loc, "array dim expected: " + std::string(pos()));
                return false;
            }
            num_elems *= dim;
            var.dims.push_back(dim);
        }
        while (match(","));

        if (!match(")")) {
            error(loc, "array dim closing ')' expected: " + std::string(pos()));
            return false;
        }
    }

    // get =
    if (!match("=")) {
        error(loc, "'=' expected: " + std::string(pos()));
        return false;
    }

    // get value
    if (is_string == false && is_array == false) {
        std::string str;

        var.type = BasicVar::Type::Number;
        if (!parse_number(var.value, str)) {
            error(loc, "number expected: " + std::string(pos()));
            return false;
        }

        if (match(",")) {
            var.type = BasicVar::Type::ForNextLoop;
            if (var.name.size() > 1) {
                error(loc, "name too long: " + var.name);
                return false;
            }

            if (!parse_number(var.limit, str)) {
                error(loc, "number expected: " + std::string(pos()));
                return false;
            }
            if (!match(",")) {
                error(loc, "',' expected: " + std::string(pos()));
                return false;
            }
            if (!parse_number(var.step, str)) {
                error(loc, "number expected: " + std::string(pos()));
                return false;
            }
            if (!match(",")) {
                error(loc, "',' expected: " + std::string(pos()));
                return false;
            }
            if (!parse_integer(var.line_num)) {
                error(loc, "integer expected: " + std::string(pos()));
                return false;
            }
            if (!at_end()) {
                error(loc, "unexpected characters at end: " + std::string(pos()));
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
                if (!match(",")) {
                    error(loc, "',' expected: " + std::string(pos()));
                    return false;
                }
            }
            if (!parse_number(value, str)) {
                error(loc, "number expected: " + std::string(pos()));
                return false;
            }
            var.values.push_back(value);
        }
        if (!at_end()) {
            error(loc, "unexpected characters at end: " + std::string(pos()));
            return false;
        }
    }
    else if (is_string == true && is_array == false) {
        var.type = BasicVar::Type::String;

        if (!parse_string(var.str)) {
            error(loc, "string expected: " + std::string(pos()));
            return false;
        }
        if (!at_end()) {
            error(loc, "unexpected characters at end: " + std::string(pos()));
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
                if (!match(",")) {
                    error(loc, "',' expected: " + std::string(pos()));
                    return false;
                }
            }
            if (!parse_string(str)) {
                error(loc, "string expected: " + std::string(pos()));
                return false;
            }
            if (str.size() != static_cast<size_t>(last_dim)) {
                error(loc, "string length should be " + std::to_string(last_dim));
                return false;
            }
            var.strs.push_back(str);
        }
        if (!at_end()) {
            error(loc, "unexpected characters at end: " + std::string(pos()));
            return false;
        }
    }

    return true;
}

Bytes Parser::parse_byte_list() {
    if (at_end()) {
        return Bytes{};
    }

    Bytes out;
    while (true) {
        int value = 0;
        if (!parse_integer(value)) {
            error(loc, "integer expected: " + std::string(pos()));
            return Bytes{};
        }
        out.push_back(value);
        if (!match(",")) {
            break;
        }
    }
    if (!at_end()) {
        error(loc, "end of line expected: " + std::string(pos()));
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
        skip_spaces();
        if (at_end()) {
            break;
        }

        Token token;

        // hex \hh
        const char* p = pos();
        if (p[0] == '\\' && isxdigit_safe(p[1]) && isxdigit_safe(p[2])) {
            char hex[3] = { p[1], p[2], 0 };
            token.code = static_cast<ZX81char>(strtol(hex, NULL, 16));
            set_pos(p + 3);
            source_line.tokens.push_back(token);
            continue;
        }

        // REM
        if (match("REM")) {
            token.code = C_REM;
            source_line.tokens.push_back(token);

            // skip only one space following REM, the other are part of the comment
            const char* p = pos();
            if(isspace_safe(*p)) {
                set_pos(p + 1);
            }

            token.code = T_rem_code;
            p = pos();
            token.bytes = encode_zx81(p, loc); // advances p past the bytes
            set_pos(p);
            source_line.tokens.push_back(token);
            continue;
        }

        // floating point number, may be $hh or 0xhh
        if (parse_number(token.fvalue, token.svalue)) {
            token.code = T_float;
            source_line.tokens.push_back(token);
            continue;
        }

        // string literal
        if (parse_string(token.svalue)) {
            token.code = T_string;
            source_line.tokens.push_back(token);
            continue;
        }

        // @line
        if (parse_line_num_ref(token.ident)) {
            token.code = T_line_num_ref;
            source_line.tokens.push_back(token);
            continue;
        }

        // &line
        if (parse_line_addr_ref(token.ident)) {
            token.code = T_line_addr_ref;
            source_line.tokens.push_back(token);
            continue;
        }

        // '{', expr, '}'
        if (match("{")) {
            Expr expr(loc);
            if (!parse_expr(expr)) {
                return false;
            }
            if (!match("}")) {
                return false;
            }

            token.code = T_const_expr;
            token.expr_index = g_expr_pool.size();
            g_expr_pool.push_back(expr);
            source_line.tokens.push_back(token);
            continue;
        }

        // keywords and BASIC symbols
        int n = match_one_of(-1, keywords);
        if (n != -1) {
            token.code = static_cast<ZX81char>(n);
            source_line.tokens.push_back(token);
            continue;
        }

        // identifier, mut be after keywords to avoid matching keywords as identifiers
        if (parse_ident(token.ident)) {
            token.code = T_ident;
            source_line.tokens.push_back(token);
            continue;
        }

        error(loc, "cannot parse: " + std::string(pos()));
        break;
    }

    Token token;
    token.code = C_newline;
    source_line.tokens.push_back(token);

    return true;
}

bool Parser::parse_ref(const std::string& prefix, std::string& ident) {
    skip_spaces();
    const char* p0 = pos();
    if (match(prefix) && parse_ident(ident)) {
        return true;
    }
    else {
        set_pos(p0);
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
    skip_spaces();
    const char* p0 = pos();
    if (parse_line_num_ref(ident) && match(":")) {
        return true;
    }
    else {
        set_pos(p0);
        return false;
    }
}

void Parser::parse_label_line_num(SourceLine& source_line) {
    // get label and/or line number
    while (true) {
        if (source_line.line_num < 0 && parse_integer(source_line.line_num)) {
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
    const char* p0 = pos();

    // at end
    set_pos(p0);
    if (at_end()) {
        error(expr.loc, "expression expected");
        return false;
    }

    // number
    set_pos(p0);
    if (parse_integer(token.ivalue)) {
        token.code = T_integer;
        expr.rpn.push_back(token);
        return true;
    }
    set_pos(p0);
    if (parse_number(token.fvalue, token.svalue)
            && floor(token.fvalue) == token.fvalue) {
        token.code = T_integer;
        token.ivalue = INT(token.fvalue);
        expr.rpn.push_back(token);
        return true;
    }

    // ASMPC
    set_pos(p0);
    if (match("$")) {
        token.code = T_ASMPC;
        expr.rpn.push_back(token);
        return true;
    }
    set_pos(p0);
    if (match("ASMPC")) {
        token.code = T_ASMPC;
        expr.rpn.push_back(token);
        return true;
    }

    // symbol reference
    set_pos(p0);
    if (parse_ident(token.ident)) {
        token.code = T_symbol;
        expr.rpn.push_back(token);
        return true;
    }
    set_pos(p0);
    if (match("&") && parse_ident(token.ident)) {
        token.code = T_symbol;
        expr.rpn.push_back(token);
        return true;
    }

    // subexpression
    set_pos(p0);
    if (match("(") && parse_conditional_expr(expr) && match(")")) {
        return true;
    }

    // character literal
    set_pos(p0);
    const char* p = pos();
    if (*p == '\'') {
        p++;
        Byte b = encode_zx81_char(p, loc);
        if (*p != '\'') {
            set_pos(p0);
            return false;
        }
        set_pos(p + 1);

        token.code = T_integer;
        token.ivalue = b;
        expr.rpn.push_back(token);
        return true;
    }

    error(expr.loc, "cannot parse expression: " + std::string(p0));
    set_pos(p0);
    return false;
}

bool Parser::parse_unary_expr(Expr& expr) {
    Token token;
    const char* p0 = pos();

    // unary minus
    set_pos(p0);
    if (match("-") && parse_unary_expr(expr)) {
        token.code = T_unary_minus;
        expr.rpn.push_back(token);
        return true;
    }

    // unary plus
    set_pos(p0);
    if (match("+") && parse_unary_expr(expr)) {
        return true;
    }

    // logical not
    set_pos(p0);
    if (match("!") && parse_unary_expr(expr)) {
        token.code = T_log_not;
        expr.rpn.push_back(token);
        return true;
    }

    // binary not
    set_pos(p0);
    if (match("~") && parse_unary_expr(expr)) {
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
        if (match("*") && parse_unary_expr(expr)) {
            token.code = C_mult;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("/") && parse_unary_expr(expr)) {
            token.code = C_div;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("%") && parse_unary_expr(expr)) {
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
        if (match("+") && parse_mult_expr(expr)) {
            token.code = C_plus;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("-") && parse_mult_expr(expr)) {
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
        if (match("<<") && parse_add_expr(expr)) {
            token.code = T_left_shift;
            expr.rpn.push_back(token);
            continue;
        }
        if (match(">>") && parse_add_expr(expr)) {
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
        if (match("<=") && parse_shift_expr(expr)) {
            token.code = C_le;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("<") && parse_shift_expr(expr)) {
            token.code = C_lt;
            expr.rpn.push_back(token);
            continue;
        }
        if (match(">=") && parse_shift_expr(expr)) {
            token.code = C_ge;
            expr.rpn.push_back(token);
            continue;
        }
        if (match(">") && parse_shift_expr(expr)) {
            token.code = C_gt;
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
        if (match("==") && parse_rel_expr(expr)) {
            token.code = C_eq;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("=") && parse_rel_expr(expr)) {
            token.code = C_eq;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("<>") && parse_rel_expr(expr)) {
            token.code = C_ne;
            expr.rpn.push_back(token);
            continue;
        }
        if (match("!=") && parse_rel_expr(expr)) {
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
        const char* p0 = pos();
        if (match("&")) {
            if (*pos() == '&') {
                set_pos(p0);
            }
            else if (parse_eq_expr(expr)) {
                token.code = T_bin_and;
                expr.rpn.push_back(token);
                continue;
            }
            else {
                set_pos(p0);
            }
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
        if (match("^") && parse_bin_and_expr(expr)) {
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
        const char* p0 = pos();
        if (match("|")) {
            if (*pos() == '|') {
                set_pos(p0);
            }
            else if (parse_bin_xor_expr(expr)) {
                token.code = T_bin_or;
                expr.rpn.push_back(token);
                continue;
            }
            else {
                set_pos(p0);
            }
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
        if (match("&&") && parse_bin_or_expr(expr)) {
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
        if (match("||") && parse_log_and_expr(expr)) {
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
    if (match("?")) {
        if (!parse_conditional_expr(expr)) {
            return false;
        }
        if (!match(":")) {
            error(expr.loc, "':' expected");
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
    expr.clear();
    return parse_conditional_expr(expr);
}

bool Parser::parse_asm_line(AsmLine& asm_line) {
    if (parse_asm_label(asm_line.label)) {
        // is it EQU?
        skip_spaces();
        const char* p0 = pos();
        if (match("EQU")) {
            if (parse_expr(asm_line.equ_expr) && at_end()) {
                return true;
            }
            else {
                return false;
            }
        }
        set_pos(p0);
    }

    if (parse_defb(asm_line) && at_end()) {
        return true;
    }
    if (parse_defw(asm_line) && at_end()) {
        return true;
    }
    if (parse_defs(asm_line) && at_end()) {
        return true;
    }
    if (parse_z80_opcode(asm_line)) {
        return true;
    }
    if (at_end()) {
        return true;
    }

    error(loc, "cannot parse: " + std::string(pos()));
    return false;
}

// identifier at column 0
bool Parser::parse_asm_label(std::string& ident) {
    const char* p = pos();
    if (isalpha_safe(*p) || *p == '_') {
        parse_ident(ident);
        skip_spaces();
        match(":");						// optional colon
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_defb(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    if (match("DEFB") || match("DEFM")) {
        while (true) {
            std::string str;
            if (parse_string(str)) {
                Bytes bytes = encode_zx81(str, loc);
                for (auto& b : bytes) {
                    asm_line.bytes.push_back(b);
                }

                if (match(",")) {
                    continue;
                }
                else {
                    break;
                }
            }

            Expr expr(loc);
            if (parse_expr(expr)) {
                add_byte_patch(asm_line, expr);

                if (match(",")) {
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
    set_pos(p0);
    return false;
}

bool Parser::parse_defw(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    if (match("DEFW")) {
        while (true) {
            Expr expr(loc);
            if (!parse_expr(expr)) {
                return false;
            }
            add_word_patch(asm_line, expr);

            if (match(",")) {
                continue;
            }
            else {
                break;
            }
        }
        return true;
    }
    set_pos(p0);
    return false;
}

bool Parser::parse_defs(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    if (match("DEFS")) {
        Expr expr(loc);
        if (parse_expr(expr) && at_end()) {
            int value = eval_const_expr(expr);
            for (int i = 0; i < value; i++) {
                asm_line.bytes.push_back(0);
            }
            return true;
        }
    }
    set_pos(p0);
    return false;
}

bool Parser::parse_z80_opcode(AsmLine& asm_line) {
    if (at_end() ||
            parse_single_opcode(asm_line) ||
            parse_ld(asm_line) ||
            parse_alu(asm_line) ||
            parse_ex(asm_line) ||
            parse_bit(asm_line) ||
            parse_jp_call(asm_line) ||
            parse_rst(asm_line) ||
            parse_ret(asm_line) ||
            parse_jr(asm_line) ||
            parse_inc_dec(asm_line) ||
            parse_in(asm_line) ||
            parse_out(asm_line) ||
            parse_im(asm_line) ||
            parse_push_pop(asm_line) ||
            parse_rotate(asm_line) ||
            parse_bool(asm_line) ||
            parse_clr(asm_line) ||
            parse_ldi(asm_line) ||
            parse_ldd(asm_line) ||
            parse_neg(asm_line)) {
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
        std::make_pair("CPI", 0xeda1),
        std::make_pair("INI", 0xeda2),
        std::make_pair("OUTI", 0xeda3),
        std::make_pair("CPD", 0xeda9),
        std::make_pair("IND", 0xedaa),
        std::make_pair("OUTD", 0xedab),
    };

    skip_spaces();
    const char* p0 = pos();

    int n = match_one_of(-1, single_opcode);
    if (n != -1 && at_end()) {
        if ((n & 0xff00) != 0) {
            asm_line.bytes.push_back((n >> 8) & 0xff);
        }
        asm_line.bytes.push_back(n & 0xff);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_neg(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();

    if (!match("NEG")) {
        set_pos(p0);
        return false;
    }

    const char* p1 = pos();

    set_pos(p1);
    if (at_end() || match("A")) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x44);
        return true;
    }

    int r1, pfx;

    // neg r
    set_pos(p1);
    if (parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0xf5);         // push af
        asm_line.bytes.push_back(0x78 + r1);    // ld a, r
        asm_line.bytes.push_back(0x2f);         // cpl
        asm_line.bytes.push_back(0x47 + 8 * r1);// ld r, a
        asm_line.bytes.push_back(0x04 + 8 * r1);// inc r
        asm_line.bytes.push_back(0xf1);         // pop af
        return true;
    }

    // neg bc|de
    set_pos(p1);
    if (parse_bc_de(r1) && at_end()) {
        int hi = 2 * r1; // b or d
        int lo = 2 * r1 + 1; // c or e
        asm_line.bytes.push_back(0xf5);          // push af
        asm_line.bytes.push_back(0x78 + hi);     // ld a, b|d
        asm_line.bytes.push_back(0x2f);          // cpl
        asm_line.bytes.push_back(0x47 + 8 * hi); // ld b|d, a
        asm_line.bytes.push_back(0x78 + lo);     // ld a, c|e
        asm_line.bytes.push_back(0x2f);          // cpl
        asm_line.bytes.push_back(0x47 + 8 * lo); // ld c|e, a
        asm_line.bytes.push_back(0x03 + 16 * r1);// inc bc|de
        asm_line.bytes.push_back(0xf1);          // pop af
        return true;
    }

    // neg hl|ix|iy
    set_pos(p1);
    if (parse_hl_ix_iy(pfx) && at_end()) {
        asm_line.bytes.push_back(0xf5);        // push af
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c);        // ld a, h|ixh|iyh
        asm_line.bytes.push_back(0x2f);        // cpl
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x67);        // ld h|ixh|iyh, a
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7d);        // ld a, l|ixl|iyl
        asm_line.bytes.push_back(0x2f);        // cpl
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x6f);        // ld l|ixl|iyl, a
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x23);        // inc hl|ix|iy
        asm_line.bytes.push_back(0xf1);        // pop af
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_ld(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();

    if (!match("LD")) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    int r1, r2, pfx1, pfx2;
    Expr expr1(loc), expr2(loc);

    // ld bc', de'
    set_pos(p1);
    if (match("BC") && match("'") && match(",") && match("DE") && match("'")
            && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0x42); // ld b, d
        asm_line.bytes.push_back(0x4b); // ld c, e
        asm_line.bytes.push_back(0xd9); // exx
        return true;
    }

    // ld bc', hl'
    set_pos(p1);
    if (match("BC") && match("'") && match(",") && match("HL") && match("'")
            && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0x44); // ld b, h
        asm_line.bytes.push_back(0x4d); // ld c, l
        asm_line.bytes.push_back(0xd9); // exx
        return true;
    }

    // ld dd, dd'
    set_pos(p1);
    if (parse_dd_sp(r1) && r1 <= 2 && match(",") && parse_dd_sp(r2) && r2 < 3
            && match("'") && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0xc5 + r2 * 16); // push dd
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0xc1 + r1 * 16); // pop dd
        return true;
    }

    // ld dd', dd'
    set_pos(p1);
    if (parse_dd_sp(r1) && r1 <= 2 && match("'") && match(",") && parse_dd_sp(r2)
            && r2 < 3 && match("'") && r1 != r2 && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2) + r2 * 2);         // ld high, high
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2 + 1) + r2 * 2 + 1); // ld low, low
        asm_line.bytes.push_back(0xd9); // exx
        return true;
    }

    // ld dd, dd
    set_pos(p1);
    if (parse_dd_sp(r1) && r1 <= 2 && match(",") && parse_dd_sp(r2) && r2 < 3
            && r1 != r2 && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2) + r2 * 2);         // ld high, high
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2 + 1) + r2 * 2 + 1); // ld low, low
        return true;
    }

    // ld ix|iy, bc|de
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && parse_bc_de(r2)
            && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x60 + r2 * 2); // ld ixh|iyh, b|d
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x68 + r2 * 2 + 1); // ld ixl|iyl, c|e
        return true;
    }

    // ld ix|iy, bc'|de'|hl'
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && parse_dd_sp(r2)
            && r2 < 3
            && match("'") && at_end()) {
        asm_line.bytes.push_back(0xd9);             // exx
        asm_line.bytes.push_back(0xc5 + r2 * 16);   // push bc'|de'|hl'
        asm_line.bytes.push_back(0xd9);             // exx
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0xe1);             // pop ix|iy
        return true;
    }

    // ld ix|iy, hl
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0xe1); // pop ix|iy
        return true;
    }

    // ld dd, (hl+)
    set_pos(p1);
    if (parse_bc_de(r1) && match(",") && match("(") && match("HL") && match("+")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2 + 1) + 6); // ld low, (hl)
        asm_line.bytes.push_back(0x23);                        // inc hl
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2) + 6);     // ld high, (hl)
        asm_line.bytes.push_back(0x23);                        // inc hl
        return true;
    }

    // ld dd, (hl)
    set_pos(p1);
    if (parse_bc_de(r1) && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2 + 1) + 6); // ld low, (hl)
        asm_line.bytes.push_back(0x23);                        // inc hl
        asm_line.bytes.push_back(0x40 + 8 * (r1 * 2) + 6);     // ld high, (hl)
        asm_line.bytes.push_back(0x2b);                        // dec hl
        return true;
    }

    // ld ix|iy, (hl+)
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && match("(") && match("HL")
            && match("+") && match(")") && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(0x5e); // ld e, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x56); // ld d, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx1);  // dd|fd
        asm_line.bytes.push_back(0xe1); // pop ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ld ix|iy, (hl)
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && match("(") && match("HL")
            && match(")") && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(0x5e); // ld e, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x56); // ld d, (hl)
        asm_line.bytes.push_back(0x2b); // dec hl
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx1);  // dd|fd
        asm_line.bytes.push_back(0xe1); // pop ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ld (hl+), dd
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && match(",")
            && parse_bc_de(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x70 + (r1 * 2 + 1)); // ld (hl), low
        asm_line.bytes.push_back(0x23);                // inc hl
        asm_line.bytes.push_back(0x70 + (r1 * 2));     // ld (hl), high
        asm_line.bytes.push_back(0x23);                // inc hl
        return true;
    }

    // ld (hl), dd
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_bc_de(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x70 + (r1 * 2 + 1)); // ld (hl), low
        asm_line.bytes.push_back(0x23);                // inc hl
        asm_line.bytes.push_back(0x70 + (r1 * 2));     // ld (hl), high
        asm_line.bytes.push_back(0x2b);                // dec hl
        return true;
    }

    // ld (bc|de+), a
    set_pos(p1);
    if (match("(") && parse_bc_de(r1) && match("+") && match(")") && match(",")
            && match("A") && at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1);
        asm_line.bytes.push_back(0x03 + 16 * r1); // inc bc|de
        return true;
    }

    // ld (bc|de-), a
    set_pos(p1);
    if (match("(") && parse_bc_de(r1) && match("-") && match(")") && match(",")
            && match("A") && at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1);
        asm_line.bytes.push_back(0x0b + 16 * r1); // dec bc|de
        return true;
    }

    // ld (bc|de), a
    set_pos(p1);
    if (match("(") && parse_bc_de(r1) && match(")") && match(",") && match("A")
            && at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1);
        return true;
    }

    // ld a, (bc|de+)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_bc_de(r1) && match("+")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1);
        asm_line.bytes.push_back(0x03 + 16 * r1); // inc bc|de
        return true;
    }

    // ld a, (bc|de-)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_bc_de(r1) && match("-")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1);
        asm_line.bytes.push_back(0x0b + 16 * r1); // dec bc|de
        return true;
    }

    // ld a, (bc|de)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_bc_de(r1) && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1);
        return true;
    }

    // ld r1, r2
    set_pos(p1);
    if (parse_r1(r1) && match(",") && parse_r1(r2) && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld r1, x8
    set_pos(p1);
    if (parse_r1(r1) && r1 != 4 && r1 != 5 && match(",") && parse_x8(pfx2, r2)
            && at_end()) {
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld x8, r1
    set_pos(p1);
    if (parse_x8(pfx1, r1) && match(",") && parse_r1(r2) && r2 != 4 && r2 != 5
            && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld x8, x8
    set_pos(p1);
    if (parse_x8(pfx1, r1) && match(",") && parse_x8(pfx2, r2) && pfx1 == pfx2
            && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x40 + 8 * r1 + r2);
        return true;
    }

    // ld (de+), r
    set_pos(p1);
    if (match("(") && match("DE") && match("+") && match(")") && match(",")
            && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        if (r1 == 2 || r1 == 3) {
            asm_line.bytes.push_back(0x40 + 8 * 6 + (r1 + 2));  // r1 de->hl
        }
        else if (r1 == 4 || r1 == 5) {
            asm_line.bytes.push_back(0x40 + 8 * 6 + (r1 - 2));  // r1 hl->de
        }
        else {
            asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        }
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    // ld (de-), r
    set_pos(p1);
    if (match("(") && match("DE") && match("-") && match(")") && match(",")
            && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        if (r1 == 2 || r1 == 3) {
            asm_line.bytes.push_back(0x40 + 8 * 6 + (r1 + 2));  // r1 de->hl
        }
        else if (r1 == 4 || r1 == 5) {
            asm_line.bytes.push_back(0x40 + 8 * 6 + (r1 - 2));  // r1 hl->de
        }
        else {
            asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        }
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    // ld (de), r
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_r1(r1)
            && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x40 + 8 * 6 + swap_reg[r1]);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld (hl+), r
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && match(",")
            && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ld (hl-), r
    set_pos(p1);
    if (match("(") && match("HL") && match("-") && match(")") && match(",")
            && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ld (hl), r
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        return true;
    }

    // ld r, (de+)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("DE") && match("+")
            && match(")")
            && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x40 + 8 * swap_reg[r1] + 6);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    // ld r, (de-)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("DE") && match("-")
            && match(")")
            && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x40 + 8 * swap_reg[r1] + 6);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    // ld r, (de)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("DE") && match(")")
            && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x40 + 8 * swap_reg[r1] + 6);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld r, (hl+)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match("+")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ld r, (hl-)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match("-")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ld r, (hl)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        return true;
    }

    // ld i, a
    set_pos(p1);
    if (match("I") && match(",") && match("A") && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x47);
        return true;
    }

    // ld r, a
    set_pos(p1);
    if (match("R") && match(",") && match("A") && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4f);
        return true;
    }

    // ld a, i
    set_pos(p1);
    if (match("A") && match(",") && match("I") && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x57);
        return true;
    }

    // ld a, r
    set_pos(p1);
    if (match("A") && match(",") && match("R") && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x5f);
        return true;
    }

    // ld (de), hl
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x73); // ld (hl), e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x72); // ld (hl), d
        asm_line.bytes.push_back(0x2b); // dec hl
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld (de+), hl
    set_pos(p1);
    if (match("(") && match("DE") && match("+") && match(")") && match(",")
            && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x73); // ld (hl), e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x72); // ld (hl), d
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld hl, (de)
    set_pos(p1);
    if (match("HL") && match(",") && match("(") && match("DE") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x5e); // ld e, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x56); // ld d, (hl)
        asm_line.bytes.push_back(0x2b); // dec hl
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld hl, (hl)
    set_pos(p1);
    if (match("HL") && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af
        asm_line.bytes.push_back(0x7e); // ld a, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x66); // ld h, (hl)
        asm_line.bytes.push_back(0x6f); // ld l, a
        asm_line.bytes.push_back(0xf1); // pop af
        return true;
    }

    // ld (hl), hl
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af
        asm_line.bytes.push_back(0x7c); // ld a, h
        asm_line.bytes.push_back(0x75); // ld (hl), l
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x77); // ld (hl), a
        asm_line.bytes.push_back(0xf1); // pop af
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ld (hl+), hl
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && match(",")
            && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af
        asm_line.bytes.push_back(0x7c); // ld a, h
        asm_line.bytes.push_back(0x75); // ld (hl), l
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x77); // ld (hl), a
        asm_line.bytes.push_back(0xf1); // pop af
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ld (hl), ix|iy
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",")
            && parse_hl_ix_iy(pfx2)
            && pfx2 != 0 && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0xe5); // push ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        asm_line.bytes.push_back(0x73); // ld (hl), e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x72); // ld (hl), d
        asm_line.bytes.push_back(0x2b); // dec hl
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ld (hl+), ix|iy
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && match(",")
            && parse_hl_ix_iy(pfx2)
            && pfx2 != 0 && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0xe5); // push ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        asm_line.bytes.push_back(0x73); // ld (hl), e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x72); // ld (hl), d
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ld (x+d), r
    set_pos(p1);
    if (parse_x_plus_ind(pfx1, expr1) && match(",") && parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x40 + 8 * 6 + r1);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // ld (x+d), dd
    set_pos(p1);
    if (parse_x_plus_ind(pfx1, expr1) && match(",") && parse_dd_sp(r1) && r1 != 3
            && at_end()) {
        if (r1 <= 2) {
            static const int low_reg[3] = { 1, 3, 5 };	// c, e, l
            static const int high_reg[3] = { 0, 2, 4 };	// b, d, h
            asm_line.bytes.push_back(pfx1);
            asm_line.bytes.push_back(0x40 + 8 * 6 + low_reg[r1]);
            add_sbyte_patch(asm_line, expr1);
            asm_line.bytes.push_back(pfx1);
            asm_line.bytes.push_back(0x40 + 8 * 6 + high_reg[r1]);
            add_sbyte_patch(asm_line, expr1.plus_n(1));
        }
        else {	// ix, iy
            int pfx_other = (r1 >> 8) & 0xff;
            asm_line.bytes.push_back(pfx_other);
            asm_line.bytes.push_back(0xe5);		// push ix|iy
            asm_line.bytes.push_back(0xe3);		// ex (sp), hl
            asm_line.bytes.push_back(pfx1);
            asm_line.bytes.push_back(0x40 + 8 * 6 + 5);	// ld (x+d), l
            add_sbyte_patch(asm_line, expr1);
            asm_line.bytes.push_back(pfx1);
            asm_line.bytes.push_back(0x40 + 8 * 6 + 4);	// ld (x+d), h
            add_sbyte_patch(asm_line, expr1.plus_n(1));
            asm_line.bytes.push_back(0xe3);		// ex (sp), hl
            asm_line.bytes.push_back(pfx_other);
            asm_line.bytes.push_back(0xe1);		// pop ix|iy
        }

        return true;
    }

    // ld dd, (x+d)
    set_pos(p1);
    if (parse_dd_sp(r1) && match(",") && parse_x_plus_ind(pfx2, expr1) && r1 != 3
            && at_end()) {
        if (r1 <= 2) {
            static const int low_reg[3] = { 1, 3, 5 };	// c, e, l
            static const int high_reg[3] = { 0, 2, 4 };	// b, d, h
            asm_line.bytes.push_back(pfx2);
            asm_line.bytes.push_back(0x40 + 8 * low_reg[r1] + 6);
            add_sbyte_patch(asm_line, expr1);
            asm_line.bytes.push_back(pfx2);
            asm_line.bytes.push_back(0x40 + 8 * high_reg[r1] + 6);
            add_sbyte_patch(asm_line, expr1.plus_n(1));
        }
        else {	// ix, iy
            pfx1 = (r1 >> 8) & 0xff;
            asm_line.bytes.push_back(pfx1);		// push ix
            asm_line.bytes.push_back(0xe5);
            asm_line.bytes.push_back(0xe3);		// ex (sp), hl
            asm_line.bytes.push_back(pfx2);
            asm_line.bytes.push_back(0x40 + 8 * 5 + 6);	// ld l, (x+d)
            add_sbyte_patch(asm_line, expr1);
            asm_line.bytes.push_back(pfx2);
            asm_line.bytes.push_back(0x40 + 8 * 4 + 6);	// ld h, (x+d)
            add_sbyte_patch(asm_line, expr1.plus_n(1));
            asm_line.bytes.push_back(0xe3);		// ex (sp), hl
            asm_line.bytes.push_back(pfx1);
            asm_line.bytes.push_back(0xe1);		// pop ix|iy
        }

        return true;
    }

    // ld r, (x+d)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && parse_x_plus_ind(pfx1, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x40 + 8 * r1 + 6);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld bc|de, ix|iy
    set_pos(p1);
    if (parse_bc_de(r1) && match(",") && parse_hl_ix_iy(pfx2) && pfx2 != 0
            && at_end()) {
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0x44 + 16 * r1);	// ld b|d, ixh|iyh
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0x4d + 16 * r1);	// ld c|e, ixl|iyl
        return true;
    }

    // ld hl, ix|iy
    set_pos(p1);
    if (match("HL") && match(",") && parse_hl_ix_iy(pfx2) && pfx2 != 0
            && at_end()) {
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0xe5);	// push ix|iy
        asm_line.bytes.push_back(0xe1);	// pop hl
        return true;
    }

    // ld ix|iy, ix|iy (different registers)
    set_pos(p1);
    if (parse_hl_ix_iy(pfx1) && pfx1 != 0 && match(",") && parse_hl_ix_iy(pfx2)
            && pfx2 != 0
            && pfx1 != pfx2
            && at_end()) {
        asm_line.bytes.push_back(pfx2);
        asm_line.bytes.push_back(0xe5);	// push ix|iy
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0xe1);	// pop ix|iy
        return true;
    }

    // ld sp, x
    set_pos(p1);
    if (match("SP") && match(",") && parse_hl_ix_iy(pfx2) && at_end()) {
        if (pfx2 != 0) {
            asm_line.bytes.push_back(pfx2);
        }
        asm_line.bytes.push_back(0xf9);
        return true;
    }

    // ld de, hl+0
    set_pos(p1);
    if (match("DE") && match(",") && match("HL") && match("+") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(0x01 + 16 * 1); // ld de, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x19); // add hl, de
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0xe1); // pop hl
        return true;
    }

    // ld de, sp+0
    set_pos(p1);
    if (match("DE") && match(",") && match("SP") && match("+") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x01 + 16 * 2); // ld hl, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x39); // add hl, sp
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld de, sp
    set_pos(p1);
    if (match("DE") && match(",") && match("SP") && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x01 + 16 * 2); // ld hl, NN
        asm_line.bytes.push_back(0x00); // low byte
        asm_line.bytes.push_back(0x00); // high byte
        asm_line.bytes.push_back(0x39); // add hl, sp
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld hl, sp+0
    set_pos(p1);
    if (match("HL") && match(",") && match("SP") && match("+") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x01 + 16 * 2); // ld hl, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x39); // add hl, sp
        return true;
    }

    // ld hl, sp-0
    set_pos(p1);
    if (match("HL") && match(",") && match("SP") && match("-") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x01 + 16 * 2); // ld hl, NN
        add_word_patch(asm_line, expr1.negate());
        asm_line.bytes.push_back(0x39); // add hl, sp
        return true;
    }

    // ld hl, sp
    set_pos(p1);
    if (match("HL") && match(",") && match("SP") && at_end()) {
        asm_line.bytes.push_back(0x01 + 16 * 2); // ld hl, NN
        asm_line.bytes.push_back(0x00); // low byte
        asm_line.bytes.push_back(0x00); // high byte
        asm_line.bytes.push_back(0x39); // add hl, sp
        return true;
    }

    // ld (de+), N
    set_pos(p1);
    if (match("(") && match("DE") && match("+") && match(")") && match(",")
            && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    // ld (de-), N
    set_pos(p1);
    if (match("(") && match("DE") && match("-") && match(")") && match(",")
            && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    // ld (de), N
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        return true;
    }

    // ld (hl+), N
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && match(",")
            && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ld (hl-), N
    set_pos(p1);
    if (match("(") && match("HL") && match("-") && match(")") && match(",")
            && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ld (hl), N
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld (x+d), N
    set_pos(p1);
    if (parse_x_plus_ind(pfx1, expr1) && match(",") && parse_expr(expr2)
            && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_sbyte_patch(asm_line, expr1);
        add_byte_patch(asm_line, expr2);
        return true;
    }

    // ld dd, (NN)
    set_pos(p1);
    if (parse_dd_sp(r1) && match(",") && match("(") && parse_expr(expr1)
            && match(")") && at_end()) {
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
    set_pos(p1);
    if (parse_dd_sp(r1) && match(",") && parse_expr(expr1) && at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back(r1 >> 8);
        }
        asm_line.bytes.push_back(0x01 + 16 * (r1 & 0xff));
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld a, (NN)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_expr(expr1) && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x3a);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // ld ixl|ixh|iyl|iyh, N
    set_pos(p1);
    if (parse_x8(pfx1, r1) && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(pfx1);
        asm_line.bytes.push_back(0x06 + 8 * r1);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld r1, N
    set_pos(p1);
    if (parse_r1(r1) && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * r1);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // ld (NN), dd
    set_pos(p1);
    if (match("(") && parse_expr(expr1) && match(")") && match(",")
            && parse_dd_sp(r1) && at_end()) {
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

    // ld (NN), a
    set_pos(p1);
    if (match("(") && parse_expr(expr1) && match(")") && match(",") && match("A")
            && at_end()) {
        asm_line.bytes.push_back(0x32);
        add_word_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
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

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1, r2, pfx;
    Expr expr1(loc), expr2(loc);

    int op = match_one_of(-1, alu1);
    if (op == -1) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // add hl|ix|iy, dd
    set_pos(p1);
    if (op == 0 && parse_dd_sp(r1) && (r1 & 0xff) == 2 && match(",")
            && parse_dd_sp(r2) && at_end()) {
        if ((r2 & 0xff) == 2 && (r1 & 0xff00) != (r2 & 0xff00)) {
            set_pos(p0);
            return false;
        }
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x09 + 16 * (r2 & 0xff));
        return true;
    }

    // add bc, NN
    set_pos(p1);
    if (op == 0 && match("BC") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(0x21); // ld hl, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x09); // add hl, bc
        asm_line.bytes.push_back(0x44); // ld b, h
        asm_line.bytes.push_back(0x4d); // ld c, l
        asm_line.bytes.push_back(0xe1); // pop hl
        return true;
    }

    // add de, NN
    set_pos(p1);
    if (op == 0 && match("DE") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(0x21); // ld hl, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x19); // add hl, de
        asm_line.bytes.push_back(0x54); // ld d, h
        asm_line.bytes.push_back(0x5d); // ld e, l
        asm_line.bytes.push_back(0xe1); // pop hl
        return true;
    }

    // add hl, NN
    set_pos(p1);
    if (op == 0 && match("HL") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(0x11); // ld de, NN
        add_word_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x19); // add hl, de
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // adc hl, dd
    set_pos(p1);
    if (op == 1 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x4a + 16 * (r2 & 0xff));
        return true;
    }

    // sbc hl, dd
    set_pos(p1);
    if (op == 3 && parse_dd_sp(r1) && r1 == 2 && match(",") && parse_dd_sp(r2)
            && (r2 & 0xff00) == 0 && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x42 + 16 * (r2 & 0xff));
        return true;
    }

    // and hl/ix/iy, bc
    set_pos(p1);
    if (op == 4 && parse_hl_ix_iy(pfx) && match(",") && match("BC") && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c); // ld a, h/ixh/iyh

        asm_line.bytes.push_back(0xa0); // and b

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x67); // ld h/ixh/iyh, a

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7d); // ld a, l/ixl/iyl

        asm_line.bytes.push_back(0xa1); // and c

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x6f); // ld l/ixl/iyl, a

        asm_line.bytes.push_back(0xf1); // pop af

        return true;
    }

    // and hl/ix/iy, de
    set_pos(p1);
    if (op == 4 && parse_hl_ix_iy(pfx) && match(",") && match("DE") && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c); // ld a, h/ixh/iyh

        asm_line.bytes.push_back(0xa2); // and d

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x67); // ld h/ixh/iyh, a

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7d); // ld a, l/ixl/iyl

        asm_line.bytes.push_back(0xa3); // and e

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x6f); // ld l/ixl/iyl, a

        asm_line.bytes.push_back(0xf1); // pop af
        return true;
    }

    // or hl/ix/iy, bc|de
    set_pos(p1);
    if (op == 6 && parse_hl_ix_iy(pfx) && match(",") && parse_bc_de(r1)
            && at_end()) {
        int hi = 2 * r1; // b or d
        int lo = 2 * r1 + 1; // c or e
        asm_line.bytes.push_back(0xf5); // push af

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c); // ld a, h/ixh/iyh

        asm_line.bytes.push_back(0xb0 + hi); // or b/d

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x67); // ld h/ixh/iyh, a

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7d); // ld a, l/ixl/iyl

        asm_line.bytes.push_back(0xb0 + lo); // or c/e

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x6f); // ld l/ixl/iyl, a

        asm_line.bytes.push_back(0xf1); // pop af
        return true;
    }

    // xor hl/ix/iy, bc|de
    set_pos(p1);
    if (op == 5 && parse_hl_ix_iy(pfx) && match(",") && parse_bc_de(r1)
            && at_end()) {
        int hi = 2 * r1; // b or d
        int lo = 2 * r1 + 1; // c or e
        asm_line.bytes.push_back(0xf5); // push af

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c); // ld a, h/ixh/iyh

        asm_line.bytes.push_back(0xa8 + hi); // xor b/d

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x67); // ld h/ixh/iyh, a

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7d); // ld a, l/ixl/iyl

        asm_line.bytes.push_back(0xa8 + lo); // xor c/e

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x6f); // ld l/ixl/iyl, a

        asm_line.bytes.push_back(0xf1); // pop af
        return true;
    }

    // {alu} a, r
    set_pos(p1);
    if (match("A") && match(",") && parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} a, x8
    set_pos(p1);
    if (match("A") && match(",") && parse_x8(pfx, r1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} x8
    set_pos(p1);
    if (parse_x8(pfx, r1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} r
    set_pos(p1);
    if (parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + r1);
        return true;
    }

    // {alu} a, (hl+)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && match("HL") && match("+")
            && match(")") && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // {alu} a, (hl-)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && match("HL") && match("-")
            && match(")") && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // {alu} a, (hl)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} (hl+)
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // {alu} (hl-)
    set_pos(p1);
    if (match("(") && match("HL") && match("-") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // {alu} (hl)
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        return true;
    }

    // {alu} a, (x+DIS)
    set_pos(p1);
    if (match("A") && match(",") && parse_x_plus_ind(pfx, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // {alu} (x+DIS)
    set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x80 + 8 * op + 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    // {alu} a, N
    set_pos(p1);
    if (match("A") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    // {alu} N
    set_pos(p1);
    if (parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xc6 + 8 * op);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_ex(AsmLine& asm_line) {
    int pfx;

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();

    if (!match("EX")) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // ex (sp), x
    set_pos(p1);
    if (match("(") && match("SP") && match(")") && match(",")
            && parse_hl_ix_iy(pfx) && at_end()) {
        if (pfx) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe3);
        return true;
    }

    // ex af', af
    set_pos(p1);
    if (match("AF") && match("'") && match(",") && match("AF") && at_end()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex af, af'
    set_pos(p1);
    if (match("AF") && match(",") && match("AF") && match("'") && at_end()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex af, af
    set_pos(p1);
    if (match("AF") && match(",") && match("AF") && at_end()) {
        asm_line.bytes.push_back(0x08);
        return true;
    }

    // ex de, hl
    set_pos(p1);
    if (match("DE") && match(",") && match("HL") && at_end()) {
        asm_line.bytes.push_back(0xeb);
        return true;
    }

    // ex hl, de
    set_pos(p1);
    if (match("HL") && match(",") && match("DE") && at_end()) {
        asm_line.bytes.push_back(0xeb);
        return true;
    }

    // ex bc, hl
    set_pos(p1);
    if (match("BC") && match(",") && match("HL") && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(0xc5); // push bc
        asm_line.bytes.push_back(0xe1); // pop hl
        asm_line.bytes.push_back(0xc1); // pop bc
        return true;
    }

    // ex hl, bc
    set_pos(p1);
    if (match("HL") && match(",") && match("BC") && at_end()) {
        asm_line.bytes.push_back(0xe5); // push hl
        asm_line.bytes.push_back(0xc5); // push bc
        asm_line.bytes.push_back(0xe1); // pop hl
        asm_line.bytes.push_back(0xc1); // pop bc
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_bit(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> bit1 = {
        std::make_pair("BIT", 0),
        std::make_pair("RES", 1),
        std::make_pair("SET", 2),
    };

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1, pfx;
    Expr bit_num(loc), expr1(loc);

    int op = match_one_of(-1, bit1);
    if (op == -1) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // {bit} N, r
    set_pos(p1);
    if (parse_expr(bit_num) && match(",") && parse_r1(r1) && at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + r1);
        return true;
    }

    // {bit} N, (hl)
    set_pos(p1);
    if (parse_expr(bit_num) && match(",") && match("(") && match("HL")
            && match(")") && at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

    // {bit} N, (x+DIS)
    set_pos(p1);
    if (parse_expr(bit_num) && match(",") && parse_x_plus_ind(pfx, expr1)
            && at_end()) {
        int bit = eval_const_expr(bit_num);
        if (bit < 0 || bit > 7) {
            set_pos(p0);
            return false;
        }
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x40 + 0x40 * op + 8 * bit + 6);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_jp_call(AsmLine& asm_line) {
    static const std::vector<std::pair<std::string, int>> jpcall1 = {
        std::make_pair("JP", 0),
        std::make_pair("CALL", 1),
    };

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1, f1, pfx;
    Expr expr1(loc), expr2(loc);

    int op = match_one_of(-1, jpcall1);
    if (op == -1) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // jp (hl|ix|iy)
    set_pos(p1);
    if (op == 0 && match("(") && parse_hl_ix_iy(pfx) && match(")") && at_end()) {
        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xe9);
        return true;
    }

    // jp (bc|de)
    set_pos(p1);
    if (op == 0 && match("(") && parse_bc_de(r1) && match(")") && at_end()) {
        asm_line.bytes.push_back(0xc5 + 16 * r1); // push bc|de
        asm_line.bytes.push_back(0xc9); // ret
        return true;
    }

    // jp|call gtu, NN
    set_pos(p1);
    if (match("GTU") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(JR_Z); // jr z, +3
        asm_line.bytes.push_back(3);

        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * FLAG_GEU);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // call leu, NN
    set_pos(p1);
    if (op == 1 && match("LEU") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(JR_Z); // jr z, +2
        asm_line.bytes.push_back(2);

        asm_line.bytes.push_back(JR_NC); // jr nc, +3
        asm_line.bytes.push_back(3);

        asm_line.bytes.push_back(0xcd); // call NN
        add_word_patch(asm_line, expr1);
        return true;
    }

    // jp leu, NN
    set_pos(p1);
    if (op == 0 && match("LEU") && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * FLAG_Z); // jp z, NN
        add_word_patch(asm_line, expr1);

        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * FLAG_C); // jp c, NN
        add_word_patch(asm_line, expr1);
        return true;
    }

    // jp|call flag, NN
    set_pos(p1);
    if (parse_flags_jp(f1) && match(",") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xc2 + 2 * op + 8 * f1);
        add_word_patch(asm_line, expr1);
        return true;
    }

    // jp|call NN
    set_pos(p1);
    if (parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0xc3 + 10 * op);
        add_word_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_rst(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    Expr expr1(loc);

    if (match("RST") && parse_expr(expr1) && at_end()) {
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

    set_pos(p0);
    return false;
}

bool Parser::parse_ret(AsmLine& asm_line) {
    int f1;

    skip_spaces();
    const char* p0 = pos();

    // ret gtu
    set_pos(p0);
    if (match("RET") && match("GTU") && at_end()) {
        asm_line.bytes.push_back(JR_Z); // jr z, +1
        asm_line.bytes.push_back(1);
        asm_line.bytes.push_back(0xc0 + 8 * FLAG_GEU); // ret c
        return true;
    }

    // ret leu
    set_pos(p0);
    if (match("RET") && match("LEU") && at_end()) {
        asm_line.bytes.push_back(0xc0 + 8 * FLAG_Z); // ret z
        asm_line.bytes.push_back(0xc0 + 8 * FLAG_C); // ret c
        return true;
    }

    set_pos(p0);
    if (match("RET") && parse_flags_jp(f1) && at_end()) {
        asm_line.bytes.push_back(0xc0 + 8 * f1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_jr(AsmLine& asm_line) {
    Expr expr1(loc);
    int f1;

    skip_spaces();
    const char* p0 = pos();

    // djnz NN
    set_pos(p0);
    if (match("DJNZ") && parse_expr(expr1) && at_end()) {
        asm_line.bytes.push_back(0x10);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr gtu, NN
    set_pos(p0);
    if (match("JR") && match("GTU") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(JR_Z); // jr z, +2
        asm_line.bytes.push_back(2);

        asm_line.bytes.push_back(0x20 + 8 * FLAG_GEU);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr leu, NN
    set_pos(p0);
    if (match("JR") && match("LEU") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x20 + 8 * FLAG_Z); // jr z, NN
        add_jr_patch(asm_line, expr1.plus_n(2));    // +2 for the jr c, NN instruction

        asm_line.bytes.push_back(0x20 + 8 * FLAG_C); // jr c, NN
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr f, NN
    set_pos(p0);
    if (match("JR") && parse_flags_jr(f1) && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(JR_NZ + 8 * f1);
        add_jr_patch(asm_line, expr1);
        return true;
    }

    // jr NN
    set_pos(p0);
    if (match("JR") && parse_expr(expr1) && at_end()) {
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

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1, pfx;
    Expr expr1(loc);

    int op = match_one_of(-1, incdec1);
    if (op == -1) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // inc|dec dd
    set_pos(p1);
    if (parse_dd_sp(r1) && at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x03 + 8 * op + 16 * (r1 & 0xff));
        return true;
    }

    // inc|dec r
    set_pos(p1);
    if (parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * r1);
        return true;
    }

    // inc|dec x8
    set_pos(p1);
    if (parse_x8(pfx, r1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * r1);
        return true;
    }

    // inc|dec (hl+)
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // inc|dec (hl-)
    set_pos(p1);
    if (match("(") && match("HL") && match("-") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // inc|dec (hl)
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        return true;
    }

    // inc|dec (x+DIS)
    set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x04 + 1 * op + 8 * 6);
        add_sbyte_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_in(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1;
    Expr expr1(loc);

    if (!match("IN")) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // in r, (c)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("C") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x40 + 8 * r1);
        return true;
    }

    // in a, (n)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_expr(expr1) && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0xdb);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_out(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1;
    Expr expr1(loc);

    if (!match("OUT")) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // out (c), r
    set_pos(p1);
    if (match("(") && match("C") && match(")") && match(",") && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0x41 + 8 * r1);
        return true;
    }

    // out (n), a
    set_pos(p1);
    if (match("(") && parse_expr(expr1) && match(")") && match(",") && match("A")
            && at_end()) {
        asm_line.bytes.push_back(0xd3);
        add_byte_patch(asm_line, expr1);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_im(AsmLine& asm_line) {
    Expr expr(loc);

    skip_spaces();
    const char* p0 = pos();

    if (match("IM") && parse_expr(expr) && at_end()) {
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

    set_pos(p0);
    return false;
}

bool Parser::parse_push_pop(AsmLine& asm_line) {
    int r1;

    skip_spaces();
    const char* p0 = pos();

    // push dd
    set_pos(p0);
    if (match("PUSH") && parse_dd_af(r1) && at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc5 + 16 * (r1 & 0xff));
        return true;
    }

    // pop dd
    set_pos(p0);
    if (match("POP") && parse_dd_af(r1) && at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0xc1 + 16 * (r1 & 0xff));
        return true;
    }

    // push dd'
    set_pos(p0);
    if (match("PUSH") && parse_dd_sp(r1) && (r1 & 0xff) <= 2 && match("'")
            && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0xc5 + 16 * (r1 & 0xff)); // push dd'
        asm_line.bytes.push_back(0xd9); // exx
        return true;
    }

    // pop dd'
    set_pos(p0);
    if (match("POP") && parse_dd_sp(r1) && (r1 & 0xff) <= 2 && match("'")
            && at_end()) {
        asm_line.bytes.push_back(0xd9); // exx
        asm_line.bytes.push_back(0xc1 + 16 * (r1 & 0xff)); // pop dd'
        asm_line.bytes.push_back(0xd9); // exx
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
        std::make_pair("SLI", 6),
        std::make_pair("SLS", 6),
        std::make_pair("SRL", 7),
    };

    skip_spaces();
    const char* p0 = pos();
    const char* p1 = pos();
    int r1, pfx;
    Expr expr1(loc);

    int op = match_one_of(-1, rot1);
    if (op == -1) {
        set_pos(p0);
        return false;
    }

    p1 = pos();

    // {rot} r
    set_pos(p1);
    if (parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + r1);
        return true;
    }

    // rl bc|de|hl
    set_pos(p1);
    if (op == 2 && parse_dd_sp(r1) && r1 <= 2 && at_end()) {
        int hi = r1 * 2;     // b, d or h
        int lo = r1 * 2 + 1; // c, e or l
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x10 + lo); // rl c|e|l
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x10 + hi); // rl b|d|h
        return true;
    }

    // rr bc|de|hl
    set_pos(p1);
    if (op == 3 && parse_dd_sp(r1) && r1 <= 2 && at_end()) {
        int hi = r1 * 2;     // b, d or h
        int lo = r1 * 2 + 1; // c, e or l
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x18 + hi); // rr b|d|h
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x18 + lo); // rr c|e|l
        return true;
    }

    // sra bc|de|hl
    set_pos(p1);
    if (op == 5 && parse_dd_sp(r1) && r1 <= 2 && at_end()) {
        int hi = r1 * 2;     // b, d or h
        int lo = r1 * 2 + 1; // c, e or l
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x28 + hi); // sra b|d|h
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x18 + lo); // rr c|e|l
        return true;
    }

    // {rot} (hl)
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && at_end()) {
        asm_line.bytes.push_back(0xcb);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

    // {rot} (x+DIS)
    set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xcb);
        add_sbyte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x00 + 8 * op + 6);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_bool(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();

    int pfx;
    if (match("BOOL") && parse_hl_ix_iy(pfx) && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x7c); // ld a, h/ix

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0xb5); // or l/ix

        asm_line.bytes.push_back(JR_Z); // jr z, +3/+4
        asm_line.bytes.push_back(pfx == 0 ? 3 : 4);

        if (pfx != 0) {
            asm_line.bytes.push_back(pfx);
        }
        asm_line.bytes.push_back(0x21); // ld h/ix, NN
        asm_line.bytes.push_back(1);
        asm_line.bytes.push_back(0);

        asm_line.bytes.push_back(0xf1); // pop af
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_clr(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();

    if (!match("CLR")) {
        set_pos(p0);
        return false;
    }

    const char* p1 = pos();

    int r1, pfx;
    Expr expr1(loc);

    // clr dd
    set_pos(p1);
    if (parse_dd_sp(r1) && at_end()) {
        if ((r1 & 0xff00) != 0) {
            asm_line.bytes.push_back((r1 >> 8) & 0xff);
        }
        asm_line.bytes.push_back(0x01 + 16 * (r1 & 0xff));
        asm_line.bytes.push_back(0);
        asm_line.bytes.push_back(0);
        return true;
    }

    // clr r
    set_pos(p1);
    if (parse_r1(r1) && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * r1);
        asm_line.bytes.push_back(0);
        return true;
    }

    // clr x8
    set_pos(p1);
    if (parse_x8(pfx, r1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x06 + 8 * r1);
        asm_line.bytes.push_back(0);
        return true;
    }

    // clr (hl+)
    set_pos(p1);
    if (match("(") && match("HL") && match("+") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        asm_line.bytes.push_back(0);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // clr (hl-)
    set_pos(p1);
    if (match("(") && match("HL") && match("-") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        asm_line.bytes.push_back(0);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // clr (hl)
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && at_end()) {
        asm_line.bytes.push_back(0x06 + 8 * 6);
        asm_line.bytes.push_back(0);
        return true;
    }

    // clr (x+DIS)
    set_pos(p1);
    if (parse_x_plus_ind(pfx, expr1) && at_end()) {
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0x06 + 8 * 6);
        add_sbyte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0);
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_ldi(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();

    if (!match("LDI")) {
        set_pos(p0);
        return false;
    }

    const char* p1 = pos();

    if (at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0xa0);
        return true;
    }

    skip_spaces();
    int r1, pfx;
    Expr expr1(loc);

    // ldi (bc|de), a
    set_pos(p1);
    if (match("(") && parse_bc_de(r1) && match(")") && match(",") && match("A")
            && at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1); // ld (bc|de), a
        asm_line.bytes.push_back(0x03 + 16 * r1); // inc bc|de
        return true;
    }

    // ldi (hl), r
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x70 + r1); // ld (hl), r
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi (hl), hl
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && match("HL")
            && at_end()) {
        asm_line.bytes.push_back(0xf5); // push af
        asm_line.bytes.push_back(0x7c); // ld a, h
        asm_line.bytes.push_back(0x75); // ld (hl), l
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x77); // ld (hl), a
        asm_line.bytes.push_back(0xf1); // pop af
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi a, (bc|de)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_bc_de(r1) && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1); // ld a, (bc|de)
        asm_line.bytes.push_back(0x03 + 16 * r1); // inc bc|de
        return true;
    }

    // ldi r, (hl)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x46 + 8 * r1); // ld r, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi r, (de)
    set_pos(p1);
    if (parse_r1(r1) && r1 != 7 && match(",") && match("(") && match("DE")
            && match(")") && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x46 + 8 * swap_reg[r1]); // ld r, (hl)
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    // ldi (de), r
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_r1(r1)
            && r1 != 7 && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x70 + swap_reg[r1]); // ld (hl), r
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    // ldi bc|de, (hl)
    set_pos(p1);
    if (parse_bc_de(r1) && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        int lo = 1 + 2 * r1; // c or e
        int hi = 0 + 2 * r1; // b or d
        asm_line.bytes.push_back(0x46 + 8 * lo); // ld c|e, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x46 + 8 * hi); // ld b|d, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi ix|iy, (hl)
    set_pos(p1);
    if (parse_hl_ix_iy(pfx) && pfx != 0 && match(",") && match("(") && match("HL")
            && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(0x5e); // ld e, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x56); // ld d, (hl)
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xe1); // pop ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ldi (hl), ix|iy
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_hl_ix_iy(pfx)
            && pfx != 0 && at_end()) {
        asm_line.bytes.push_back(0xd5); // push de
        asm_line.bytes.push_back(pfx);
        asm_line.bytes.push_back(0xe5); // push ix|iy
        asm_line.bytes.push_back(0xd1); // pop de
        asm_line.bytes.push_back(0x73); // ld (hl), e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x72); // ld (hl), d
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0xd1); // pop de
        return true;
    }

    // ldi (hl), bc|de
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_bc_de(r1)
            && at_end()) {
        int lo = 1 + 2 * r1; // c or e
        int hi = 0 + 2 * r1; // b or d
        asm_line.bytes.push_back(0x70 + lo); // ld (hl), c|e
        asm_line.bytes.push_back(0x23); // inc hl
        asm_line.bytes.push_back(0x70 + hi); // ld (hl), b|d
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi (hl), N
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x36); // ld (hl), N
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x23); // inc hl
        return true;
    }

    // ldi (de), N
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x36); // ld (hl), N
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x13); // inc de
        return true;
    }

    set_pos(p0);
    return false;
}

bool Parser::parse_ldd(AsmLine& asm_line) {
    skip_spaces();
    const char* p0 = pos();

    if (!match("LDD")) {
        set_pos(p0);
        return false;
    }

    const char* p1 = pos();

    if (at_end()) {
        asm_line.bytes.push_back(0xed);
        asm_line.bytes.push_back(0xa8);
        return true;
    }

    skip_spaces();
    int r1;
    Expr expr1(loc);

    // ldd (bc|de), a
    set_pos(p1);
    if (match("(") && parse_bc_de(r1) && match(")") && match(",") && match("A")
            && at_end()) {
        asm_line.bytes.push_back(0x02 + 16 * r1); // ld (bc|de), a
        asm_line.bytes.push_back(0x0b + 16 * r1); // dec bc|de
        return true;
    }

    // ldd a, (bc|de)
    set_pos(p1);
    if (match("A") && match(",") && match("(") && parse_bc_de(r1) && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x0a + 16 * r1); // ld a, (bc|de)
        asm_line.bytes.push_back(0x0b + 16 * r1); // dec bc|de
        return true;
    }

    // ldd r, (hl)
    set_pos(p1);
    if (parse_r1(r1) && match(",") && match("(") && match("HL") && match(")")
            && at_end()) {
        asm_line.bytes.push_back(0x46 + 8 * r1); // ld r, (hl)
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ldd r, (de)
    set_pos(p1);
    if (parse_r1(r1) && r1 != 7 && match(",") && match("(") && match("DE")
            && match(")") && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x46 + 8 * swap_reg[r1]); // ld r, (hl)
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    // ldd (de), r
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_r1(r1)
            && r1 != 7 && at_end()) {
        static const int swap_reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 }; // ex de, hl swaps d<->h, e<->l
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x70 + swap_reg[r1]); // ld (hl), r
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    // ldd (hl), r
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_r1(r1)
            && at_end()) {
        asm_line.bytes.push_back(0x70 + r1); // ld (hl), r
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ldd (hl), N
    set_pos(p1);
    if (match("(") && match("HL") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0x36); // ld (hl), N
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0x2b); // dec hl
        return true;
    }

    // ldd (de), N
    set_pos(p1);
    if (match("(") && match("DE") && match(")") && match(",") && parse_expr(expr1)
            && at_end()) {
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x36); // ld (hl), N
        add_byte_patch(asm_line, expr1);
        asm_line.bytes.push_back(0xeb); // ex de, hl
        asm_line.bytes.push_back(0x1b); // dec de
        return true;
    }

    set_pos(p0);
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
    n = match_one_of(-1, regs);
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
    n = match_one_of(-1, regs);
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
    n = match_one_of(-1, regs);
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
    pfx = match_one_of(-1, regs);
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
    n = match_one_of(-1, regs);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_x8(int& pfx, int& n) {
    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("IXH", 0xdd04),
        std::make_pair("IXL", 0xdd05),
        std::make_pair("IYH", 0xfd04),
        std::make_pair("IYL", 0xfd05),
    };
    n = match_one_of(-1, regs);
    if (n != -1) {
        pfx = n >> 8;
        n &= 0xff;
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_flags_jr(int& n) {
    const char* p0 = pos();
    if (parse_flags_jp(n) && n < 4) {
        return true;
    }
    set_pos(p0);
    return false;
}

bool Parser::parse_flags_jp(int& n) {
    static const std::vector<std::pair<std::string, int>> flags = {
        std::make_pair("NZ", FLAG_NZ),
        std::make_pair("Z", FLAG_Z),
        std::make_pair("NC", FLAG_NC),
        std::make_pair("C", FLAG_C),
        std::make_pair("PO", FLAG_PO),
        std::make_pair("PE", FLAG_PE),
        std::make_pair("P", FLAG_P),
        std::make_pair("M", FLAG_M),
        std::make_pair("NE", FLAG_NE),
        std::make_pair("EQ", FLAG_EQ),
        std::make_pair("GEU", FLAG_GEU),
        std::make_pair("LTU", FLAG_LTU),
        std::make_pair("NV", FLAG_NV),
        std::make_pair("V", FLAG_V),
    };
    n = match_one_of(-1, flags);
    if (n != -1) {
        return true;
    }
    else {
        return false;
    }
}

bool Parser::parse_x_plus_ind(int& pfx, Expr& offset) {
    offset.clear();

    static const std::vector<std::pair<std::string, int>> regs = {
        std::make_pair("IX", 0xdd),
        std::make_pair("IY", 0xfd),
    };

    skip_spaces();
    const char* p0 = pos();
    if (!match("(")) {
        set_pos(p0);
        return false;
    }

    pfx = match_one_of(-1, regs);
    if (pfx == -1) {
        set_pos(p0);
        return false;
    }

    if (match(")")) {			// no offset
        Token token;
        token.code = T_integer;
        token.ivalue = 0;
        offset.rpn.push_back(token);
        return true;
    }
    else if (parse_expr(offset) && match(")")) {
        return true;
    }
    else {
        set_pos(p0);
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
