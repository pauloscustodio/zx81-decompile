//-----------------------------------------------------------------------------
// zx81dis
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disz80.h"
#include "model.h"
#include "utils.h"
#include <cassert>

static std::string dd1(int n, int x) {
    const char* n_lut[] = { "bc", "de", "hl", "sp" };
    const char* x_lut[] = { "hl", "ix", "iy" };
    n &= 0x03;
    if (n == 2) {
        return x_lut[x];
    }
    else {
        return n_lut[n];
    }
}

static std::string dd2(int n, int x) {
    const char* n_lut[] = { "bc", "de", "hl", "af" };
    const char* x_lut[] = { "hl", "ix", "iy" };
    n &= 0x03;
    if (n == 2) {
        return x_lut[x];
    }
    else {
        return n_lut[n];
    }
}

static std::string r1(int n, int x) {
    const char* n_lut[] = { "b", "c", "d", "e", "h", "l", "(hl)", "a" };
    const char* x_lut[] = { "(hl)", "(ix+DIS)", "(iy+DIS)" };
    n &= 0x07;
    if (n == 6) {
        return x_lut[x];
    }
    else {
        return n_lut[n];
    }
}

static std::string x1(int x) {
    const char* x_lut[] = { "hl", "ix", "iy" };
    return x_lut[x];
}

static std::string flags1(int n) {
    const char* n_lut[] = { "nz", "z", "nc", "c", "po", "pe", "p", "m" };
    n &= 0x07;
    return n_lut[n];
}

static std::string flags2(int n) {
    n &= 0x03;
    return flags1(n);
}

static std::string alu1(int n) {
    const char* n_lut[] = { "add a, ", "adc a, ", "sub ", "sbc a, ", "and ", "xor ", "or ", "cp " };
    n &= 0x07;
    return n_lut[n];
}

static std::string rot1(int n) {
    const char* n_lut[] = { "rlc ", "rrc ", "rl ", "rr ", "sla ", "sra ", "sll ", "srl " };
    n &= 0x07;
    return n_lut[n];
}

static std::string bit1(int n) {
    const char* n_lut[] = { "", "bit ", "res ", "set " };
    n &= 0x03;
    return n_lut[n];
}

Opcode disasm_z80(Memory& mem, int start_addr) {
    int addr = start_addr & 0xFFFF;
    Opcode opcode(MemoryType::Asm, addr, 0);

    auto fetch = [&]() -> int {
        int ret = mem.peek(addr++);
        addr &= 0xFFFF;
        return ret;
    };
    auto collect_nn = [&]() {
        opcode.nn = mem.peek_word(addr);
        addr += 2;
        addr &= 0xFFFF;
    };
    auto collect_n = [&]() {
        opcode.dis = mem.peek(addr++);
        addr &= 0xFFFF;
    };
    auto collect_dis = [&]() {
        opcode.dis = mem.peek(addr++);
        addr &= 0xFFFF;
    };
    auto collect_jr = [&]() {
        opcode.n = mem.peek(addr++);
        opcode.nn = addr + opcode.n;
        addr &= 0xFFFF;
    };

    int x = 0;		// hl, ix, iy
    bool done = false;
    while (!done) {
        done = true;
        int b = fetch();
        switch (b) {
        case 0x00:
            opcode.opcode = "nop";
            break;
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
            opcode.opcode = "ld " + dd1(b >> 4, x) + ", NN";
            collect_nn();
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
                collect_dis();
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
                collect_dis();
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
                collect_dis();
            }
            opcode.opcode = "ld " + r1(b >> 3, x) + ", N";
            collect_n();
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
            opcode.opcode = "ld a, (" + dd1(b >> 4, 0) + "";
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
            collect_jr();
            opcode.is_jump = true;
            break;
        case 0x17:
            opcode.opcode = "rla";
            break;
        case 0x18:
            opcode.opcode = "jr NN";
            collect_jr();
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
            collect_jr();
            opcode.is_jump = true;
            break;
        case 0x22:
            opcode.opcode = "ld (NN), " + x1(x);
            collect_nn();
            break;
        case 0x27:
            opcode.opcode = "daa";
            break;
        case 0x2a:
            opcode.opcode = "ld " + x1(x) + ", (NN)";
            collect_nn();
            break;
        case 0x2f:
            opcode.opcode = "cpl";
            break;
        case 0x32:
            opcode.opcode = "ld (NN), a";
            collect_nn();
            break;
        case 0x37:
            opcode.opcode = "scf";
            break;
        case 0x3a:
            opcode.opcode = "ld a, (NN)";
            collect_nn();
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
        case 0x7f:
            if (x != 0)
                if ((b & 0x07) == 0x06 || (b & 0x38) == 0x30) {
                    collect_dis();
                }
            opcode.opcode = "ld " + r1(b >> 3, x) + ", " + r1(b, x);
            break;
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
                    collect_dis();
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
            collect_nn();
            opcode.is_jump = true;
            break;
        case 0xc3:
            opcode.opcode = "jp NN";
            collect_nn();
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
            collect_nn();
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
            collect_n();
            break;
        case 0xc7:
        case 0xcf:
        case 0xd7:
        case 0xdf:
        case 0xe7:
        case 0xef:
        case 0xf7:
        case 0xff:
            opcode.opcode = "rst " + int8_to_hex(b & 0x34) + "h";
            break;
        case 0xc9:
            opcode.opcode = "ret";
            opcode.ends_flow = true;
            break;
        case 0xcb:
            if (x != 0)
                if ((b & 0x07) == 0x06) {
                    collect_dis();
                }
            b = fetch();
            if (b <= 0x3f) {
                opcode.opcode = rot1(b >> 3) + r1(b, 0);
            }
            else {
                opcode.opcode = bit1(b >> 6) + std::to_string((b >> 3) & 0x07) + ", " + r1(b,
                                0);
            }
            break;
        case 0xcd:
            opcode.opcode = "call NN";
            collect_nn();
            opcode.is_jump = true;
            break;
        case 0xd3:
            opcode.opcode = "out (N), a";
            collect_n();
            break;
        case 0xd9:
            opcode.opcode = "exx";
            break;
        case 0xdb:
            opcode.opcode = "in a, (N)";
            collect_n();
            break;
        case 0xdd:
            x = 1;
            done = false;
            break;
        case 0xe3:
            opcode.opcode = "ex (sp), " + x1(x);
            break;
        case 0xe9:
            opcode.opcode = "jp (" + x1(x) + "";
            opcode.ends_flow = true;
            break;
        case 0xeb:
            opcode.opcode = "ex de, " + x1(x);
            break;
        case 0xed:
            b = fetch();
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
                collect_nn();
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
                collect_nn();
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
                break;
            default:
                assert(0);
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
            assert(0);
        }
    }

    opcode.size = (addr - opcode.addr) & 0xFFFF;
    return opcode;
}
