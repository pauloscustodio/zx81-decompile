//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "ctl_file.h"
#include "errors.h"
#include "scan.h"
#include <fstream>
#include <string>

void CtlFile::load_ctl_file(const std::string& filename) {
    elems.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        fatal_error("open file: " + filename);
    }

    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        SourceLoc loc(filename, line_num);

        Scanner scan(Scanner::Type::Basic, line);
        if (scan.at_end() || scan.match("#")) {
            continue; // skip empty lines and comments
        }

        // get address/line number
        int addr = -1;
        if (!scan.parse_integer(addr)) {
            error(loc, "invalid address/line number");
            continue;
        }

        // get count
        int count = 1;
        if (scan.parse_integer(count)) {
            ; // count is optional
        }

        // get type character
        std::string text;
        scan.skip_spaces();
        CtlElement::Type type = CtlElement::Type::None;
        if (scan.match("L")) {
            type = CtlElement::Type::BasicCode;
            if (count != 1) {
                error(loc, "invalid count for code element");
                continue;
            }
        }
        else if (scan.match("C")) {
            type = CtlElement::Type::AsmCode;
            if (count != 1) {
                error(loc, "invalid count for code element");
                continue;
            }
        }
        else if (scan.match("B")) {
            type = CtlElement::Type::Byte;
        }
        else if (scan.match("W")) {
            type = CtlElement::Type::Word;
        }
        else if (scan.match("T")) {
            type = CtlElement::Type::Text;
        }
        else if (scan.match("#")) {
            type = CtlElement::Type::Header;
            if (count != 1) {
                error(loc, "invalid count for header element");
                continue;
            }
            text = scan.get_rest_of_line();
        }
        else if (scan.match(";")) {
            type = CtlElement::Type::Comment;
            if (count != 1) {
                error(loc, "invalid count for comment element");
                continue;
            }
            text = scan.get_rest_of_line();
        }
        else {
            error(loc, "invalid type character");
            continue;
        }

        // get optional label
        // comment and header have already eaten the rest of the line
        std::string label;
        if (scan.parse_ident(label)) {
            // label is optional
        }

        // ignore rest of line

        elems.push_back({ loc, type, addr, count, label, text });
    }
}
