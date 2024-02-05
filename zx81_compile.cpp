//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "errors.h"
#include "getopt.h"
#include "zx81.h"
#include <filesystem>
#include <iostream>
#include <string>
using namespace std;

static void exit_usage() {
	cerr << "Usage: zx81_compile [-o file.p] file.b81" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	string out_file;

	while (true) {
		int c = getopt(argc, argv, const_cast<char*>("o:"));
		if (c == -1)
			break;
		switch (c) {
		case 'o':
			out_file = optarg;
			break;
		default:
			exit_usage();
		}
	}

	if (argc != optind + 1)
		exit_usage();

	string b81_file = argv[optind];
	if (out_file.empty())
		out_file = filesystem::path(b81_file).replace_extension(".p").generic_string();

	ZX81 zx81;
	zx81.read_b81_file(b81_file);
	zx81.compile();
	zx81.write_p_file(out_file);

	EXIT_STATUS();
}
