//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic_info.h"
#include "ctl_file.h"
#include "disasm.h"
#include "errors.h"
#include "release_assert.h"
#include "scan.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <vector>

bool CtlFile::load_file(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        perror(filename.c_str());
        error("read file " + filename);
        return false;
    }

    bool ok = true;
    std::string text;
    int line_num = 0;
    while (getline(ifs, text)) {
        line_num++;
        SourceLoc loc(filename, line_num);
        ok &= parse_ctl_line(text, loc);
    }

    return ok;
}

const std::vector<MemElement>& CtlFile::get_elements() const {
    return elements;
}

bool CtlFile::parse_ctl_line(const std::string& line, const SourceLoc& loc) {
    Scan s(line);
    MemElement elem;

    // empty line or comment line
    if (s.at_end('#')) {
        return true;
    }

    // get source type
    std::string source_type;
    if (!s.parse_ident(source_type)) {
        error(loc, "source type expected");
        return false;
    }
    source_type = str_toupper(source_type);
    if (source_type == "BASIC") {
        elem.source_type = MemElement::SourceType::Basic;
    }
    else if (source_type == "ASM") {
        elem.source_type = MemElement::SourceType::Asm;
    }
    else {
        error(loc, "expected BASIC or ASM");
        return false;
    }

    // comment prefix
    std::string comment_prefix = elem.source_type == MemElement::SourceType::Asm ?
                                 "; " : "' ";

    // get address or line number
    int value;
    if (!s.parse_integer(value)) {
        error(loc, "address or line number expected");
        return false;
    }
    if (elem.source_type == MemElement::SourceType::Basic) {
        elem.line_num = value;
    }
    else {
        elem.addr = value;
    }

    // get size
    if (s.match("(")) {
        if (!(s.parse_integer(elem.size) && s.match(")"))) {
            error(loc, "expected (size)");
            return false;
        }
    }

    // get type
    if (elem.source_type == MemElement::SourceType::Asm && s.match("B")) {
        elem.type = MemElement::ElementType::Defb;
    }
    else if (elem.source_type == MemElement::SourceType::Asm && s.match("W")) {
        elem.type = MemElement::ElementType::Defw;
    }
    else if (elem.source_type == MemElement::SourceType::Asm && s.match("M")) {
        elem.type = MemElement::ElementType::Defm;
    }
    else if (s.match("C")) {
        elem.type = MemElement::ElementType::Code;
    }
    else if (s.match(";")) {
        s.skip_spaces();
        elem.type = MemElement::ElementType::Comment;
        elem.comment = str_chomp(s.pos());
        elements.push_back(elem);
        return true;
    }
    else if (s.match("#")) {
        s.skip_spaces();
        elem.type = MemElement::ElementType::Comment;
        elem.header += comment_prefix + str_chomp(s.pos()) + "\n";
        elements.push_back(elem);
        return true;
    }
    else {
        error(loc, elem.source_type == MemElement::SourceType::Asm ?
              "expected B,W,M,C,;,#" : "expected C,;,#");
        return false;
    }

    // end of line
    if (s.at_end('#')) {
        elements.push_back(elem);
        return true;
    }

    // get size
    if (elem.source_type == MemElement::SourceType::Asm && s.match("(")) {
        if (!(s.parse_integer(elem.size) && s.match(")"))) {
            error(loc, "expected (size)");
            return false;
        }
    }

    // end of line
    if (s.at_end('#')) {
        elements.push_back(elem);
        return true;
    }

    // get label
    if (s.parse_ident(elem.label)) {
    }

    // get comment
    if (s.match("#")) {
        s.skip_spaces();
        elem.header += comment_prefix + str_chomp(s.pos()) + "\n";
    }
    else if (s.match(";")) {
        s.skip_spaces();
        elem.comment = str_chomp(s.pos());
    }

    elements.push_back(elem);
    return true;
}

void CtlFile::apply_elements(BasicInfo* basic_info, DisasmCode* disasm) {
    for (auto& elem : elements) {
        if (elem.source_type == MemElement::SourceType::Basic) {
            if (!elem.header.empty()) {
                basic_info->add_header(elem.line_num, elem.header);
            }
            if (!elem.comment.empty()) {
                basic_info->set_comment(elem.line_num, elem.comment);
            }
            if (!elem.label.empty()) {
                basic_info->set_label(elem.line_num, elem.label);
            }
        }
        else {
            if (!elem.header.empty()) {
                disasm->add_header(elem.addr, elem.header);
            }
            if (!elem.comment.empty()) {
                disasm->set_comment(elem.addr, elem.comment);
            }
            if (!elem.label.empty()) {
                disasm->set_label(elem.addr, elem.label);
            }
            switch (elem.type) {
            case MemElement::ElementType::Defb:
                disasm->set_defb(elem.addr, elem.size);
                break;
            case MemElement::ElementType::Defw:
                disasm->set_defw(elem.addr, elem.size);
                break;
            case MemElement::ElementType::Defm:
                disasm->set_defm(elem.addr, elem.size);
                break;
            case MemElement::ElementType::Code:
                disasm->set_code(elem.addr);
                break;
            case MemElement::ElementType::Comment:
                break;
            default:
                release_assert(0);
            }
        }
    }
}
