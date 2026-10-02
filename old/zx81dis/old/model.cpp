//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disz80.h"
#include "errors.h"
#include "lexer.h"
#include "model.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

//-----------------------------------------------------------------------------

char memory_type_char(MemoryType type) {
    switch (type) {
    case MemoryType::Undefined:
        return ' ';
    case MemoryType::Unknown:
        return '-';
    case MemoryType::Asm:
        return 'C';
    case MemoryType::AsmData:
        return 'C';
    case MemoryType::Defb:
        return 'B';
    case MemoryType::DefbData:
        return 'B';
    case MemoryType::Defw:
        return 'W';
    case MemoryType::DefwData:
        return 'W';
    case MemoryType::Defm:
        return 'T';
    case MemoryType::DefmData:
        return 'T';
    case MemoryType::System:
        return 'x';
    case MemoryType::Basic:
        return 'b';
    case MemoryType::Video:
        return 'x';
    case MemoryType::Vars:
        return 'v';
    default:
        return '?';
    }
}

//-----------------------------------------------------------------------------

Opcode::Opcode(MemoryType type_, int addr_, int size_)
    : type(type_), addr(addr_), size(size_) {}

std::string Opcode::to_string(Prog& prog) {
    std::ostringstream oss;

    // header
    auto header = prog.dis_data.asm_header.get(addr);
    if (!header.empty()) {
        oss << std::endl;
        for (auto& line : header) {
            oss << line << std::endl;
        }
    }

    // label
    std::string label;
    if (prog.dis_data.asm_labels.find(addr, label)) {
        oss << label << ":" << std::endl;
    }

    // opcode
    oss << std::setw(8) << "" << std::setw(24) << std::left;
    switch (type) {
    case MemoryType::Unknown:
        oss << decode_undef(prog);
        break;
    case MemoryType::Asm:
        oss << decode_opcode(prog);
        break;
    case MemoryType::Defb:
        oss << decode_defb(prog);
        break;
    case MemoryType::Defw:
        oss << decode_defw(prog);
        break;
    case MemoryType::Defm:
        oss << decode_defm(prog);
        break;
    default:
        error("Trying to decode opcode at non-code address: " + int16_to_hex(addr));
        return "";
    }

    // comment
    oss << "; ";
    auto comments = prog.dis_data.asm_comments.get(addr);
    if (comments.empty()) {
        oss << "[" << int16_to_hex(addr) << "]" << std::endl;
    }
    else {
        oss << comments.front() << std::endl;
        comments.erase(comments.begin());
        for (auto& comment : comments) {
            oss << std::setw(8 + 24) << "" << "; " << comment << std::endl;
        }
    }

    return oss.str();
}

std::string Opcode::decode_opcode(Prog& prog) {
    // get opcode
    std::string instr, args;
    auto p = opcode.find(' ');
    if (p == std::string::npos) {
        instr = opcode;
        args = "";
    }
    else {
        instr = opcode.substr(0, p);
        args = opcode.substr(p + 1);
    }

    // get arguments
    if (args.find("+DIS") != std::string::npos) {
        std::string s;
        if (dis > 0) {
            s = "+" + std::to_string(dis);
        }
        else if (dis < 0) {
            s = std::to_string(dis);
        }
        else {
            s = "";
        }
        args = str_replace_all(args, "+DIS", s);
    }

    if (args.find("NN") != std::string::npos) {
        std::string s;
        int value;
        std::string label;
        if (!refer_to.empty() && prog.dis_data.asm_labels.find(refer_to, value)) {
            int offset = nn - value;
            if (offset < 0) {
                s = refer_to + "-" + int16_to_hex(-offset);
            }
            else if (n > 0) {
                s += refer_to + "+" + int16_to_hex(offset);
            }
            else {
                s = refer_to;
            }
        }
        else {
            s = decode_label(prog, nn);
        }

        args = str_replace_all(args, "NN", s);
    }

    if (args.find("N") != std::string::npos) {
        std::string s = int8_to_hex(n);
        args = str_replace_all(args, "N", s);
    }

    // result
    std::ostringstream oss;
    oss << std::setw(8) << std::left << instr
        << std::setw(0) << std::left << args;
    return oss.str();
}

