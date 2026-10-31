//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "scan.h"
#include "types.h"
#include "utils.h"
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

Scanner::Scanner(Type type_, const std::string& text_) {
    set_text(type_, text_);
}

void Scanner::set_text(Type type_, const std::string& text_) {
    type = type_;
    text = text_;
    p = text.c_str();
}

const char* Scanner::pos() const {
    return p;
}

void Scanner::set_pos(const char* new_pos) {
    p = new_pos;
}

void Scanner::skip_spaces() {
    while (*p != '\0' && isspace_safe(*p)) {
        p++;
    }
}

bool Scanner::at_end() {
    skip_spaces();
    char comment_char = (type == Type::Asm) ? ';' : '\'';
    if (*p == '\0' || *p == comment_char) {
        return true;
    }
    else {
        return false;
    }
}

bool Scanner::parse_integer(int& value) {
    skip_spaces();
    const char* p0 = p;
    if (*p == '$') {	// hex
        if (!isxdigit_safe(p[1])) {
            return false;
        }
        p++;
        while (isxdigit_safe(*p)) {
            p++;
        }
        value = INT(strtol(p0 + 1, NULL, 16));
        return true;
    }
    else if (p[0] == '0' && p[1] == 'x') {
        if (!isxdigit_safe(p[2])) {
            return false;
        }
        p += 2;
        while (isxdigit_safe(*p)) {
            p++;
        }
        value = INT(strtol(p0 + 2, NULL, 16));
        return true;
    }
    else {				// decimal
        if (!isdigit_safe(*p)) {
            return false;
        }
        while (isdigit_safe(*p)) {
            p++;
        }
        value = atoi(p0);
        return true;
    }
}

bool Scanner::parse_number(double& value, std::string& value_text) {
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
        else if (isdigit_safe(*p)) {
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
    if (toupper_safe(*p) == 'E') {
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
    value_text = std::string(p0, p);
    return true;
}

bool Scanner::parse_string(std::string& str) {
    std::string out;
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

bool Scanner::parse_ident(std::string& ident) {
    skip_spaces();
    if (!(isalpha_safe(*p) || *p == '_')) {
        return false;
    }
    while (isalnum_safe(*p) || *p == '_') {
        ident.push_back(*p++);
    }
    return true;
}

bool Scanner::match(const std::string& compare) {
    skip_spaces();
    for (size_t i = 0; i < compare.size(); i++) {
        if (p[i] == '\0') {
            return false;
        }
        if (toupper_safe(p[i]) != toupper_safe(compare[i])) {
            return false;
        }
    }
    p += compare.size();
    return true;
}

int Scanner::match_one_of(int not_found_result,
                          const std::vector<std::pair<std::string, int>>& compare_list) {
    skip_spaces();
    for (const auto& compare : compare_list) {
        if (match(compare.first)) {
            return compare.second;
        }
    }
    return not_found_result;
}

