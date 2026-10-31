//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

class Scanner {
public:
    enum class Type { Asm, Basic };
    explicit Scanner(Type type_ = Type::Basic, const std::string& text_ = "");
    void set_text(Type type_, const std::string& text_);

    const char* pos() const;
    void set_pos(const char* new_pos);

    void skip_spaces();
    bool at_end();
    bool parse_integer(int& value);
    bool parse_number(double& value, std::string& value_text);
    bool parse_string(std::string& str);
    bool parse_ident(std::string& ident);
    bool match(const std::string& compare);
    int match_one_of(int not_found_result,
                     const std::vector<std::pair<std::string, int>>& compare_list);

private:
    Type type = Type::Basic;
    std::string text;
    const char* p{ "" };
};
