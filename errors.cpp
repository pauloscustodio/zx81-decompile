//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include <sstream>
using namespace std;

static Errors g_errors;

// does not clear count
void Errors::clear() {
	m_line_num = 0;
	m_filename.clear();
}

void Errors::set_filename(const string& filename) {
	m_filename = filename;
	m_line_num = 0;
}

void Errors::set_line_num(int line_num) {
	m_line_num = line_num;
}

void Errors::error(const string& message, const string& arg) {
	m_count++;

	if (!m_filename.empty()) {
		cerr << m_filename << ":";
		if (m_line_num > 0)
			cerr << m_line_num << ":";
		cerr << " ";
	}
	
	cerr << "error: " << message;

	if (!arg.empty())
		cerr << ": " << arg;
	
	cerr << endl;
}

void Errors::fatal_error(const string& message, const string& arg) {
	error(message, arg);
	exit(EXIT_FAILURE);
}

void Errors::exit_status() const {
	if (m_count == 0)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

void err_clear() {
	g_errors.clear();
}

void err_set_filename(const string& filename) {
	g_errors.set_filename(filename);
}

void err_set_line_num(int line_num) {
	g_errors.set_line_num(line_num);
}

string err_get_filename() {
	return g_errors.get_filename();
}

int err_get_line_num() {
	return g_errors.get_line_num();
}

int err_get_count() {
	return g_errors.get_count();
}

void error(const string& message, const string& arg) {
	g_errors.error(message, arg);
}

void fatal_error(const string& message, const string& arg) {
	g_errors.fatal_error(message, arg);
}

void exit_status() {
	g_errors.exit_status();
}
