//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include <sstream>

static Errors g_errors;

// does not clear count
void Errors::clear() {
    m_line_num = 0;
    m_filename.clear();
}

void Errors::set_filename(const std::string& filename) {
    m_filename = filename;
    m_line_num = 0;
}

void Errors::set_line_num(int line_num) {
    m_line_num = line_num;
}

void Errors::error(const std::string& message, const std::string& arg) {
    m_count++;

    if (!m_filename.empty()) {
        std::cerr << m_filename << ":";
        if (m_line_num > 0) {
            std::cerr << m_line_num << ":";
        }
        std::cerr << " ";
    }

    std::cerr << "error: " << message;

    if (!arg.empty()) {
        std::cerr << ": " << arg;
    }

    std::cerr << std::endl;
}

void Errors::fatal_error(const std::string& message, const std::string& arg) {
    error(message, arg);
    exit(EXIT_FAILURE);
}

void Errors::exit_status() const {
    if (m_count == 0) {
        exit(EXIT_SUCCESS);
    }
    else {
        exit(EXIT_FAILURE);
    }
}

void err_clear() {
    g_errors.clear();
}

void err_set_filename(const std::string& filename) {
    g_errors.set_filename(filename);
}

void err_set_line_num(int line_num) {
    g_errors.set_line_num(line_num);
}

std::string err_get_filename() {
    return g_errors.get_filename();
}

int err_get_line_num() {
    return g_errors.get_line_num();
}

int err_get_count() {
    return g_errors.get_count();
}

void error(const std::string& message, const std::string& arg) {
    g_errors.error(message, arg);
}

void fatal_error(const std::string& message, const std::string& arg) {
    g_errors.fatal_error(message, arg);
}

void exit_status() {
    g_errors.exit_status();
}
