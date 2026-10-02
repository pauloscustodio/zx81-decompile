//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include <string>
#include <vector>

enum class TokenType {
    None,
#define X(str, tt) tt,
#include "tokens.def"
#undef X
};

std::string token_type_name(TokenType type);

enum class Keyword {
    None,
#define X(str, kw) kw,
#include "keywords.def"
#undef X
};

Keyword lookup_keyword(const std::string& text);
std::string keyword_name(Keyword keyword);

struct Token {
    TokenType type = TokenType::None;
    Keyword keyword = Keyword::None;
    std::string text;       // original token text
    size_t pos = 0;         // position within text
    std::string svalue;     // string contents as ASCII
    int ivalue = 0;
    double nvalue = 0.0;
    std::string ws_before;  // white space before token

    explicit Token() = default;
    explicit Token(TokenType type_, const std::string& text_)
        : type(type_), keyword(lookup_keyword(text_)), text(text_) {}
    explicit Token(TokenType type_, int value_)
        : type(type_), ivalue(value_) {}
    explicit Token(TokenType type_, double value_)
        : type(type_), nvalue(value_) {}
};

std::string to_string(const std::vector<Token>& tokens);

enum class SourceType { BASIC, ASM };

