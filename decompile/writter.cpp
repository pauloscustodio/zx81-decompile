//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "basic.h"
#include "consts.h"
#include "disasm.h"
#include "errors.h"
#include "getopt.h"
#include "release_assert.h"
#include "utils.h"
#include "writter.h"
#include "zx81encode.h"
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static void append_elem(std::ostringstream& oss, size_t& col,
                        const std::string& elem) {
    if (oss.str().empty()) {
        col = 16;
    }
    else {
        oss << ",";
        col++;
    }
    if (col + elem.size() + 2 > 80) {
        oss << " \\" << std::endl << std::string(16, ' ');
        col = 16;
    }
    col += elem.size();
    oss << elem;
}

static bool is_ident_char(char c) {
    return isalnum(c) || c == '_';
}

static bool ends_with_hex_escape(const std::string& str) {
    if (str.size() < 3) {
        return false;
    }
    if (str[str.size() - 3] != '\\') {
        return false;
    }
    if (!isxdigit(str[str.size() - 2]) || !isxdigit(str[str.size() - 1])) {
        return false;
    }
    return true;
}

static std::string concat(const std::string& a, const std::string& b) {
    if (a.empty()) {
        return b;
    }
    else if (b.empty()) {
        return a;
    }
    else if (a.back() == ' ' || b.front() == ' ') {
        return a + b;
    }
    else if (ends_with_hex_escape(a)) {
        return a + b;
    }
    else if (b.front() == '@' || b.front() == '&') {
        return a + " " + b;
    }
    else if (is_ident_char(a.back()) && is_ident_char(b.front())) {
        return a + " " + b;
    }
    else {
        return a + b;
    }
}

bool BasWriter::write_bas_file(const std::string& filename) {
    ofs.open(filename, std::ios::binary);
    if (!ofs.is_open()) {
        perror(filename.c_str());
        error("write file " + filename);
        return false;
    }

    write_basic_vars();
    write_basic_lines();
    write_sysvars();
    write_video();
    write_basic_memory_map();

    return true;
}

