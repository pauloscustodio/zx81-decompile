//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "disasm.h"
#include "encode.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include "utils.h"
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string Opcode::decode_opcode() {
    // get opcode
    string instr, args;
    auto p = opcode.find(' ');
    if (p == string::npos) {
        instr = opcode;
        args = "";
    }
    else {
        instr = opcode.substr(0, p);
        args = opcode.substr(p + 1);
    }

    // get arguments
    if (args.find("+DIS") != string::npos) {
        string s;
        if (dis > 0) {
            s = string("+") + std::to_string(dis);
        }
        else if (dis < 0) {
            s = std::to_string(dis);
        }
        else {
            s = "";
        }
        args = str_replace_all(args, "+DIS", s);
    }

    if (args.find("NN") != string::npos) {
        string s;
        int value;
        string label;
        if (!refer_to.empty() && g_asm_labels.find(refer_to, value)) {
            int offset = nn - value;
            if (offset < 0) {
                s = refer_to + "-$" + fmt_hex(-offset, 4);
            }
            else if (n > 0) {
                s += refer_to + "+$" + fmt_hex(offset, 4);
            }
            else {
                s = refer_to;
            }
        }
        else {
            s = decode_label(nn);
        }

        args = str_replace_all(args, "NN", s);
    }

    if (args.find("N") != string::npos) {
        string s = string("$") + fmt_hex(n, 2);
        args = str_replace_all(args, "N", s);
    }

    // result
    ostringstream oss;
    oss << setw(8) << left << instr
        << setw(0) << left << args;
    return oss.str();
}

string Opcode::decode_undef() {
    ostringstream oss;
    oss << setw(8) << left << "defb"
        << setw(0) << left << "$" << fmt_hex(peek(addr), 2);
    return oss.str();
}

string Opcode::decode_defb() {
    ostringstream oss;
    oss << setw(8) << left << "defb"
        << setw(0) << left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << "$" << fmt_hex(value, 2);
    }
    return oss.str();
}

string Opcode::decode_defw() {
    ostringstream oss;
    oss << setw(8) << left << "defw"
        << setw(0) << left;
    bool first = true;
    for (auto& value : values) {
        if (!first) {
            oss << ", ";
        }
        first = false;

        oss << decode_label(value);
    }
    return oss.str();
}

string Opcode::decode_defm() {
    ostringstream oss;
    oss << setw(8) << left << "defm"
        << setw(0) << left
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

string Opcode::decode_label(int value) {
    ostringstream oss;
    string label;
    if (g_asm_labels.find(value, label)) {
        oss << label;
    }
    else if (g_asm_labels.find(value - 1, label)) {
        oss << label << "+1";
    }
    else if (g_asm_labels.find(value - 2, label)) {
        oss << label << "+2";
    }
    else {
        oss << "$" << fmt_hex(value, 4);
    }
    return oss.str();
}
