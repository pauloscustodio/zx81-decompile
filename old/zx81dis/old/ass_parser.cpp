//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "ass_parser.h"
#include "errors.h"
#include "lexer.h"
#include "model.h"
#include "scan.h"
#include <cstdint>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

static bool parse_variable(Variable& var, const std::vector<Token>& tokens,
                           const SourceLoc& loc) {
    size_t pos = 2; // after #VARS

    // collect variable name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::Identifier) {
        error(loc, "Expected variable name after #VARS");
        return false;
    }
    var.name = tokens[pos].text;
    pos++;

    var.is_string_var = !var.name.empty() && var.name.back() == '$';
    size_t name_len = var.name.length() - (var.is_string_var ? 1 : 0);

    if (var.is_string_var || var.is_array_var || var.is_loop_var) {
        if (name_len != 1) {
            error(loc, "Invalid variable name: '" + var.name + "'");
            return false;
        }
    }

    // collect array dimensions if present
    size_t num_values = 1;
    if (pos < tokens.size() && tokens[pos].type == TokenType::LParen ) {
        var.is_array_var = true;
        pos++; // skip '('
        while (pos < tokens.size() && tokens[pos].type != TokenType::RParen) {
            if (tokens[pos].type == TokenType::Integer) {
                var.dims.push_back(tokens[pos].ivalue);
                num_values *= tokens[pos].ivalue;
            }
            else if (tokens[pos].type == TokenType::Float) {
                var.dims.push_back(static_cast<int>(tokens[pos].nvalue));
                num_values *= static_cast<int>(tokens[pos].nvalue);
            }
            else {
                error(loc, "Expected integer or float for array dimension");
                return false;
            }
            pos++; // skip number
            if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
                pos++; // skip ','
            }
            else if (tokens[pos].type != TokenType::RParen) {
                error(loc, "Expected ',' or ')' in array dimensions");
                return false;
            }
        }
        if (pos >= tokens.size() || tokens[pos].type != TokenType::RParen) {
            error(loc, "Expected ')' in array dimensions");
            return false;
        }
        pos++; // skip ')'
    }

    // equal sign is required after variable name or dimensions
    if (pos >= tokens.size() || tokens[pos].type != TokenType::Equal) {
        error(loc, "Expected '=' after variable name or dimensions");
        return false;
    }
    pos++; // skip '='

    // in string arrays, the last dimension in the size of the strings
    if (var.is_string_var && var.is_array_var && !var.dims.empty()) {
        size_t last_dim = var.dims.back();
        num_values /= last_dim;
    }

    // read num_values values
    for (size_t i = 0; i < num_values; ++i) {
        if (pos >= tokens.size()) {
            error(loc, "Expected value for variable");
            return false;
        }
        else if (var.is_string_var) {
            if (tokens[pos].type != TokenType::StringLiteral) {
                error(loc, "Expected string literal for variable");
                return false;
            }
            var.svalues.push_back(tokens[pos].svalue);
        }
        else if (tokens[pos].type == TokenType::Float) {
            var.nvalues.push_back(tokens[pos].nvalue);
        }
        else if (tokens[pos].type == TokenType::Integer) {
            var.nvalues.push_back(static_cast<double>(tokens[pos].ivalue));
        }
        else {
            error(loc, "Expected value for variable");
            return false;
        }
        pos++; // skip value

        if (i + 1 < num_values) {
            if (pos >= tokens.size() || tokens[pos].type != TokenType::Comma) {
                error(loc, "Expected ',' between values");
                return false;
            }
            pos++; // skip ','
        }
    }

    // parse optional limit and step
    if (num_values == 1 && pos < tokens.size()
            && tokens[pos].type == TokenType::Comma) {
        var.is_loop_var = true;
        pos++; // skip ','
        if (pos >= tokens.size()) {
            error(loc, "Expected limit value for loop variable");
            return false;
        }
        if (tokens[pos].type == TokenType::Float) {
            var.limit = tokens[pos].nvalue;
            pos++;
        }
        else if (tokens[pos].type == TokenType::Integer) {
            var.limit = static_cast<double>(tokens[pos].ivalue);
            pos++;
        }
        else {
            error(loc, "Expected limit value for loop variable");
            return false;
        }
        if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
            pos++; // skip ','
            if (pos >= tokens.size()) {
                error(loc, "Expected step value for loop variable");
                return false;
            }
            if (tokens[pos].type == TokenType::Float) {
                var.step = tokens[pos].nvalue;
                pos++;
            }
            else if (tokens[pos].type == TokenType::Integer) {
                var.step = static_cast<double>(tokens[pos].ivalue);
                pos++;
            }
            else {
                error(loc, "Expected step value for loop variable");
                return false;
            }
        }

        if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
            pos++; // skip ','
            if (pos >= tokens.size()) {
                error(loc, "Expected line number for loop variable");
                return false;
            }
            if (tokens[pos].type == TokenType::Integer) {
                var.line_num = tokens[pos].ivalue;
                pos++;
            }
            else {
                error(loc, "Expected line number for loop variable");
                return false;
            }
        }
    }

    if (pos < tokens.size()) {
        error(loc, "Unexpected tokens after variable definition");
        return false;
    }

    return true;
}

