//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "errors.h"
#include "release_assert.h"
#include "types.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>

void Basic::write_bas_file(const std::string& filename) const {
    // open file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        fatal_error("write file: " + filename);
    }

    // write components
    write_sysvars(file);
    write_dfile(file);
    write_vars(file);
    write_source_lines(file);
}

void Basic::write_sysvars(std::ostream& os) const {
    bool need_newline = false;

    // AUTOSTART and FAST flags
    if (autostart) {
        os << "#AUTOSTART=1" << std::endl;
        os << "#AUTOSTART_LINE=" << autostart_line << std::endl;
        need_newline = true;
    }

    if (fast) {
        os << "#FAST=1" << std::endl;
        need_newline = true;
    }

    // SYSVARS
    Bytes sysvars = this->sysvars;
    while (!sysvars.empty() && sysvars.back() == 0) {
        sysvars.pop_back();
    }
    if (!sysvars.empty()) {
        os << "#SYSVARS=";
        bool first = true;
        for (auto byte : sysvars) {
            if (!first) {
                os << ",";
            }
            first = false;
            if (byte < 10) {
                os << static_cast<unsigned int>(byte);
            }
            else {
                os << fmt_hex(byte, 2);
            }
        }
        os << std::endl;
        need_newline = true;
    }

    if (need_newline) {
        os << std::endl;
    }
}

void Basic::write_dfile(std::ostream& os) const {
    bool need_newline = false;

    // DFILE_COLLAPSED and DFILE lines
    if (dfile_collapsed) {
        os << "#DFILE_COLLAPSED=1" << std::endl;
        need_newline = true;
    }
    for (const auto& line : dfile_lines) {
        os << "#DFILE=\"" << line << "\"" << std::endl;
        need_newline = true;
    }

    if (need_newline) {
        os << std::endl;
    }
}

void Basic::write_vars(std::ostream& os) const {
    int num_elems = 0;
    int last_dim = 0;

    for (auto& var : basic_vars) {
        switch (var.type) {
        case BasicVar::Type::Number:
            os << "#VARS " << var.name << "=" << var.value << std::endl;
            break;
        case BasicVar::Type::ArrayNumbers:
            num_elems = 1;
            os << "#VARS " << var.name << "(";

            for (size_t i = 0; i < var.dims.size(); i++) {
                if (i > 0) {
                    os << ",";
                }
                os << var.dims[i];
                num_elems *= var.dims[i];
            }

            os << ")=";

            for (int i = 0; i < num_elems; i++) {
                if (i > 0) {
                    os << ",";
                }
                os << var.values[i];
            }

            os << std::endl;
            break;
        case BasicVar::Type::ForNextLoop:
            os << "#VARS " << var.name << "=" << var.value
               << "," << var.limit << "," << var.step << "," << var.line_num << std::endl;
            break;
        case BasicVar::Type::String:
            os << "#VARS " << var.name << "$=\"" << var.str << "\"" << std::endl;
            break;
        case BasicVar::Type::ArrayStrings:
            num_elems = 1;
            os << "#VARS " << var.name << "$(";

            for (size_t i = 0; i < var.dims.size(); i++) {
                if (i > 0) {
                    os << ",";
                }
                os << var.dims[i];
                num_elems *= var.dims[i];
            }

            os << ")=";

            last_dim = var.dims.back();
            num_elems /= last_dim;
            for (int i = 0; i < num_elems; i++) {
                if (i > 0) {
                    os << ",";
                }
                os << "\"" << var.strs[i] << "\"";
            }

            os << std::endl;
            break;
        default:
            release_assert(0);
        }
    }

    if (!basic_vars.empty()) {
        os << std::endl;
    }
}

void Basic::write_source_lines(std::ostream& os) const {
    for (auto& line : source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            if (!line.label.empty()) {
                os << "@" << line.label << ":" << std::endl;
            }

            os << fmt_line_number(line.line_num) << " ";

            bool is_rem = !line.tokens.empty() && line.tokens[0].code == C_REM;
            for (auto& token : line.tokens) {
                switch (token.code) {
                case T_none:
                    break;
                case T_float:
                    os << token.svalue;
                    break;
                case T_string:
                    os << "\"" << token.svalue << "\"";
                    break;
                case T_ident:
                    os << token.ident;
                    break;
                case T_rem_code:
                    for (auto byte : token.bytes) {
                        os << fmt_hex(byte, 2, "\\");
                    }
                    break;
                case T_line_addr_ref:
                    os << "&" << token.ident;
                    break;
                case T_line_num_ref:
                    os << "@" << token.ident;
                    break;
                case C_space:
                    if (is_rem) {
                        os << " ";
                    }
                    else {
                        os << fmt_hex(token.code, 2, "\\");
                    }
                    break;
                case C_newline:
                    os << std::endl;
                    break;
                default:
                    release_assert(token.code < 0x100);
                    os << decode_zx81(token.code);
                }
            }
        }
    }

    if (!source_lines.empty()) {
        os << std::endl;
    }
}
