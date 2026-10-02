//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>
#include <vector>

class Scan {
public:
    explicit Scan() = default;
    explicit Scan(const std::string& text);

    void set_text(const std::string& text);
    const char*& pos();

    void skip_spaces();
    bool at_end(char comment_char = '\0');
    bool parse_integer(int& value);
    bool parse_number(double& value, std::string& value_text);
    bool parse_string(std::string& str);
    bool parse_ident(std::string& ident);
    bool match(const std::string& compare);
    int match_one_of(int not_found_result,
                     std::vector<std::pair<std::string, int>> compare_list);

private:
    std::string text;
    const char* p = "";
};