static void parse_directive(std::vector<Token>& tokens, SourceType& source_type,
                            const SourceLoc& loc, Prog& prog) {
    if (tokens[1].keyword == Keyword::ASM) {
        source_type = SourceType::ASM;
        if (tokens.size() > 2) {
            error(loc, "Unexpected tokens after directive");
        }
        return;
    }

    if (tokens[1].keyword == Keyword::ENDASM) {
        source_type = SourceType::BASIC;
        if (tokens.size() > 2) {
            error(loc, "Unexpected tokens after directive");
        }
        return;
    }

    if (tokens[1].keyword == Keyword::INCREMENT) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            prog.increment = tokens[3].ivalue;
            return;
        }
        else {
            error(loc, "Invalid #INCREMENT directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::SYSVARS) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            size_t pos = 3;
            while (pos < tokens.size() && tokens[pos].type == TokenType::Integer) {
                prog.sysvars.push_back(static_cast<uint8_t>(tokens[pos].ivalue));
                pos++;
                if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
                    pos++;
                }
                else {
                    break;
                }
            }
            if (pos < tokens.size()) {
                error(loc, "Unexpected tokens after directive");
            }
            return;
        }
        else {
            error(loc, "Invalid #SYSVARS directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::TRAILER) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            size_t pos = 3;
            while (pos < tokens.size() && tokens[pos].type == TokenType::Integer) {
                prog.trailer.push_back(static_cast<uint8_t>(tokens[pos].ivalue));
                pos++;
                if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
                    pos++;
                }
                else {
                    break;
                }
            }
            if (pos < tokens.size()) {
                error(loc, "Unexpected tokens after directive");
            }
            return;
        }
        else {
            error(loc, "Invalid #TRAILER directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::AUTOSTART) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            prog.autostart = tokens[3].ivalue != 0;
            return;
        }
        else {
            error(loc, "Invalid #AUTOSTART directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::AUTOSTART_LINE) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            prog.autostart_line = tokens[3].ivalue;
            return;
        }
        else {
            error(loc, "Invalid #AUTOSTART_LINE directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::FAST) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            prog.fast = tokens[3].ivalue != 0;
            return;
        }
        else {
            error(loc, "Invalid #FAST directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::VARS) {
        Variable var;
        if (parse_variable(var, tokens, loc)) {
            prog.variables.push_back(std::move(var));
            return;
        }
        else {
            error(loc, "Invalid #VARS directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::DFILE) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::StringLiteral) {
            size_t pos = 3;
            while (pos < tokens.size() && tokens[pos].type == TokenType::StringLiteral) {
                prog.dfile.push_back(tokens[pos].svalue);
                pos++;
                if (pos < tokens.size() && tokens[pos].type == TokenType::Comma) {
                    pos++;
                }
                else {
                    break;
                }
            }
            if (pos < tokens.size()) {
                error(loc, "Unexpected tokens after directive");
            }
            return;
        }
        else {
            error(loc, "Invalid #DFILE directive");
            return;
        }
    }

    if (tokens[1].keyword == Keyword::DFILE_COLAPSED) {
        if (tokens.size() >= 4 &&
                tokens[2].type == TokenType::Equal &&
                tokens[3].type == TokenType::Integer) {
            prog.dfile_colapsed = tokens[3].ivalue != 0;
            return;
        }
        else {
            error(loc, "Invalid #DFILE_COLAPSED directive");
            return;
        }
    }

    error(loc, "Unknown directive: " + tokens[1].text);
}

static void parse_line(const std::string& text, SourceType& source_type,
                       const SourceLoc& loc, Line& line, Prog& prog) {
    std::vector<Token> tokens;
    if (!tokenize_line(text, source_type, loc, tokens)) {
        return;
    }

    if (tokens.empty()) {
        return;
    }

    // directives
    if (tokens.size() >= 2 &&
            tokens[0].type == TokenType::Hash &&
            tokens[1].type == TokenType::Identifier) {
        parse_directive(tokens, source_type, loc, prog);
        return;
    }

    // ASM lines are stored as-is, no further processing
    if (source_type == SourceType::ASM) {
        line.tokens.insert(line.tokens.end(), tokens.begin(), tokens.end());
        return;
    }

    // parse BASIC line number and label
    size_t pos = 0;
    bool have_line_num = false;
    bool have_label = false;
    while (pos < tokens.size()) {
        Token& token = tokens[pos];
        if (!have_line_num && token.type == TokenType::Integer) {
            line.line_num = token.ivalue;
            have_line_num = true;
            pos++;
        }
        else if (!have_label &&
                 token.type == TokenType::LabelRefLine &&
                 pos + 1 < tokens.size() &&
                 tokens[pos + 1].type == TokenType::Colon) {
            line.label = token.svalue;
            have_label = true;
            pos += 2;
        }
        else {
            break;
        }
    }

    // collect remaining tokens
    line.tokens.insert(line.tokens.end(), tokens.begin() + pos, tokens.end());
}

void ass_parse_prog(Prog& prog, const std::string& bas_filename) {
    prog.bas_filename = bas_filename;

    std::ifstream infile(bas_filename, std::ios::binary);
    if (!infile) {
        fatal("Failed to open file: " + bas_filename);
    }

    int line_num = 0;
    std::string text;
    SourceType source_type = SourceType::BASIC;

    while (std::getline(infile, text)) {
        line_num++;
        while (!text.empty() && text.back() == '\\') {
            text.back() = ' ';

            std::string next_line;
            if (std::getline(infile, next_line)) {
                line_num++;
                text += next_line;
            }
        }
        SourceLoc loc(bas_filename, line_num);

        Line line;
        line.type = source_type;

        parse_line(text, source_type, loc, line, prog);
        if (!line.empty()) {
            prog.lines.push_back(std::move(line));
        }
    }
}

