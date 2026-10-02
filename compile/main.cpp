#include "utils.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    std::cout << basename(argv[0]) << std::endl;
}
