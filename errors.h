//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2024
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <iostream>
using namespace std;

extern int error_count;

#define ERROR(x)		do { cerr << "error: " << x << endl; error_count++; } while (false)
#define FATAL_ERROR(x)	do { ERROR(x); exit(EXIT_FAILURE); } while (false)
#define EXIT_STATUS()	do { if (error_count) exit(EXIT_FAILURE); else exit(EXIT_SUCCESS); } while (false)