void BasWriter::write_sysvars() {
    // SYSVARS
    std::ostringstream oss;
    size_t col = 0;
    for (int addr = SAVE_ADDR; addr < PROG; addr++) {
        append_elem(oss, col, fmt_hex(mem->peek(addr), 2));
    }
    ofs << std::setw(16) << std::left << "#SYSVARS = "
        << oss.str() << std::endl;

    // AUTOSTART
    ofs << std::setw(16) << std::left << "#AUTOSTART = "
        << basic->autostart << std::endl;
    ofs << std::setw(16) << std::left << "#AUTOSTART_LINE = "
        << basic->autostart_line_num << std::endl;

    // FAST
    ofs << std::setw(16) << std::left << "#FAST = "
        << basic->fast << std::endl;

    // WORKSPACE
    oss.str("");
    col = 0;
    for (const auto& byte : basic->workspace_bytes) {
        append_elem(oss, col, fmt_hex(byte, 2));
    }
    ofs << std::setw(16) << std::left << "#WORKSPACE = "
        << oss.str() << std::endl;

    ofs << std::endl;

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        ofs << "# [VERSN    = " << mem->peek(VERSN) << " ]" << std::endl;
        ofs << "# [E_PPC    = " << mem->dpeek(E_PPC) << " ]" << std::endl;
        ofs << "# [D_FILE   = " << fmt_hex(mem->dpeek(D_FILE), 4) << " ]" << std::endl;
        ofs << "# [DF_CC    = " << fmt_hex(mem->dpeek(DF_CC), 4) << " ]" << std::endl;
        ofs << "# [VARS     = " << fmt_hex(mem->dpeek(VARS), 4) << " ]" << std::endl;
        ofs << "# [DEST     = " << fmt_hex(mem->dpeek(DEST), 4) << " ]" << std::endl;
        ofs << "# [E_LINE   = " << fmt_hex(mem->dpeek(E_LINE), 4) << " ]" << std::endl;
        ofs << "# [CH_ADD   = " << fmt_hex(mem->dpeek(CH_ADD), 4) << " ]" << std::endl;
        ofs << "# [X_PTR    = " << fmt_hex(mem->dpeek(X_PTR), 4) << " ]" << std::endl;
        ofs << "# [STKBOT   = " << fmt_hex(mem->dpeek(STKBOT), 4) << " ]" << std::endl;
        ofs << "# [STKEND   = " << fmt_hex(mem->dpeek(STKEND), 4) << " ]" << std::endl;
        ofs << "# [BREG     = " << mem->peek(BREG) << " ]" << std::endl;
        ofs << "# [MEM      = " << fmt_hex(mem->dpeek(MEM), 4) << " ]" << std::endl;
        ofs << "# [FREE1    = " << fmt_hex(mem->peek(FREE1), 2) << " ]" << std::endl;
        ofs << "# [DF_SZ    = " << mem->peek(DF_SZ) << " ]" << std::endl;
        ofs << "# [S_TOP    = " << mem->dpeek(S_TOP) << " ]" << std::endl;
        ofs << "# [LAST_K   = " << fmt_hex(mem->dpeek(LAST_K), 4) << " ]" << std::endl;
        ofs << "# [DEBOUNCE = " << fmt_hex(mem->peek(DEBOUNCE), 2) << " ]" << std::endl;
        ofs << "# [MARGIN   = " << mem->peek(MARGIN) << " ]" << std::endl;
        ofs << "# [NXTLIN   = " << fmt_hex(mem->dpeek(NXTLIN), 4) << " ]" << std::endl;
        ofs << "# [OLDPPC   = " << mem->dpeek(OLDPPC) << " ]" << std::endl;
        ofs << "# [FLAGX    = " << fmt_hex(mem->peek(FLAGX), 2) << " ]" << std::endl;
        ofs << "# [STRLEN   = " << mem->dpeek(STRLEN) << " ]" << std::endl;
        ofs << "# [T_ADDR   = " << fmt_hex(mem->dpeek(T_ADDR), 4) << " ]" << std::endl;
        ofs << "# [SEED     = " << fmt_hex(mem->dpeek(SEED), 4) << " ]" << std::endl;
        ofs << "# [FRAMES   = " << fmt_hex(mem->dpeek(FRAMES), 4) << " ]" << std::endl;
        ofs << "# [COORDX   = " << mem->peek(COORDX) << " ]" << std::endl;
        ofs << "# [COORDY   = " << mem->peek(COORDY) << " ]" << std::endl;
        ofs << "# [PR_CC    = " << fmt_hex(mem->peek(PR_CC), 2) << " ]" << std::endl;
        ofs << "# [S_POSNC  = " << mem->peek(S_POSNC) << " ]" << std::endl;
        ofs << "# [S_POSNR  = " << mem->peek(S_POSNR) << " ]" << std::endl;
        ofs << "# [CDFLAG   = " << fmt_hex(mem->peek(CDFLAG), 2) << " ]" << std::endl;

        ofs << "# [PRBUFF   = \"";
        for (int i = 0; i < 33; i++) {
            ofs << "\\" << decode_zx81_str_char(mem->peek(PRBUFF + i));
        }
        ofs << "\" ]" << std::endl;

        for (int i = 0; i < 6; i++) {
            double value = mem->fpeek(MEM0 + i * 5);
            ofs << "# [MEM" << i << "     = " << value << " ]" << std::endl;
        }

        ofs << "# [FREE2    = " << fmt_hex(mem->dpeek(FREE2), 4) << " ]" << std::endl;
        ofs << std::endl;
    }
}

void BasWriter::write_basic_lines() {
    if (basic->source_lines.empty()) {
        return;
    }

    for (auto& line : basic->source_lines) {
        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [" << fmt_hex(line.addr, 4) << "]" << std::endl;
        }
        if (line.type == SourceLine::Type::Basic) {
            std::string header = basic_info->get_header(line.basic_line.line_num);
            if (!header.empty()) {
                ofs << std::endl;
                ofs << header;
                ofs << std::endl;
            }

            std::string label = line.basic_line.label;
            if (!label.empty()) {
                ofs << "@" << label << ":" << std::endl;
            }

            std::string text;
            bool sent_newline = false;
            text = concat(text, fmt_line_number(line.basic_line.line_num));

            for (auto& token : line.basic_line.tokens) {
                switch (token.code) {
                case T_none:
                    break;
                case T_float:
                    text = concat(text, fmt_double(token.fvalue));
                    break;
                case T_string:
                    text = concat(text, "\"" + token.svalue + "\"");
                    break;
                case T_ident:
                    text = concat(text, token.ident);
                    break;
                case T_rem_code:
                    ofs << text << std::endl;
                    text.clear();
                    write_disassembly(line.addr + 5, INT(token.bytes.size()));
                    sent_newline = true;
                    break;
                case T_line_addr_ref:
                    text = concat(text, "&" + token.ident);
                    break;
                case T_line_num_ref:
                    text = concat(text, "@" + token.ident);
                    break;
                case C_space:
                    text = concat(text, "\\00");
                    break;
                case C_newline: {
                    std::string comment = basic_info->get_comment(line.basic_line.line_num);
                    if (!comment.empty()) {
                        text += " ' " + comment;
                    }

                    if (!sent_newline) {
                        text = concat(text, "\n");
                    }
                    sent_newline = true;
                    break;
                }
                default:
                    release_assert(token.code < 0x100);
                    text = concat(text, decode_zx81(token.code));
                }
            }
            ofs << text;
        }
    }

    ofs << std::endl;
}

