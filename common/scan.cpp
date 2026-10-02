//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "consts.h"
#include "scan.h"
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

Scan::Scan(const std::string& text) {
    set_text(text);
}

void Scan::set_text(const std::string& text_) {
    text = text_;
    p = text.c_str();
}

const char*& Scan::pos() {
    return p;
}

void Scan::skip_spaces() {
    while (*p != '\0' && isspace(*p)) {
        p++;
    }
}

bool Scan::at_end(char comment_char) {
    skip_spaces();
    if (*p == '\0' || *p == comment_char) {
        return true;
    }
    else {
        return false;
    }
}

bool Scan::parse_integer(int& value) {
    skip_spaces();
    const char* p0 = p;

    // $hhhh
    p = p0;
    if (*p == '$') {
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

    // dhhhH
    p = p0;
    if (isdigit(*p)) {
        while (isxdigit(*p)) {
            p++;
        }
        if (toupper(*p) == 'H') {
            p++;
            value = INT(strtol(p0, NULL, 16));
            return true;
        }
    }

    // 0xhhhh
    p = p0;
    if (p[0] == '0' && p[1] == 'x' && isxdigit(p[2])) {
        p += 2;
        while (isxdigit(*p)) {
            p++;
        }
        value = INT(strtol(p0 + 2, NULL, 16));
        return true;
    }

    // decimal
    p = p0;
    if (isdigit(*p)) {
        while (isdigit(*p)) {
            p++;
        }
        value = atoi(p0);
        return true;
    }

    p = p0;
    return false;
}

bool Scan::parse_number(double& value, std::string& value_text) {
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
    value_text = std::string(p0, p);
    return true;
}

bool Scan::parse_string(std::string& str) {
    std::string out;
    skip_spaces();
    const char* p0 = p;

    if (*p != '"') {
        return false;
    }
    p++;
    while (*p != '\0' && *p != '"') {
        if (*p == '\\' && p[1] != '\0') {
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

bool Scan::parse_ident(std::string& ident) {
    skip_spaces();
    if (!isalpha(*p) && *p != '_') {
        return false;
    }
    while (isalnum(*p) || *p == '_') {
        ident.push_back(*p++);
    }
    return true;
}

bool Scan::match(const std::string& compare) {
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

int Scan::match_one_of(int not_found_result,
                       std::vector<std::pair<std::string, int>> compare_list) {
    skip_spaces();
    for (auto& compare : compare_list) {
        if (match(compare.first)) {
            return compare.second;
        }
    }
    return not_found_result;
}
