#include "lang.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        List_t vars = {}, funcs = {}, fParams = {};
        _listCtor(&vars);
        _listCtor(&funcs);
        _listCtor(&fParams);
        Node_t* node = readLangFile(argv[1], &vars, &funcs, &fParams);

        // printf("FUNCS: %ld\n", funcs.size);
        // for (int i = 0; i < funcs.size; i++) {
        //     ListElement_t* elem = logicToPhysics(&funcs, i);
        //     printf("%s\n", elem->value);
        // }

        printf("VARS: %ld\n", vars.size);
        for (int i = 0; i < vars.size; i++) {
            ListElement_t* elem = logicToPhysics(&vars, i);
            printf("%s\n", elem->value);
        }

        // printf("FPARAMS: %ld\n", fParams.size);
        // for (int i = 0; i < fParams.size; i++) {
        //     ListElement_t* elem = logicToPhysics(&fParams, i);
        //     printf("%s\n", elem->value);
        // }

        // Node_t* con  = makeConnections(node);
        graphDump(node);
        // printTree("test.o", con, &vars);
        // readTreeFile("test.o", "ass.txt");

        // system("./libraries/Processor/assembler/assembler ass.txt ../../../output.txt");
        // system("./libraries/Processor/cpu/cpu ../../../output.txt");

        // nodeDtor(con);
    } else {
        fprintf(stderr, "Incorrect arguments provided\n");
    }

    return 0;
}