void BasWriter::write_disassembly(int start_addr, int size) {
    ofs << "#ASM" << std::endl;

    if (!wrote_equs) {
#define X(name, value) \
        ofs << std::setw(16) << std::left << std::string(#name) +" " << "equ     " << fmt_hex(value, 4) << std::endl;
#include "consts.def"
#undef X

        ofs << std::endl;
        wrote_equs = true;
    }

    for (int addr = start_addr; addr < start_addr + size; ) {
        Opcode* opc = disasm->get(addr);
        release_assert(opc != nullptr);
        write_opcode(opc);
        addr += opc->size;
    }

    ofs << "#ENDASM" << std::endl;
}

void BasWriter::write_opcode(Opcode* opcode) {
    // header
    std::string header = disasm->get_header(opcode->addr);
    if (!header.empty()) {
        ofs << std::endl << header << std::endl;
    }

    // label
    std::string label;
    if (disasm->get_labels().find(opcode->addr, label)) {
        ofs << label << ":" << std::endl;
    }

    // opcode
    std::ostringstream oss;
    oss << std::setw(8) << "" << std::setw(24) << std::left;
    switch (opcode->type) {
    case Opcode::Type::Undef:
    case Opcode::Type::Unknown:
        oss << decode_undef(opcode);
        break;
    case Opcode::Type::Asm:
        oss << decode_opcode(opcode);
        break;
    case Opcode::Type::Defb:
        oss << decode_defb(opcode);
        break;
    case Opcode::Type::Defw:
        oss << decode_defw(opcode);
        break;
    case Opcode::Type::Defm:
        oss << decode_defm(opcode);
        break;
    default:
        release_assert(0);
    }

    // comment
    oss << std::setw(0);
    std::string comment = disasm->get_comment(opcode->addr);
    if (!comment.empty()) {
        oss << "; " << comment;
    }
    else if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        oss << "; [" << fmt_hex(opcode->addr, 4) << "]";
    }
    oss << std::endl;

    ofs << oss.str();
}

std::string BasWriter::decode_undef(Opcode* opcode) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defb"
        << std::setw(0) << std::left << fmt_hex(mem->peek(opcode->addr), 2);
    return oss.str();
}

std::string BasWriter::decode_opcode(Opcode* opcode) {
    // get opcode
    std::string instr, args;
    auto p = opcode->opcode.find(' ');
    if (p == std::string::npos) {
        instr = opcode->opcode;
        args = "";
    }
    else {
        instr = opcode->opcode.substr(0, p);
        args = opcode->opcode.substr(p + 1);
    }

    // get arguments
    if (args.find("+DIS") != std::string::npos) {
        std::string s;
        if (opcode->dis > 0) {
            s = std::string("+") + std::to_string(opcode->dis);
        }
        else if (opcode->dis < 0) {
            s = std::to_string(opcode->dis);
        }
        else {
            s = "";
        }
        args = str_replace_all(args, "+DIS", s);
    }

    if (args.find("NN") != std::string::npos) {
        std::string s;
        int value = 0;
        std::string label;
        if (!opcode->refer_to.empty()
                && disasm->get_labels().find(opcode->refer_to, value)) {
            int offset = opcode->nn - value;
            if (offset < 0) {
                s = opcode->refer_to + "-" + fmt_hex(-offset, 4);
            }
            else if (opcode->n > 0) {
                s += opcode->refer_to + "+" + fmt_hex(offset, 4);
            }
            else {
                s = opcode->refer_to;
            }
        }
        else {
            s = decode_label(opcode->nn);
        }

        args = str_replace_all(args, "NN", s);
    }

    if (args.find("N") != std::string::npos) {
        std::string s = fmt_hex(opcode->n, 2);
        args = str_replace_all(args, "N", s);
    }

    // result
    std::ostringstream oss;
    oss << std::setw(8) << std::left << instr
        << std::setw(0) << std::left << args;
    return oss.str();
}

std::string BasWriter::decode_defb(Opcode* opcode) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defb"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : opcode->values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << fmt_hex(value, 2);
    }
    return oss.str();
}

std::string BasWriter::decode_defw(Opcode* opcode) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defw"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : opcode->values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << decode_label(value);
    }
    return oss.str();
}

std::string BasWriter::decode_defm(Opcode* opcode) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defm"
        << std::setw(0) << std::left
        << "\"";
    for (auto& c : opcode->str) {
        if (c == '"')
            oss << "\\\"";
        else {
            oss << c;
        }
    }
    oss << "\"";
    return oss.str();
}

