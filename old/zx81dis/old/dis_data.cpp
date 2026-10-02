//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "dis_data.h"
#include "errors.h"
#include "model.h"
#include <fstream>
#include <string>
#include "lexer.h"
#include <vector>
#include "scan.h"
#include <iostream>

void dis_load_dat(Prog& prog, const std::string& dat_filename) {
    std::ifstream ifs(dat_filename, std::ios::binary);
    if (!ifs) {
        std::cout << dat_filename << " not found, skipping" << std::endl;
        return;
    }

    std::string text;
    int  line_num = 0;
    while (std::getline(ifs, text)) {
        line_num++;
        SourceLoc loc(dat_filename, line_num);
        std::vector<Token> tokens;

        if (!tokenize_line(text, SourceType::BASIC, loc, tokens)) {
            continue;
        }

        if (tokens.empty()) {
            continue;
        }

        // process BASIC_LABEL
        if (tokens[0].keyword == Keyword::BASIC_LABEL) {
            if (tokens.size() >= 3 &&
                    tokens[1].type == TokenType::Integer &&
                    tokens[2].type == TokenType::Identifier) {
                int line_num = tokens[1].ivalue;
                std::string label = tokens[2].text;

                prog.dis_data.basic_labels.add(label, line_num);
                continue;
            }
            error(loc, "Expected format: BASIC_LABEL <line_number> <label> <comment>");
            continue;
        }

        // process BASIC_COMMENT
        if (tokens[0].keyword == Keyword::BASIC_COMMENT) {
            if (tokens.size() >= 2 &&
                    tokens[1].type == TokenType::Integer) {
                int line_num = tokens[1].ivalue;

                // collect comment
                std::string comment_text = tokens.size() == 2 ? "'" : "' " + text.substr(
                                               tokens[2].pos);

                // add to line comments
                prog.dis_data.basic_header.add(line_num, comment_text);
                continue;
            }
            error(loc, "Expected format: BASIC_COMMENT <line_number> <text>");
            continue;
        }

        error(loc, "Unknown directive: " + tokens[0].text);
    }
}