std::string Opcode::decode_undef(Prog& prog) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defb"
        << std::setw(0) << std::left << int8_to_hex(prog.mem.peek(addr));
    return oss.str();
}

std::string Opcode::decode_defb(Prog&) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defb"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << int8_to_hex(value);
    }
    return oss.str();
}

std::string Opcode::decode_defw(Prog& prog) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defw"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << decode_label(prog, value);
    }
    return oss.str();
}

std::string Opcode::decode_defm(Prog&) {
    std::ostringstream oss;
    oss << std::setw(8) << std::left << "defm"
        << std::setw(0) << std::left
        << "\"";
    for (auto& c : str) {
        if (c == '"')
            oss << "\\\"";
        else {
            oss << c;
        }
    }
    oss << "\"";
    return oss.str();
}

std::string Opcode::decode_label(Prog& prog, int value) {
    std::ostringstream oss;
    std::string label;
    if (prog.dis_data.asm_labels.find(value, label)) {
        oss << label;
    }
    else if (prog.dis_data.asm_labels.find(value - 1, label)) {
        oss << label << "+1";
    }
    else if (prog.dis_data.asm_labels.find(value - 2, label)) {
        oss << label << "+2";
    }
    else {
        oss << int16_to_hex(value);
    }
    return oss.str();
}

//-----------------------------------------------------------------------------

MemoryType Memory::get_type(int addr) const {
    addr &= 0xFFFF;
    if (addr < BASE_ADDR
            || addr >= BASE_ADDR + static_cast<int>(data_type.size())) {
        return MemoryType::Undefined;
    }
    return data_type[addr - BASE_ADDR];
}

void Memory::set_type(MemoryType type, int addr) {
    addr &= 0xFFFF;
    if (addr < BASE_ADDR) {
        return;
    }
    if (addr >= BASE_ADDR + static_cast<int>(data_type.size())) {
        data_type.resize(addr - BASE_ADDR + 1, MemoryType::Undefined);
    }
    data_type[addr - BASE_ADDR] = type;
    if (type == MemoryType::Unknown) {
        get_opcode(addr) = Opcode(type, addr, 1);
    }
    else if (type == MemoryType::Asm) {
        // TODO: disassemble to get size
        get_opcode(addr) = Opcode(type, addr, 1);
    }
    else if (type == MemoryType::Defb || type == MemoryType::Defw
             || type == MemoryType::Defm) {
        get_opcode(addr) = Opcode(type, addr, 1);
    }
}

void Memory::set_type(MemoryType type, int addr, int size) {
    addr &= 0xFFFF;
    for (int i = 0; i < size; ++i) {
        set_type(type, addr + i);
    }
}

Opcode& Memory::get_opcode(int addr) {
    addr &= 0xFFFF;
    if (addr < BASE_ADDR) {
        static Opcode dummy;
        return dummy;
    }
    if (addr >= BASE_ADDR + static_cast<int>(opcodes.size())) {
        opcodes.resize(addr - BASE_ADDR + 1);
    }

    return opcodes[addr - BASE_ADDR];
}

void Memory::set_unknown(int addr, int count) {
    for (int i = 0; i < 1 * count; ++i) {
        get_opcode(addr + i) = Opcode(MemoryType::Unknown, ( addr + i) & 0xFFFF, 1);
    }
    set_type(MemoryType::Unknown, addr, 1 * count);
}

void Memory::set_defb(int addr, int count) {
    get_opcode(addr) = Opcode(MemoryType::Defb, addr & 0xFFFF, 1 * count);
    auto& op = get_opcode(addr);
    for (int i = 0; i < 1 * count; ++i) {
        op.values.push_back(peek(addr + i));
    }
    set_type(MemoryType::Defb, addr, 1);
    set_type(MemoryType::DefbData, addr + 1, 1 * count - 1);
}