std::string BasWriter::decode_label(int addr) {
    std::ostringstream oss;
    std::string label;
    if (disasm->get_labels().find(addr, label)) {
        oss << label;
    }
    else if (disasm->get_labels().find(addr - 1, label)) {
        oss << label << "+1";
    }
    else if (disasm->get_labels().find(addr - 2, label)) {
        oss << label << "+2";
    }
    else {
        oss << fmt_hex(addr, 4);
    }
    return oss.str();
}

void BasWriter::write_video() {
    // DFILE
    std::ostringstream oss;
    size_t col = 0;
    auto& lines = basic->video_lines;
    for (const auto& line : lines) {
        append_elem(oss, col, "\"" + line + "\"");
    }
    ofs << std::setw(16) << std::left << "#DFILE = "
        << oss.str() << std::endl;
    ofs << std::setw(16) << std::left << "#DFILE_COLLAPSED = "
        << basic->video_collapsed << std::endl;

    ofs << std::endl;
}

void BasWriter::write_basic_vars() {
    if (basic->basic_vars.empty()) {
        return;
    }

    int num_elements = 0;
    int last_dimension = 0;

    int addr = 0;
    for (auto& var : basic->basic_vars) {
        addr = var.addr;

        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [" << fmt_hex(addr, 4) << "]" << std::endl;
        }

        switch (var.type) {
        case BasicVar::Type::Number:
            ofs << "#VARS " << var.name << " = " << var.value << std::endl;
            break;
        case BasicVar::Type::ArrayNumbers:
            num_elements = 1;
            ofs << "#VARS " << var.name << "(";

            for (size_t i = 0; i < var.dimensions.size(); i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.dimensions[i];
                num_elements *= var.dimensions[i];
            }

            ofs << ") = ";

            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.values[i];
            }

            ofs << std::endl;
            break;
        case BasicVar::Type::ForNextLoop:
            ofs << "#VARS " << var.name << " = " << var.value
                << "," << var.limit << "," << var.step << "," << var.line_num << std::endl;
            break;
        case BasicVar::Type::String:
            ofs << "#VARS " << var.name << "$ = \"" << var.str << "\"" << std::endl;
            break;
        case BasicVar::Type::ArrayStrings:
            num_elements = 1;
            ofs << "#VARS " << var.name << "$(";

            for (size_t i = 0; i < var.dimensions.size(); i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.dimensions[i];
                num_elements *= var.dimensions[i];
            }

            ofs << ") = ";

            last_dimension = var.dimensions.back();
            num_elements /= last_dimension;
            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << "\"" << var.strs[i] << "\"";
            }

            ofs << std::endl;
            break;
        default:
            release_assert(0);
        }
        addr += var.size;
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        ofs << "# [" << fmt_hex(addr, 4) << "] = $" << fmt_hex(mem->peek(addr),
                2) << std::endl;
    }

    ofs << std::endl;
}

void BasWriter::write_basic_memory_map() {
    ofs << std::endl;
    int d_file = mem->dpeek(D_FILE);
    int vars = mem->dpeek(VARS);
    int e_line = mem->dpeek(E_LINE);
    for (int addr = RAM_ADDR; addr < e_line; addr += 64) {
        ofs << "# [" << fmt_hex(addr, 4) << "] ";
        for (int p = addr; p < addr + 64; p++) {
            if (p < VERSN) {
                ofs << " ";
                continue;
            }
            if (p < PROG) {
                ofs << "x";
                continue;
            }
            if (p < d_file) {
                switch (disasm->get_type(p)) {
                case Opcode::Type::Undef:
                    ofs << "b";
                    break;
                case Opcode::Type::Unknown:
                    ofs << "-";
                    break;
                case Opcode::Type::Asm:
                    ofs << "C";
                    break;
                case Opcode::Type::AsmData:
                    ofs << "C";
                    break;
                case Opcode::Type::Defb:
                    ofs << "B";
                    break;
                case Opcode::Type::DefbData:
                    ofs << "B";
                    break;
                case Opcode::Type::Defw:
                    ofs << "W";
                    break;
                case Opcode::Type::DefwData:
                    ofs << "W";
                    break;
                case Opcode::Type::Defm:
                    ofs << "T";
                    break;
                case Opcode::Type::DefmData:
                    ofs << "T";
                    break;
                default:
                    release_assert(0);
                }
                continue;
            }
            if (p < vars) {
                ofs << "d";
                continue;
            }
            if (p < e_line) {
                ofs << "v";
                continue;
            }
            ofs << " ";
        }
        ofs << std::endl;
    }

    ofs << std::endl;
}
