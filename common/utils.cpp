//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "utils.h"
#include <cctype>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

std::string fmt_hex(int value, int digits) {
    std::ostringstream oss;
    oss << std::setw(digits) << std::setfill('0') << std::hex << value;
    return oss.str();
}

std::string fmt_line_number(int value, int digits) {
    std::ostringstream oss;
    oss << std::setw(digits) << std::setfill(' ') << value;
    return oss.str();
}

std::string str_chomp(std::string str) {
    while (!str.empty() && isspace(str.back())) {
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

