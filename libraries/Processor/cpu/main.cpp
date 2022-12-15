#include "cpu.h"

int main(int argc, char* argv[]) {
    if (argc == 2) {
        execute(argv[1]);
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }
    return 0;
}