#include "assembler.h"
#include <cctype>
using namespace std;

void Assembler::skip_spaces() {
	while (*p != '\0' && isspace(*p))
		p++;
}
