//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "lexer.h"
#include "scan.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

std::string token_type_name(TokenType type) {
    static std::unordered_map<TokenType, std::string> token_type_names = {
#define X(str, tt) { TokenType::tt, str },
#include "tokens.def"
#undef X
    };

    auto it = token_type_names.find(type);
    if (it == token_type_names.end()) {
        return "Unknown";
    }
    else {
        return it->second;
    }
}

Keyword lookup_keyword(const std::string& text) {
    static std::unordered_map<std::string, Keyword> keywords = {
#define X(str, kw)   { str, Keyword::kw },
#include "keywords.def"
#undef X
    };

    auto it = keywords.find(str_toupper(text));
    if (it == keywords.end()) {
        return Keyword::None;
    }
    else {
        return it->second;
    }
}

std::string keyword_name(Keyword keyword) {
    static std::unordered_map<Keyword, std::string> keyword_names = {
#define X(str, kw) { Keyword::kw, str },
#include "keywords.def"
#undef X
    };

    auto it = keyword_names.find(keyword);
    if (it == keyword_names.end()) {
        return "Unknown";
    }
    else {
        return it->second;
    }
}
std::string to_string(const std::vector<Token>& tokens) {
    std::string out;
    for (const auto& token : tokens) {
        out += token.ws_before + token.text;
    }
    return out;
}

