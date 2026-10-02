//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "ass_analyse.h"
#include "errors.h"
#include "model.h"
#include <string>
#include <unordered_map>

// if first line is ASM, insert a REM
static void insert_first_rem(Prog& prog) {
    if (!prog.lines.empty() && prog.lines.front().type == SourceType::ASM) {
        Line line;
        line.type = SourceType::BASIC;
        line.line_num = 0;
        line.tokens.push_back(Token(TokenType::Identifier, "REM"));
        prog.lines.push_back(std::move(line));
    }
}

// attach labels to the next basic line after the label
static void attach_labels(Prog& prog) {
    for (size_t i = 0; i < prog.lines.size(); ++i) {
        Line& line = prog.lines[i];
        if (line.type == SourceType::BASIC &&
                !line.label.empty() &&
                line.tokens.empty()) {
            // found a label alone in its line, attach it to the next line
            if (i + 1 < prog.lines.size() && prog.lines[i + 1].type == SourceType::BASIC) {
                Line& next_line = prog.lines[i + 1];
                if (!next_line.label.empty()) {
                    fatal("Label " + line.label +
                          " is attached to a line that already has a label: " + next_line.label);
                }
                next_line.label = line.label;

                // remove the label line
                prog.lines.erase(prog.lines.begin() + i);
                --i;
            }
            else {
                fatal("Label " + line.label + " is not attached to any BASIC line");
            }
        }
    }
}

// number all lines without numbers
static void number_lines(Prog& prog) {
    int last_line_num = -1;
    for (auto& line : prog.lines) {
        if (line.type != SourceType::BASIC ||
                line.tokens.empty() ||
                line.tokens.front().type == TokenType::Hash) {
            continue;
        }

        if (line.line_num >= 0 && line.line_num <= last_line_num) {
            fatal("Basic line " + std::to_string(line.line_num) + " out of sequence");
        }
        else if (line.line_num >= 0) {
            last_line_num = line.line_num;
        }
        else {
            last_line_num += prog.increment;
            line.line_num = last_line_num;
        }
    }
}

// patch all @label with the actual line numbers
static void patch_labels(Prog& prog) {
    // collect all labels
    std::unordered_map<std::string, int> labels;
    for (auto& line : prog.lines) {
        if (line.label.empty()) {
            continue;
        }

        auto it = labels.find(line.label);
        if (it != labels.end()) {
            fatal("Duplicate label " + line.label);
        }

        labels[line.label] = line.line_num;
    }

    // replace all labels
    for (auto& line : prog.lines) {
        for (auto& token : line.tokens) {
            if (token.type == TokenType::LabelRefLine) {
                auto it = labels.find(token.svalue);
                if (it == labels.end()) {
                    fatal("Undefined label " + token.svalue);
                }

                // replace
                token.type = TokenType::Integer;
                token.text = std::to_string(it->second);
                token.ivalue = it->second;
                token.svalue.clear();
            }
        }
    }
}

void ass_analyse_prog(Prog& prog) {
    insert_first_rem(prog);
    attach_labels(prog);
    number_lines(prog);
    patch_labels(prog);
}
