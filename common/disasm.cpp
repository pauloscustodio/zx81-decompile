//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disasm.h"
#include "errors.h"
#include "release_assert.h"
#include "utils.h"
#include "zx81chars.h"
#include "zx81consts.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

//-----------------------------------------------------------------------------
// labels
//-----------------------------------------------------------------------------

void Labels::add(const std::string& name, int value) {
    auto it = by_name.find(name);
    if (it != by_name.end()) {
        if (it->second == value) {
            return;     // nothing to do
        }
        error("label redefinition: " + name + " = " +
              fmt_hex(value, 4) + " != " + fmt_hex(it->second, 4));
    }

    by_name[name] = value;
    by_value[value] = name;		// overwrite if tow labels with same value
}

int Labels::get(const std::string& name) const {
    int value = 0;
    if (find(name, value)) {
        return value;
    }

    error("label undefined: " + name);
    return 0;
}

bool Labels::find(const std::string& name, int& value) const {
    auto it = by_name.find(name);
    if (it == by_name.end()) {
        return false;
    }

    value = it->second;
    return true;
}

bool Labels::find(int value, std::string& name) const {
    auto it = by_value.find(value);
    if (it == by_value.end()) {
        return false;
    }

    name = it->second;
    return true;
}

void Labels::clear() {
    by_name.clear();
    by_value.clear();
    init();
}

std::string BasicLabels::add_or_get_from_value(int line_num) {
    std::string label;
    if (find(line_num, label)) {
        return label;
    }

    std::ostringstream oss;
    oss << "Line" << std::setw(5) << std::setfill('_') << line_num;
    label = oss.str();
    add(label, line_num);
    return label;
}

AsmLabels::AsmLabels() {
    clear();
}

void AsmLabels::init() {
#define X(name, value)		add(#name, value);
#include "zx81consts.def"
#undef X
}

std::string AsmLabels::add_or_get_from_value(int addr) {
    std::string label;
    if (find(addr, label)) {
        return label;
    }

    label = fmt_hex(addr, 4, "L");
    add(label, addr);
    return label;
}

//-----------------------------------------------------------------------------
// opcode
//-----------------------------------------------------------------------------

std::string Opcode::to_string(const AsmLabels& labels) const {
    std::ostringstream oss;

    switch (type) {
    case MemType::Asm:
        oss << decode_opcode(labels);
        break;
    case MemType::Unknown:
    case MemType::Byte:
        oss << decode_defb();
        break;
    case MemType::Word:
        oss << decode_defw(labels);
        break;
    case MemType::Text:
        oss << decode_defm();
        break;
    case MemType::Undefined:
    case MemType::Sysvars:
    case MemType::Dfile:
    case MemType::Vars:
    case MemType::Basic:
        error("invalid address for opcode: " + fmt_hex(addr, 4));
        break;
    default:
        release_assert(0);
    }

    return oss.str();
}

std::string Opcode::decode_opcode(const AsmLabels& labels) const {
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
            s = std::string("+") + std::to_string(dis);
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
        int value = 0;
        std::string label;
        if (!refer_to.empty() && labels.find(refer_to, value)) {
            int offset = nn - value;
            if (offset < 0) {
                s = refer_to + std::to_string(offset);  // has a minus sign
            }
            else if (n > 0) {
                s += refer_to + "+" + std::to_string(offset);
            }
            else {
                s = refer_to;
            }
        }
        else {
            s = decode_label(labels, nn);
        }

        args = str_replace_all(args, "NN", s);
    }

    if (args.find("N") != std::string::npos) {
        std::string s = fmt_hex(n, 2);
        args = str_replace_all(args, "N", s);
    }

    // result
    std::ostringstream oss;
    if (args.empty()) {
        oss << instr;
    }
    else {
        oss << std::setw(OpcodeWidth) << std::left << instr
            << std::setw(0) << std::left << args;
    }
    return oss.str();
}

std::string Opcode::decode_defb() const {
    std::ostringstream oss;
    oss << std::setw(OpcodeWidth) << std::left << "defb"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << fmt_hex(value, 2);
    }
    return oss.str();
}

