//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include "basic_info.h"
#include "disasm.h"
#include "errors.h"
#include <string>
#include <vector>

struct MemElement {
    enum class SourceType { Basic, Asm };
    enum class ElementType { Defb, Defw, Defm, Code, Comment };
    SourceType source_type = SourceType::Basic;
    ElementType type = ElementType::Defb;
    int line_num = 0;   // for Basic
    int addr = 0;       // for Asm
    int size = 1;
    std::string label;
    std::string comment;
    std::string header;
};

class CtlFile {
public:
    explicit CtlFile() = default;
    bool load_file(const std::string& filename);
    const std::vector<MemElement>& get_elements() const;
    void apply_elements(BasicInfo* basic_info, DisasmCode* disasm);

private:
    std::vector<MemElement> elements;

    bool parse_ctl_line(const std::string& line, const SourceLoc& loc);
};
