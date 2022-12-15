#include "assembler.h"

int main(int argc, char* argv[]) {
    if (argc == 3) {
        execute(argv[1], argv[2]);
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }
    return 0;
}