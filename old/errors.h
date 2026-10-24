//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <string>

class Errors {
public:
    void clear();
    void set_filename(const std::string& filename);
    void set_line_num(int line_num);

    const std::string& get_filename() const {
        return m_filename;
    }
    int get_line_num() const {
        return m_line_num;
    }
    int get_count() const {
        return m_count;
    }

    void error(const std::string& message, const std::string& arg = "");
    void fatal_error(const std::string& message, const std::string& arg = "");
    void exit_status() const;

private:
    int m_count;
    int m_line_num;
    std::string m_filename;

};

void err_clear();
void err_set_filename(const std::string& filename);
void err_set_line_num(int line_num);
std::string err_get_filename();
int err_get_line_num();
int err_get_count();
void error(const std::string& message, const std::string& arg = "");
void fatal_error(const std::string& message, const std::string& arg = "");
void exit_status();
