//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic_info.h"
#include "errors.h"
#include "utils.h"
#include <basic.h>
#include <string>

BasicInfo::BasicInfo(Basic* basic_)
    : basic(basic_) {
    build_source_lines_map();
}

std::string BasicInfo::get_label(int line_num) {
    auto it = source_lines_map.find(line_num);
    if (it == source_lines_map.end()) {
        return "";
    }
    else {
        std::string label = basic_labels.get_add(line_num);
        it->second->basic_line.label = label;
        return label;
    }
}

std::string BasicInfo::get_header(int line_num) {
    auto it = headers.find(line_num);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

std::string BasicInfo::get_comment(int line_num) {
    auto it = comments.find(line_num);
    if (it != comments.end()) {
        return it->second;
    }
    return "";
}

void BasicInfo::set_label(int line_num, const std::string& label) {
    auto it = source_lines_map.find(line_num);
    if (it == source_lines_map.end()) {
        error("line number " + std::to_string(line_num) + " not found");
    }
    else {
        if (!label.empty()) {
            basic_labels.add(label, line_num);
        }
        std::string defined_label = basic_labels.get_add(line_num);
        it->second->basic_line.label = defined_label;
    }
}

void BasicInfo::add_header(int line_num, const std::string& line) {
    auto it = headers.find(line_num);
    if (it != headers.end()) {
        it->second += line;
    }
    else {
        headers[line_num] = line;
    }
}

void BasicInfo::set_comment(int line_num, const std::string& text) {
    comments[line_num] = text;
}

BasicLabels& BasicInfo::get_labels() {
    return basic_labels;
}

SourceLine* BasicInfo::get_source_line(int line_num) {
    auto it = source_lines_map.find(line_num);
    if (it == source_lines_map.end()) {
        return nullptr;
    }
    else {
        return it->second;
    }
}

void BasicInfo::build_source_lines_map() {
    source_lines_map.clear();
    for (auto& line : basic->source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            source_lines_map[line.basic_line.line_num] = &line;
        }
    }
}

