//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "disasm.h"
#include "errors.h"
#include "release_assert.h"
#include "types.h"
#include "utils.h"
#include "zx81consts.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

void Basic::write_bas_file(const std::string& filename,
                           const DisasmCode& disasm_code) const {
    // open file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        fatal_error("write file: " + filename);
    }

    // write components
    write_sysvars(file);
    write_dfile(file);
    write_vars(file);
    write_source_lines(file, disasm_code);
    write_labels(file, disasm_code);
    write_mem_map(file, disasm_code);
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

void Basic::write_source_lines(std::ostream& os,
                               const DisasmCode& disasm_code) const {
    for (auto& line : source_lines) {
        if (line.type == SourceLine::Type::Basic) {
            // output header for basic line
            write_header(os, line.line_num, disasm_code);

            // output label
            if (!line.label.empty()) {
                os << std::endl << "@" << line.label << ":" << std::endl;
            }

            // output basic line
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
                    os << std::endl;
                    write_asm_lines(os, line.addr + 5, token.bytes, disasm_code);
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

void Basic::write_asm_lines(std::ostream& os, int addr,
                            const Bytes& bytes,
                            const DisasmCode& disasm_code) const {
    const auto& labels = disasm_code.get_asm_labels();

    os  << "#ASM [" << fmt_hex(addr, 4) << "]" << std::endl;
    for (int i = 0; i < INT(bytes.size()); i++) {
        int addr1 = addr + i;

        // output header for asm line
        write_header(os, addr1, disasm_code);

        // output label
        std::string label;
        if (labels.find(addr1, label)) {
            os << std::endl << label << ":" << std::endl;
        }

        // output string of unknown bytes
        if (disasm_code.get_mem_type(addr1) == MemType::Unknown) {
            os << std::endl
               << std::setw(LabelWidth) << ""
               << std::setw(OpcodeWidth) << std::left << "defb"
               << fmt_hex(bytes[i], 2);
            while (i + 1 < INT(bytes.size()) &&
                    !labels.find(addr + i + 1, label) &&
                    disasm_code.get_mem_type(addr + i + 1) == MemType::Unknown) {
                i++;
                os << "," << fmt_hex(bytes[i], 2);
            }
            os << " ; " << static_cast<char>(disasm_code.get_mem_type(addr1))
               << std::endl << std::endl;
            continue;
        }

        // output opcode
        const Opcode* opcode = disasm_code.get_opcode(addr1);
        release_assert(opcode);
        std::string opcode_str = opcode->to_string(labels);

        // output comments
        std::vector<std::string> comment = disasm_code.get_comment(addr1);
        if (comment.empty()) {
            os << std::setw(LabelWidth) << ""
               << opcode_str << std::endl;
        }
        else {
            if (opcode_str.size() > OpcodeWidth + OperandsWidth) {
                os << std::setw(LabelWidth) << ""
                   << opcode_str << std::endl;
            }
            else {
                os << std::setw(LabelWidth) << ""
                   << std::setw(OpcodeWidth + OperandsWidth) << std::left
                   << opcode_str
                   << comment.front() << std::endl;
                comment.erase(comment.begin());
            }
            for (auto& c : comment) {
                os << std::setw(LabelWidth + OpcodeWidth + OperandsWidth) << ""
                   << c << std::endl;
            }
        }

        if (opcode->ends_flow) {
            os << std::endl;
        }

        i += opcode->size - 1;
    }
    os << std::endl
       << "#ENDASM [" << fmt_hex(addr + INT(bytes.size()), 4) << "]" << std::endl;
}

void Basic::write_header(std::ostream& os, int addr,
                         const DisasmCode& disasm_code) const {
    std::vector<std::string> header = disasm_code.get_header(addr);
    if (!header.empty()) {
        os << std::endl;
        for (const auto& h : header) {
            os << h << std::endl;
        }
        os << std::endl;
    }
}

void Basic::write_mem_map(std::ostream& os,
                          const DisasmCode& disasm_code) const {
    if (!has_asm_code) {
        return;
    }

    for (int addr = RAM_ADDR; addr < MEM_SIZE; addr += 64) {
        if (disasm_code.get_mem_type(addr) == MemType::Undefined) {
            break;  // reached end of loaded memory
        }
        os << "#: [" << fmt_hex(addr, 4) << "] ";
        for (int addr1 = addr; addr1 < addr + 64; addr1++) {
            if (disasm_code.get_mem_type(addr1) == MemType::Undefined) {
                break;  // reached end of loaded memory
            }
            os << static_cast<char>(disasm_code.get_mem_type(addr1));
        }
        os << std::endl;
    }

    os << std::endl;
}

void Basic::write_labels(std::ostream& os,
                         const DisasmCode& disasm_code) const {
    auto& basic_labels = disasm_code.get_basic_labels();
    auto& asm_labels = disasm_code.get_asm_labels();

    std::vector<std::pair<std::string, int>> labels;

    // show BASIC labels
    labels.clear();
    for (const auto& name : basic_labels.get_names()) {
        labels.emplace_back(name, basic_labels.get(name));
    }

    if (!labels.empty()) {
        std::sort(labels.begin(), labels.end(),
        [](const auto & a, const auto & b) {
            return a.second < b.second;
        });

        for (auto& [label, addr] : labels) {
            os << "#: " << std::setw(SymbolWidth) << std::left << label
               << " = " << std::setw(6) << std::right << addr << std::endl;
        }

        os << std::endl;
    }

    // show ASM labels
    labels.clear();
    for (const auto& name : asm_labels.get_names()) {
        labels.emplace_back(name, asm_labels.get(name));
    }

    if (!labels.empty()) {
        std::sort(labels.begin(), labels.end(),
        [](const auto & a, const auto & b) {
            return a.second < b.second;
        });

        for (auto& [label, addr] : labels) {
            os << "#: " << std::setw(SymbolWidth) << std::left << label
               << " = " << fmt_hex(addr, 4)
               << " = " << std::setw(5) << std::right  << addr << std::endl;
        }

        os << std::endl;
    }
}
