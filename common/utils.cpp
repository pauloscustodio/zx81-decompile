//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "utils.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <cmath>

bool str_ends_with(const std::string& str, const std::string& ending) {
    if (str.length() >= ending.length()) {
        return (0 == str.compare(str.length() - ending.length(), ending.length(),
                                 ending));
    }
    else {
        return false;
    }
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

std::string str_tolower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](char c) {
        return std::tolower(c);
    });
    return str;
}

std::string str_toupper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](char c) {
        return std::toupper(c);
    });
    return str;
}

std::string str_trim(const std::string& str) {
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(0, last + 1);
}

std::string fmt_hex(int value, int digits, const std::string& prefix) {
    std::ostringstream oss;
    oss << prefix << std::uppercase << std::hex
        << std::setfill('0') << std::setw(digits)
        << (value & 0xFFFF);
    return oss.str();
}

std::string fmt_line_number(int value, int digits) {
    std::ostringstream oss;
    oss << std::setfill(' ') << std::setw(digits) << value;
    return oss.str();
}

std::string fmt_double(double value, int precision) {
    std::ostringstream oss;
    if (floor(value) == value) {
        oss << value;
    }
    else {
        oss << std::setprecision(precision) << value;
    }
    return oss.str();
}

std::string str_chomp(std::string str) {
    while (!str.empty() && isspace(str.back())) {
        str.pop_back();
    }
    return str;
}