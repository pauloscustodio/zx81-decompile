//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic.h"
#include <unordered_map>
#include <string>
#include "labels.h"

class BasicInfo {
public:
    explicit BasicInfo(Basic* basic);
    virtual ~BasicInfo() = default;

    std::string get_label(int line_num);
    std::string get_header(int line_num);
    std::string get_comment(int line_num);

    void set_label(int line_num, const std::string& label);
    void add_header(int line_num, const std::string& line);
    void set_comment(int line_num, const std::string& text);

    BasicLabels& get_labels();
    SourceLine* get_source_line(int line_num);

private:
    Basic* basic = nullptr;
    std::unordered_map<int, SourceLine*> source_lines_map;
    std::unordered_map<int, std::string> headers;
    std::unordered_map<int, std::string> comments;
    BasicLabels basic_labels;

    void build_source_lines_map();
};
