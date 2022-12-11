#include "lang.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        Node_t* node = readLangFile(argv[1]);
        Node_t* con = makeConnections(node);
        graphDump(con);
        // TODO: remove unneccessary fict nodes
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }

    return 0;
}