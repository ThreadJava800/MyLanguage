#include "lang.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        List_t vars = {};
        _listCtor(&vars);
        Node_t* node = readLangFile(argv[1], &vars);
        Node_t* con  = makeConnections(node);
        graphDump(con);
        printTree("test.o", con, &vars);
        readTreeFile("test.o", "ass.txt");

        nodeDtor(con);
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }

    return 0;
}