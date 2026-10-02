//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "dis_analyse_bas.h"
#include "lexer.h"
#include "model.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static Line* find_line_by_number(Prog& prog, int line_num) {
    for (Line& line : prog.lines) {
        if (line.type == SourceType::BASIC && int(line.line_num) == line_num) {
            return &line;
        }
    }
    return nullptr;
}

// find labels from dis_fata.basic_labels and annotate the corresponding lines with these labels
static void find_labels(Prog& prog) {
    for (Line& line : prog.lines) {
        if (line.type != SourceType::BASIC) {
            continue;
        }

        // check if this line has a label in dis_data.basic_labels
        std::string label;
        if (prog.dis_data.basic_labels.find(line.line_num, label)) {
            line.label = label;
        }
    }
}

// find goto/gosub targets that are not labeled yet and create labels for them,
// then annotate the target lines with these labels
static void find_goto_targets(Prog& prog) {
    for (Line& line : prog.lines) {
        if (line.type != SourceType::BASIC) {
            continue;
        }

        for (const Token& token : line.tokens) {
            if (token.keyword == Keyword::GOTO ||
                    token.keyword == Keyword::GOSUB) {
                // check if next token is a line number
                size_t pos = &token - &line.tokens[0] + 1;
                if (pos < line.tokens.size() &&
                        (line.tokens[pos].type == TokenType::Float ||
                         line.tokens[pos].type == TokenType::Integer)) {
                    int target_line_num = (line.tokens[pos].type == TokenType::Float) ?
                                          int(line.tokens[pos].nvalue) :
                                          int(line.tokens[pos].ivalue);

                    // find the line with this line number
                    Line* target_line = find_line_by_number(prog, target_line_num);
                    if (!target_line) {
                        continue;
                    }

                    // find label for this line number
                    std::string label = prog.dis_data.get_basic_label(target_line_num);

                    // annotate the target line with this label
                    target_line->label = label;

                    // replace token with labe reference
                    line.tokens[pos].type = TokenType::LabelRefLine;
                    line.tokens[pos].text = "@" + label;
                    line.tokens[pos].svalue = label;
                }
            }
        }
    }
}

void dis_analyse_bas(Prog& prog) {
    find_labels(prog);
    find_goto_targets(prog);
}
