//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "utils.h"
#include <cctype>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

std::string fmt_hex(int value, int digits, const std::string& prefix) {
    std::ostringstream oss;
    oss << prefix << std::setw(digits) << std::setfill('0')
        << std::hex << std::uppercase << value;
    return oss.str();
}

std::string fmt_line_number(int value, int digits) {
    std::ostringstream oss;
    oss << std::setw(digits) << std::setfill(' ') << value;
    return oss.str();
}

std::string str_chomp(std::string str) {
    while (!str.empty() && isspace_safe(str.back())) {
        str.pop_back();
    }
    return str;
}

std::string str_replace_all(std::string str, const std::string& from,
                            const std::string& to) {
    std::string::size_type n = 0;
    while ((n = str.find(from, n)) != std::string::npos) {
        str.replace(n, from.size(), to);
        n += to.size();
    }
    return str;
}

std::string basename(const std::string& filename) {
    auto path_pos = filename.find_last_of("/\\");
    if (path_pos == std::string::npos) {
        return filename;
    }
    else {
        return filename.substr(path_pos + 1);
    }
}

std::string replace_extension(const std::string& filename,
                              const std::string& extension) {
    std::string base = basename(filename);
    auto dot_pos = base.find_last_of(".");
    if (dot_pos == std::string::npos) {
        return filename + extension;
    }
    else {
        dot_pos += filename.length() - base.length();
        return filename.substr(0, dot_pos) + extension;
    }
}

bool isdigit_safe(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool isxdigit_safe(char c) {
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool isspace_safe(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

bool isalpha_safe(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool isalnum_safe(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

bool isprint_safe(char c) {
    return std::isprint(static_cast<unsigned char>(c)) != 0;
}

bool isgraph_safe(char c) {
    return std::isgraph(static_cast<unsigned char>(c)) != 0;
}

bool ispunct_safe(char c) {
    return std::ispunct(static_cast<unsigned char>(c)) != 0;
}

bool iscntrl_safe(char c) {
    return std::iscntrl(static_cast<unsigned char>(c)) != 0;
}

bool islower_safe(char c) {
    return std::islower(static_cast<unsigned char>(c)) != 0;
}

bool isupper_safe(char c) {
    return std::isupper(static_cast<unsigned char>(c)) != 0;
}

char tolower_safe(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

char toupper_safe(char c) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}
