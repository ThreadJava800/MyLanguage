#include "lang.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        List_t vars = {};
        _listCtor(&vars, 1, 0);
        Node_t* node = readLangFile(argv[1], &vars);
        for (int i = 0; i <= vars.size; i++) {
            if (vars.values[i].value) printf("%.3d %s\n", i, vars.values[i].value);
            else printf("%.3d (nil)\n", i);
        }
        Node_t* con  = makeConnections(node);
        graphDump(con);

        nodeDtor(con);
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }

    return 0;
}