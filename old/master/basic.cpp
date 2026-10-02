int Basic::eval_const_expr(const Expr& rpn) {
    vector<int> stack;
    Symbol* symbol{ nullptr };
    int value = 0;

    for (auto& token : rpn) {
        switch (token.code) {
        case T_integer:
            stack.push_back(token.ivalue);
            break;
        case T_line_num_ref:
            symbol = symtab.get(token.ident);
            if (symbol && symbol->type == Symbol::Type::Const) {
                stack.push_back(dpeek_be(symbol->value));
            }
            else {
                error("const expression refers to non-const symbol", token.ident);
                return 0;
            }
            break;
        case T_line_addr_ref:
            symbol = symtab.get(token.ident);
            if (symbol && symbol->type == Symbol::Type::Const) {
                stack.push_back(symbol->value);
            }
            else {
                error("const expression refers to non-const symbol", token.ident);
                return 0;
            }
            break;
        case T_unary_minus:
            stack.back() = -stack.back();
            break;
        case C_plus:
            value = stack.back();
            stack.pop_back();
            stack.back() += value;
            break;
        case C_minus:
            value = stack.back();
            stack.pop_back();
            stack.back() -= value;
            break;
        case C_mult:
            value = stack.back();
            stack.pop_back();
            stack.back() *= value;
            break;
        case C_div:
            value = stack.back();
            stack.pop_back();
            stack.back() /= value;
            break;
        default:
            assert(0);
        }
    }

    assert(stack.size() == 1);
    return stack.back();
}

bool Basic::eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error) {
    vector<int> stack;
    Symbol* symbol{ nullptr };
    bool result = true;

    for (auto& token : rpn) {
        switch (token.code) {
        case T_integer:
            stack.push_back(token.ivalue);
            break;
        case T_line_num_ref:
            symbol = symtab.get(token.ident);
            if (symbol) {
                stack.push_back(dpeek_be(symbol->value));
            }
            else {
                result = false;
                if (do_error) {
                    error("undefined symbol", token.ident);
                }
                stack.push_back(0);
            }
            break;
        case T_line_addr_ref:
            symbol = symtab.get(token.ident);
            if (symbol) {
                stack.push_back(symbol->value);
            }
            else {
                result = false;
                if (do_error) {
                    error("undefined symbol", token.ident);
                }
                stack.push_back(0);
            }
            break;
        case T_ASMPC:
            stack.push_back(asmpc);
            break;
        case T_unary_minus:
            stack.back() = -stack.back();
            break;
        case C_plus:
            value = stack.back();
            stack.pop_back();
            stack.back() += value;
            break;
        case C_minus:
            value = stack.back();
            stack.pop_back();
            stack.back() -= value;
            break;
        case C_mult:
            value = stack.back();
            stack.pop_back();
            stack.back() *= value;
            break;
        case C_div:
            value = stack.back();
            stack.pop_back();
            stack.back() /= value;
            break;
        default:
            assert(0);
        }
    }

    assert(stack.size() == 1);
    value = stack.back();
    return result;
}

void Basic::parse_b81_file(const string& filename) {
    ::parse_b81_file(*this, filename);
}

void Basic::write_basic_vars(ofstream& ofs) const {
    int num_elements = 0;
    int last_dimension = 0;

    int addr = 0;
    for (auto& var : basic_vars) {
        addr = var.addr;

        if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
            ofs << "# [$" << fmt_hex(addr, 4) << "]" << endl;
        }

        switch (var.type) {
        case BasicVar::Type::Number:
            ofs << "#VARS " << var.name << "=" << var.value << endl;
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

            ofs << ")=";

            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << var.values[i];
            }

            ofs << endl;
            break;
        case BasicVar::Type::ForNextLoop:
            ofs << "#VARS " << var.name << "=" << var.value
                << "," << var.limit << "," << var.step << "," << var.line_num << endl;
            break;
        case BasicVar::Type::String:
            ofs << "#VARS " << var.name << "$=\"" << var.str << "\"" << endl;
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

            ofs << ")=";

            last_dimension = var.dimensions.back();
            num_elements /= last_dimension;
            for (int i = 0; i < num_elements; i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << "\"" << var.strs[i] << "\"";
            }

            ofs << endl;
            break;
        default:
            assert(0);
        }
        addr += var.size;
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG) {
        ofs << "# [$" << fmt_hex(addr, 4) << "] = $" << fmt_hex(peek(addr), 2) << endl;
    }

    if ((optflags & FLAG_DEBUG) == FLAG_DEBUG || !basic_vars.empty()) {
        ofs << endl;
    }
}

void Basic::write_basic_memory_map(ofstream& ofs) const {
    ofs << endl;
    int d_file = dpeek(D_FILE);
    int vars = dpeek(VARS);
    int e_line = dpeek(E_LINE);
    for (int addr = RAM_ADDR; addr < e_line; addr += 64) {
        ofs << "# [$" << fmt_hex(addr, 4) << "] ";
        for (int p = addr; p < addr + 64; p++) {
            if (p < PROG) {
                ofs << "!";
            }
            else if (p < d_file) {
                switch (g_disasm_code.get_type(p)) {
                case Opcode::Type::Undef:
                    ofs << "#";
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
                    assert(0);
                }
            }
            else if (p < vars) {
                ofs << "*";
            }
            else {
                ofs << "$";
            }
        }
        ofs << endl;
    }
}