std::string Opcode::decode_defw(const AsmLabels& labels) const {
    std::ostringstream oss;
    oss << std::setw(OpcodeWidth) << std::left << "defw"
        << std::setw(0) << std::left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << decode_label(labels, value);
    }
    return oss.str();
}

std::string Opcode::decode_defm() const {
    std::ostringstream oss;
    oss << std::setw(OpcodeWidth) << std::left << "defm"
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

std::string Opcode::decode_label(const AsmLabels& labels, int value) const {
    std::ostringstream oss;
    std::string label;
    if (labels.find(value, label)) {
        oss << label;
    }
    else if (labels.find(value - 1, label)) {
        oss << label << "+1";
    }
    else if (labels.find(value - 2, label)) {
        oss << label << "+2";
    }
    else {
        oss << fmt_hex(value, 4);
    }
    return oss.str();
}

//-----------------------------------------------------------------------------
// disassembler
//-----------------------------------------------------------------------------

std::string Disasm::dd1(int n, int x) {
    static const char* n_lut[3][4] = {
        { "bc", "de", "hl", "sp" },
        { "bc", "de", "ix", "sp" },
        { "bc", "de", "iy", "sp" }
    };
    n &= 0x03;
    return n_lut[x][n];
}

std::string Disasm::dd2(int n, int x) {
    static const char* n_lut[3][4] = {
        { "bc", "de", "hl", "af" },
        { "bc", "de", "ix", "af" },
        { "bc", "de", "iy", "af" }
    };
    n &= 0x03;
    return n_lut[x][n];
}

std::string Disasm::r1(int n, int x) {
    static const char* n_lut[3][8] = {
        { "b", "c", "d", "e", "h", "l", "(hl)", "a" },
        { "b", "c", "d", "e", "ixh", "ixl", "(ix+DIS)", "a" },
        { "b", "c", "d", "e", "iyh", "iyl", "(iy+DIS)", "a" }
    };
    n &= 0x07;
    return n_lut[x][n];
}

std::string Disasm::x1(int x) {
    static const char* x_lut[] = {
        "hl", "ix", "iy"
    };
    return x_lut[x];
}

std::string Disasm::flags1(int n) {
    static const char* n_lut[] = {
        "nz", "z", "nc", "c", "po", "pe", "p", "m"
    };
    n &= 0x07;
    return n_lut[n];
}

std::string Disasm::flags2(int n) {
    n &= 0x03;
    return flags1(n);
}

std::string Disasm::alu1(int n) {
    static const char* n_lut[] = {
        "add a, ", "adc a, ", "sub ", "sbc a, ", "and ", "xor ", "or ", "cp "
    };
    n &= 0x07;
    return n_lut[n];
}

std::string Disasm::rot1(int n) {
    static const char* n_lut[] = {
        "rlc ", "rrc ", "rl ", "rr ", "sla ", "sra ", "sll ", "srl "
    };
    n &= 0x07;
    return n_lut[n];
}

std::string Disasm::bit1(int n) {
    static const char* n_lut[] = {
        "", "bit ", "res ", "set "
    };
    n &= 0x03;
    return n_lut[n];
}

void Disasm::collect_nn(Opcode& opcode) {
    opcode.nn = mem->dpeek(addr);
    addr += 2;
}

void Disasm::collect_n(Opcode& opcode) {
    opcode.n = mem->peek(addr++);
}

void Disasm::collect_dis(Opcode& opcode) {
    opcode.dis = mem->speek(addr++);
}

void Disasm::collect_jr(Opcode& opcode) {
    opcode.n = mem->speek(addr++);
    opcode.nn = addr + opcode.n;
}

bool Disasm::disasm(int addr_, Opcode& opcode) {
    addr = addr_;
    opcode = Opcode(MemType::Asm, addr, 0);

    int x = 0;		// hl, ix, iy
    bool done = false;
    while (!done) {
        done = true;
        int b = mem->peek(addr++);
        switch (b) {
        case 0x00:
            opcode.opcode = "nop";
            break;
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
            opcode.opcode = "ld " + dd1(b >> 4, x) + ", NN";
            collect_nn(opcode);
            break;
        case 0x02:
        case 0x12:
            opcode.opcode = "ld (" + dd1(b >> 4, 0) + "), a";
            break;
        case 0x03:
        case 0x13:
        case 0x23:
        case 0x33:
            opcode.opcode = "inc " + dd1(b >> 4, x);
            break;
        case 0x04:
        case 0x0c:
        case 0x14:
        case 0x1c:
        case 0x24:
        case 0x2c:
        case 0x34:
        case 0x3c:
            if (b == 0x34 && x != 0) {
                collect_dis(opcode);
            }
            opcode.opcode = "inc " + r1(b >> 3, x);
            break;
        case 0x05:
        case 0x0d:
        case 0x15:
        case 0x1d:
        case 0x25:
        case 0x2d:
        case 0x35:
        case 0x3d:
            if (b == 0x35 && x != 0) {
                collect_dis(opcode);
            }
            opcode.opcode = "dec " + r1(b >> 3, x);
            break;
        case 0x06:
        case 0x0e:
        case 0x16:
        case 0x1e:
        case 0x26:
        case 0x2e:
        case 0x36:
        case 0x3e:
            if (b == 0x36 && x != 0) {
                collect_dis(opcode);
            }
            opcode.opcode = "ld " + r1(b >> 3, x) + ", N";
            collect_n(opcode);
            break;
        case 0x07:
            opcode.opcode = "rlca";
            break;
        case 0x08:
            opcode.opcode = "ex af, af'";
            break;
        case 0x09:
        case 0x19:
        case 0x29:
        case 0x39:
            opcode.opcode = "add " + x1(x) + ", " + dd1(b >> 4, x);
            break;
        case 0x0a:
        case 0x1a:
            opcode.opcode = "ld a, (" + dd1(b >> 4, 0) + ")";
            break;
        case 0x0b:
        case 0x1b:
        case 0x2b:
        case 0x3b:
            opcode.opcode = "dec " + dd1(b >> 4, x);
            break;
        case 0x0f:
            opcode.opcode = "rrca";
            break;
        case 0x10:
            opcode.opcode = "djnz NN";
            collect_jr(opcode);
            opcode.is_jump = true;
            break;
        case 0x17:
            opcode.opcode = "rla";
            break;
        case 0x18:
            opcode.opcode = "jr NN";
            collect_jr(opcode);
            opcode.is_jump = true;
            opcode.ends_flow = true;
            break;
        case 0x1f:
            opcode.opcode = "rra";
            break;
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            opcode.opcode = "jr " + flags2(b >> 3) + ", NN";
            collect_jr(opcode);
            opcode.is_jump = true;
            break;
        case 0x22:
            opcode.opcode = "ld (NN), " + x1(x);
            collect_nn(opcode);
            break;
        case 0x27:
            opcode.opcode = "daa";
            break;
        case 0x2a:
            opcode.opcode = "ld " + x1(x) + ", (NN)";
            collect_nn(opcode);
            break;
        case 0x2f:
            opcode.opcode = "cpl";
            break;
        case 0x32:
            opcode.opcode = "ld (NN), a";
            collect_nn(opcode);
            break;
        case 0x37:
            opcode.opcode = "scf";
            break;
        case 0x3a:
            opcode.opcode = "ld a, (NN)";
            collect_nn(opcode);
            break;
        case 0x3f:
            opcode.opcode = "ccf";
            break;
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f: {
            int src = b & 0x07;
            int dst = (b >> 3) & 0x07;
            if (x != 0) {
                if (src == 6) {
                    collect_dis(opcode);
                    opcode.opcode = "ld " + r1(dst, 0) + ", " + r1(src, x);
                }
                else if (dst == 6) {
                    collect_dis(opcode);
                    opcode.opcode = "ld " + r1(dst, x) + ", " + r1(src, 0);
                }
                else {
                    opcode.opcode = "ld " + r1(dst, x) + ", " + r1(src, x);
                }
            }
            else {
                opcode.opcode = "ld " + r1(dst, x) + ", " + r1(src, x);
            }
            break;
        }
        case 0x76:
            opcode.opcode = "halt";
            break;
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
            if (x != 0)
                if ((b & 0x07) == 0x06) {
                    collect_dis(opcode);
                }
            opcode.opcode = alu1(b >> 3) + r1(b, x);
            break;
        case 0xc0:
        case 0xc8:
        case 0xd0:
        case 0xd8:
        case 0xe0:
        case 0xe8:
        case 0xf0:
        case 0xf8:
            opcode.opcode = "ret " + flags1(b >> 3);
            break;
        case 0xc1:
        case 0xd1:
        case 0xe1:
        case 0xf1:
            opcode.opcode = "pop " + dd2(b >> 4, x);
            break;
        case 0xc2:
        case 0xca:
        case 0xd2:
        case 0xda:
        case 0xe2:
        case 0xea:
        case 0xf2:
        case 0xfa:
            opcode.opcode = "jp " + flags1(b >> 3) + ", NN";
            collect_nn(opcode);
            opcode.is_jump = true;
            break;
        case 0xc3:
            opcode.opcode = "jp NN";
            collect_nn(opcode);
            opcode.is_jump = true;
            opcode.ends_flow = true;
            break;
        case 0xc4:
        case 0xcc:
        case 0xd4:
        case 0xdc:
        case 0xe4:
        case 0xec:
        case 0xf4:
        case 0xfc:
            opcode.opcode = "call " + flags1(b >> 3) + ", NN";
            collect_nn(opcode);
            opcode.is_jump = true;
            break;
        case 0xc5:
        case 0xd5:
        case 0xe5:
        case 0xf5:
            opcode.opcode = "push " + dd2(b >> 4, x);
            break;
        case 0xc6:
        case 0xce:
        case 0xd6:
        case 0xde:
        case 0xe6:
        case 0xee:
        case 0xf6:
        case 0xfe:
            opcode.opcode = alu1(b >> 3) + "N";
            collect_n(opcode);
            break;
        case 0xc7:
        case 0xcf:
        case 0xd7:
        case 0xdf:
        case 0xe7:
        case 0xef:
        case 0xf7:
        case 0xff:
            opcode.opcode = "rst " + fmt_hex(b & 0x38, 2);
            break;
        case 0xc9:
            opcode.opcode = "ret";
            opcode.ends_flow = true;
            break;
        case 0xcb:
            if (x != 0) {
                collect_dis(opcode);
            }
            b = mem->peek(addr++);
            if (b <= 0x3f) {
                opcode.opcode = rot1(b >> 3) + r1(b, x);
            }
            else {
                opcode.opcode = bit1(b >> 6) + std::to_string((b >> 3) & 0x07) + ", " +
                                r1(b, x);
            }
            break;
        case 0xcd:
            opcode.opcode = "call NN";
            collect_nn(opcode);
            opcode.is_jump = true;
            break;
        case 0xd3:
            opcode.opcode = "out (N), a";
            collect_n(opcode);
            break;
        case 0xd9:
            opcode.opcode = "exx";
            break;
        case 0xdb:
            opcode.opcode = "in a, (N)";
            collect_n(opcode);
            break;
        case 0xdd:
            x = 1;
            done = false;
            break;
        case 0xe3:
            opcode.opcode = "ex (sp), " + x1(x);
            break;
        case 0xe9:
            opcode.opcode = "jp (" + x1(x) + ")";
            opcode.ends_flow = true;
            break;
        case 0xeb:
            opcode.opcode = "ex de, " + x1(x);
            break;
        case 0xed:
            b = mem->peek(addr++);
            switch (b) {
            case 0x40:
            case 0x48:
            case 0x50:
            case 0x58:
            case 0x60:
            case 0x68:
            case 0x78:
                opcode.opcode = "in " + r1(b >> 3, 0) + ", (c)";
                break;
            case 0x41:
            case 0x49:
            case 0x51:
            case 0x59:
            case 0x61:
            case 0x69:
            case 0x79:
                opcode.opcode = "out (c), " + r1(b >> 3, 0);
                break;
            case 0x42:
            case 0x52:
            case 0x62:
            case 0x72:
                opcode.opcode = "sbc hl, " + dd1(b >> 4, 0);
                break;
            case 0x43:
            case 0x53:
            case 0x63:
            case 0x73:
                opcode.opcode = "ld (NN), " + dd1(b >> 4, 0);
                collect_nn(opcode);
                break;
            case 0x44:
                opcode.opcode = "neg";
                break;
            case 0x45:
                opcode.opcode = "retn";
                opcode.ends_flow = true;
                break;
            case 0x46:
                opcode.opcode = "im 0";
                break;
            case 0x47:
                opcode.opcode = "ld i, a";
                break;
            case 0x4a:
            case 0x5a:
            case 0x6a:
            case 0x7a:
                opcode.opcode = "adc hl, " + dd1(b >> 4, 0);
                break;
            case 0x4b:
            case 0x5b:
            case 0x6b:
            case 0x7b:
                opcode.opcode = "ld " + dd1(b >> 4, 0) + ", (NN)";
                collect_nn(opcode);
                break;
            case 0x4d:
                opcode.opcode = "reti";
                opcode.ends_flow = true;
                break;
            case 0x4f:
                opcode.opcode = "ld r, a";
                break;
            case 0x56:
                opcode.opcode = "im 1";
                break;
            case 0x57:
                opcode.opcode = "ld a, i";
                break;
            case 0x5e:
                opcode.opcode = "im 2";
                break;
            case 0x5f:
                opcode.opcode = "ld a, r";
                break;
            case 0x67:
                opcode.opcode = "rrd";
                break;
            case 0x6f:
                opcode.opcode = "rld";
                break;
            case 0xa0:
                opcode.opcode = "ldi";
                break;
            case 0xa1:
                opcode.opcode = "cpi";
                break;
            case 0xa2:
                opcode.opcode = "ini";
                break;
            case 0xa3:
                opcode.opcode = "outi";
                break;
            case 0xa8:
                opcode.opcode = "ldd";
                break;
            case 0xa9:
                opcode.opcode = "cpd";
                break;
            case 0xaa:
                opcode.opcode = "ind";
                break;
            case 0xab:
                opcode.opcode = "outd";
                break;
            case 0xb0:
                opcode.opcode = "ldir";
                break;
            case 0xb1:
                opcode.opcode = "cpir";
                break;
            case 0xb2:
                opcode.opcode = "inir";
                break;
            case 0xb3:
                opcode.opcode = "otir";
                break;
            case 0xb8:
                opcode.opcode = "lddr";
                break;
            case 0xb9:
                opcode.opcode = "cpdr";
                break;
            case 0xba:
                opcode.opcode = "indr";
                break;
            case 0xbb:
                opcode.opcode = "otdr";
                break;
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0c:
            case 0x0d:
            case 0x0e:
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f:
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3a:
            case 0x3b:
            case 0x3c:
            case 0x3d:
            case 0x3e:
            case 0x3f:
            case 0x4c:
            case 0x4e:
            case 0x54:
            case 0x55:
            case 0x5c:
            case 0x5d:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x70:
            case 0x71:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x7c:
            case 0x7d:
            case 0x7e:
            case 0x7f:
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8a:
            case 0x8b:
            case 0x8c:
            case 0x8d:
            case 0x8e:
            case 0x8f:
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
            case 0x98:
            case 0x99:
            case 0x9a:
            case 0x9b:
            case 0x9c:
            case 0x9d:
            case 0x9e:
            case 0x9f:
            case 0xa4:
            case 0xa5:
            case 0xa6:
            case 0xa7:
            case 0xac:
            case 0xad:
            case 0xae:
            case 0xaf:
            case 0xb4:
            case 0xb5:
            case 0xb6:
            case 0xb7:
            case 0xbc:
            case 0xbd:
            case 0xbe:
            case 0xbf:
            case 0xc0:
            case 0xc1:
            case 0xc2:
            case 0xc3:
            case 0xc4:
            case 0xc5:
            case 0xc6:
            case 0xc7:
            case 0xc8:
            case 0xc9:
            case 0xca:
            case 0xcb:
            case 0xcc:
            case 0xcd:
            case 0xce:
            case 0xcf:
            case 0xd0:
            case 0xd1:
            case 0xd2:
            case 0xd3:
            case 0xd4:
            case 0xd5:
            case 0xd6:
            case 0xd7:
            case 0xd8:
            case 0xd9:
            case 0xda:
            case 0xdb:
            case 0xdc:
            case 0xdd:
            case 0xde:
            case 0xdf:
            case 0xe0:
            case 0xe1:
            case 0xe2:
            case 0xe3:
            case 0xe4:
            case 0xe5:
            case 0xe6:
            case 0xe7:
            case 0xe8:
            case 0xe9:
            case 0xea:
            case 0xeb:
            case 0xec:
            case 0xed:
            case 0xee:
            case 0xef:
            case 0xf0:
            case 0xf1:
            case 0xf2:
            case 0xf3:
            case 0xf4:
            case 0xf5:
            case 0xf6:
            case 0xf7:
            case 0xf8:
            case 0xf9:
            case 0xfa:
            case 0xfb:
            case 0xfc:
            case 0xfd:
            case 0xfe:
            case 0xff:
                return false; // invalid opcode
            default:
                release_assert(0);
            }
            break;
        case 0xf3:
            opcode.opcode = "di";
            break;
        case 0xf9:
            opcode.opcode = "ld sp, " + x1(x);
            break;
        case 0xfb:
            opcode.opcode = "ei";
            break;
        case 0xfd:
            x = 2;
            done = false;
            break;
        default:
            release_assert(0);
        }
    }

    opcode.size = addr - opcode.addr;
    return true;
}

//-----------------------------------------------------------------------------
// image being disassembled
//-----------------------------------------------------------------------------

BasicLabels& DisasmCode::get_basic_labels() {
    return basic_labels;
}

const BasicLabels& DisasmCode::get_basic_labels() const {
    return basic_labels;
}

AsmLabels& DisasmCode::get_asm_labels() {
    return asm_labels;
}

const AsmLabels& DisasmCode::get_asm_labels() const {
    return asm_labels;
}

MemType DisasmCode::get_mem_type(int addr) const {
    return mem_map[addr & 0xffff];
}

void DisasmCode::set_mem_type(int addr, MemType type, int count) {
    for (int i = 0; i < count; ++i) {
        if (mem_map[(addr + i) & 0xffff] != MemType::Unknown &&
                mem_map[(addr + i) & 0xffff] != type) {
            error("memory overlap at address " + fmt_hex(addr + i, 4));
        }
        mem_map[(addr + i) & 0xffff] = type;
    }
}

void DisasmCode::set_mem_type_no_overlap_check(int addr, MemType type,
        int count) {
    for (int i = 0; i < count; ++i) {
        mem_map[(addr + i) & 0xffff] = type;
    }
}

void DisasmCode::set_code(int addr) {
    bool first = true;
    bool done = false;

    while (true) {
        addr &= 0xffff;

        // check for overlap with existing code or data
        switch (get_mem_type(addr)) {
        case MemType::Undefined:
            return; // jump outside of program, e.g. to ROM
        case MemType::Sysvars:
        case MemType::Dfile:
        case MemType::Vars:
        case MemType::Basic:
        case MemType::Byte:
        case MemType::Word:
        case MemType::Text:
            error("jump into data at address " + fmt_hex(addr, 4));
            return;
        case MemType::Unknown:
            break; // continue
        case MemType::Asm:
            done = true; // already analysed
            break;
        default:
            release_assert(0);
        }

        // define label
        if (first) {
            asm_labels.add_or_get_from_value(addr);
            first = false;
        }

        // exit if already analysed, but only after defining label
        if (done) {
            break;
        }

        // disassemble instruction
        auto opcode = std::make_unique<Opcode>(MemType::Asm, addr, 0);
        if (!disasm.disasm(addr, *opcode)) {
            error("invalid opcode at address " + fmt_hex(addr, 4));
            break;
        }

        // register instruction
        opcodes[addr] = std::move(opcode);
        auto opcode_ptr = opcodes[addr].get();

        // mark memory as code
        for (int i = 0; i < opcode_ptr->size; ++i) {
            set_mem_type(addr, MemType::Asm);
            addr = (addr + 1) & 0xffff;
        }

        // recurse if jump instruction
        if (opcode_ptr->is_jump) {
            set_code(opcode_ptr->nn);
        }

        // exit if instruction ends flow of control
        if (opcode_ptr->ends_flow) {
            break;
        }
    }
}

void DisasmCode::set_byte(int addr, int count) {
    addr &= 0xffff;

    // check for overlap with existing code or data
    std::vector<int> values;
    for (int i = 0; i < count; ++i) {
        int addr1 = (addr + i) & 0xffff;
        values.push_back(mem->peek(addr1));

        switch (get_mem_type(addr1)) {
        case MemType::Undefined:
        case MemType::Sysvars:
        case MemType::Dfile:
        case MemType::Vars:
        case MemType::Basic:
        case MemType::Asm:
        case MemType::Word:
        case MemType::Text:
            error("memory overlap at address " + fmt_hex(addr1, 4));
            return;
        case MemType::Byte:
        case MemType::Unknown:
            break; // continue
        default:
            release_assert(0);
        }
    }

    // mark memory as data
    for (int i = 0; i < count; ++i) {
        set_mem_type((addr + i) & 0xffff, MemType::Byte);
    }

    // create opcode for data
    auto opcode = std::make_unique<Opcode>(MemType::Byte, addr, count);
    opcode->values = values;
    opcodes[addr] = std::move(opcode);
}

void DisasmCode::set_word(int addr, int count) {
    addr &= 0xffff;

    // check for overlap with existing code or data
    std::vector<int> values;
    for (int i = 0; i < 2 * count; i += 2) {
        int addr1 = (addr + i) & 0xffff;
        values.push_back(mem->dpeek(addr1));

        switch (get_mem_type(addr1)) {
        case MemType::Undefined:
        case MemType::Sysvars:
        case MemType::Dfile:
        case MemType::Vars:
        case MemType::Basic:
        case MemType::Asm:
        case MemType::Byte:
        case MemType::Text:
            error("memory overlap at address " + fmt_hex(addr1, 4));
            return;
        case MemType::Word:
        case MemType::Unknown:
            break; // continue
        default:
            release_assert(0);
        }
    }

    // mark memory as data
    for (int i = 0; i < 2 * count; ++i) {
        set_mem_type((addr + i) & 0xffff, MemType::Word);
    }

    // create opcode for data
    auto opcode = std::make_unique<Opcode>(MemType::Word, addr, 2 * count);
    opcode->values = values;
    opcodes[addr] = std::move(opcode);
}

void DisasmCode::set_text(int addr, int count) {
    addr &= 0xffff;

    // check for overlap with existing code or data
    std::string str;
    for (int i = 0; i < count; i++) {
        int addr1 = (addr + i) & 0xffff;
        str += decode_zx81(mem->peek(addr1));

        switch (get_mem_type(addr1)) {
        case MemType::Undefined:
        case MemType::Sysvars:
        case MemType::Dfile:
        case MemType::Vars:
        case MemType::Basic:
        case MemType::Asm:
        case MemType::Byte:
        case MemType::Word:
            error("memory overlap at address " + fmt_hex(addr1, 4));
            return;
        case MemType::Text:
        case MemType::Unknown:
            break; // continue
        default:
            release_assert(0);
        }
    }

    // mark memory as data
    for (int i = 0; i < count; i++) {
        set_mem_type((addr + i) & 0xffff, MemType::Text);
    }

    // create opcode for data
    auto opcode = std::make_unique<Opcode>(MemType::Text, addr, count);
    opcode->str = str;
    opcodes[addr] = std::move(opcode);
}

const Opcode* DisasmCode::get_opcode(int addr) const {
    Opcode* opcode = opcodes[addr & 0xffff].get();
    if (opcode) {
        return opcode;
    }
    return nullptr;
}

void DisasmCode::add_header(int addr, const std::string& header) {
    addr &= 0xffff;
    if (!headers[addr]) {
        headers[addr] = std::make_unique<std::vector<std::string>>();
    }
    headers[addr]->push_back(header);
}

void DisasmCode::add_comment(int addr, const std::string& comment) {
    addr &= 0xffff;
    if (!comments[addr]) {
        comments[addr] = std::make_unique<std::vector<std::string>>();
    }
    comments[addr]->push_back(comment);
}

const std::vector<std::string>& DisasmCode::get_header(int addr) const {
    addr &= 0xffff;
    static const std::vector<std::string> empty;

    if (!headers[addr]) {
        return empty;
    }

    return *headers[addr];
}

const std::vector<std::string>& DisasmCode::get_comment(int addr) const {
    addr &= 0xffff;
    static const std::vector<std::string> empty;

    if (!comments[addr]) {
        return empty;
    }

    return *comments[addr];
}

