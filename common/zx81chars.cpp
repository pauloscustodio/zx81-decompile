//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "types.h"
#include "utils.h"
#include "zx81chars.h"
#include <array>
#include <cstdlib>
#include <sstream>
#include <string>

static const auto zx81chars = []() {
    std::array<std::string, 256> arr;

    for (size_t i = 0; i < 256; ++i) {
        arr[i] = fmt_hex(INT(i), 2, "\\");
    }

#define X(code, name, str) arr[code] = str;
#include "zx81chars.def"
#undef X

    return arr;
}
();

std::string decode_zx81(char c) {
    std::string code = zx81chars[BYTE(c)];
    if (code.size() > 1 && isalnum_safe(code.front())) {
        code = std::string(" ") + code + " ";
    }
    return code;
}

Bytes encode_zx81(const std::string& str, const SourceLoc& loc) {
    const char* p = str.c_str();
    return encode_zx81(p, loc);
}

Bytes encode_zx81(const char*& p, const SourceLoc& loc) {
    Bytes bytes;
    while (*p != '\0') {
        if (p[0] == '\\' &&
                isxdigit_safe(p[1]) &&
                isxdigit_safe(p[2])) {
            char hex[3] = { p[1], p[2], '\0' };
            bytes.push_back(strtol(hex, NULL, 16) & 0xff);
            p += 3;
            continue;
        }

        bool encoded = false;
        for (size_t i = 0; i < zx81chars.size(); i++) {
            bool found = true;
            for (size_t j = 0; j < zx81chars[i].size(); j++) {
                if (toupper_safe(p[j]) != toupper_safe(zx81chars[i][j])) {
                    found = false;
                    break;
                }
            }
            if (found) {
                bytes.push_back(i & 0xff);
                encoded = true;
                p += zx81chars[i].size();
                break;
            }
        }
        if (!encoded) {
            error(loc, "cannot encode: " + std::string(p));
            break;
        }
    }
    return bytes;
}

std::string encode_hex(const Bytes& bytes) {
    std::ostringstream oss;
    bool first = true;
    for (auto& b : bytes) {
        if (!first) {
            oss << ",";
        }
        oss << fmt_hex(b, 2);
        first = false;
    }
    return oss.str();
}

