//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "decompiler.h"
#include "errors.h"
#include "getopt.h"
#include "memory.h"
#include <filesystem>
#include <iostream>
#include <string>
using namespace std;

static void exit_usage() {
	cerr << "Usage: zx81_decompile [-o file.b81] [-d] file.p" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	string out_file;
	string ctl_file;

	while (true) {
		int c = getopt(argc, argv, const_cast<char*>("do:c:"));
		if (c == -1)
			break;
		switch (c) {
		case 'c':
			ctl_file = optarg;
			break;
		case 'o':
			out_file = optarg;
			break;
		case 'd':
			optflags |= FLAG_DEBUG;
			break;
		default:
			exit_usage();
		}
	}

	if (argc != optind + 1)
		exit_usage();

	string p_file = argv[optind];
	if (out_file.empty())
		out_file = filesystem::path(p_file).replace_extension(".b81").generic_string();

	read_p_file(p_file);
	if (err_get_count())
		exit_status();

	Basic basic;
	if (ctl_file.empty())
		decompile(basic);
	else
		decompile(basic, ctl_file);
	if (err_get_count())
		exit_status();

	basic.write_b81_file(out_file);

	exit_status();
}