void Memory::set_defw(int addr, int count) {
    get_opcode(addr) = Opcode(MemoryType::Defw, addr & 0xFFFF, 2 * count);
    auto& op = get_opcode(addr);
    for (int i = 0; i < 2 * count; ++i) {
        op.values.push_back(peek(addr + i));
    }
    set_type(MemoryType::Defw, addr, 1);
    set_type(MemoryType::DefwData, addr + 1, 2 * count - 1);
}

void Memory::set_defm(int addr, int count) {
    get_opcode(addr) = Opcode(MemoryType::Defm, addr & 0xFFFF, 2 * count);
    auto& op = get_opcode(addr);
    for (int i = 0; i < 1 * count; ++i) {
        op.values.push_back(peek(addr + i));
    }
    set_type(MemoryType::Defm, addr, 1);
    set_type(MemoryType::DefmData, addr + 1, 1 * count - 1);
}

void Memory::set_code(Prog& prog, int addr) {
    while (true) {
        MemoryType type = get_type(addr);
        if (type == MemoryType::Asm) {
            return; // already scanned
        }
        else if (type != MemoryType::Unknown) {
            error("code flow entered a '" + std::string(1, memory_type_char(type)) +
                  "' region at " + int16_to_hex(addr));
            return;
        }

        Opcode opcode = disasm_z80(*this, addr);

        // show progress
        std::cout << opcode.to_string(prog);

        if (opcode.opcode.empty()) {
            error("invalid disassembly at " + int16_to_hex(addr));
            return;
        }

        // set memory type
        prog.mem.set_type(MemoryType::Asm, addr);
        for (int i = 1; i < opcode.size; i++) {
            prog.mem.set_type(MemoryType::AsmData, addr + i);
        }

        // store opcode
        get_opcode(addr) = opcode;
        set_type(MemoryType::Asm, addr);
        for (int p = addr + 1; p < addr + opcode.size; p++) {
            set_type(MemoryType::AsmData, p & 0xffff, 1);
        }

        if (opcode.is_jump) {
            prog.dis_data.get_asm_label(opcode.nn);
            if (get_type(opcode.nn) == MemoryType::Undefined) {
                set_code(prog, opcode.nn);    // recurse for branches
                if (get_error_count() > 0) {
                    return;    // prevent cascade of errors
                }
            }
        }

        addr += opcode.size;
        addr &= 0xFFFF;

        if (opcode.ends_flow) {
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void Comments::clear() {
    lines.clear();
}

void Comments::add(int addr, const std::string& line) {
    lines[addr].push_back(line);
}

std::vector<std::string> Comments::get(int addr) const {
    auto it = lines.find(addr);
    if (it == lines.end()) {
        return std::vector<std::string>();
    }
    else {
        return it->second;
    }
}

//-----------------------------------------------------------------------------

bool Line::empty() const {
    return line_num == -1 && label.empty() && tokens.empty();
}

void Line::clear() {
    type = SourceType::BASIC;
    line_num = -1;
    label.clear();
    tokens.clear();
    start_addr = 0;
    asm_length = 0;
}

//-----------------------------------------------------------------------------

std::string DisData::get_basic_label(int line_num) {
    std::string label;
    if (basic_labels.find(line_num, label)) {
        return label;
    }
    else {
        std::ostringstream oss;
        oss << "LINE_" << std::setw(4) << std::setfill('0') << line_num;
        label = oss.str();
        basic_labels.add(label, line_num);
        return label;
    }
}

std::string DisData::get_asm_label(int addr) {
    std::string label;
    if (asm_labels.find(addr, label)) {
        return label;
    }
    else {
        std::ostringstream oss;
        oss << "LBL_" << std::uppercase << std::setfill('0') << std::hex << std::setw(
                4) << addr;
        label = oss.str();
        asm_labels.add(label, addr);
        return label;
    }
}

//-----------------------------------------------------------------------------
// 10-digit precision to match 32-bit mantissa
std::string double_to_string(double value) {
    std::ostringstream oss;
    if (floor(value) == value) {
        oss << value;
    }
    else {
        oss << std::setprecision(10) << value;
    }
    return oss.str();
}

std::string fixed_width_string(const std::string& text, size_t width) {
    if (text.length() < width) {
        return text + std::string(width - text.length(), ' ');
    }
    else {
        return text.substr(0, width);
    }
}
