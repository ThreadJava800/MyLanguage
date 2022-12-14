#include "../lang.h"

void printTree(const char* outputName, Node_t* tree, List_t* vars) {
    ON_ERROR(!outputName, "Name is not provided", );
    ON_ERROR(!tree, "Tree is null", );
    ON_ERROR(!vars, "List is null", )

    FILE* outputFile = fopen(outputName, "w");
    ON_ERROR(!outputFile, "Error opening file!", );

    printHeader(outputFile, vars);
    printNode(outputFile, tree);
    fclose(outputFile);
}

void printHeader(FILE* file, List_t* vars) {
    ON_ERROR(!vars, "List is null", )
    ON_ERROR(!file, "File is null", )

    fprintf(file, "%ld\n", vars->size);
    for (int i = 0; i < vars->size; i++) {
        ListElement_t* value = logicToPhysics(vars, i);
        fprintf(file, "%s\n", value->value);
    }
}

void printNode(FILE* file, Node_t* node, int tabCount) {
    ON_ERROR(!file, "File is null", );
    ON_ERROR(!node, "Node is null", );

    ADD_TABS();
    fprintf(file, "{ %d %d", node->type, node->value.num);
    if (L(node)) {
        fprintf(file, "\n");
        printNode(file, L(node), ++tabCount);
    }
    if (R(node)) {
        fprintf(file, "\n");
        if (!(L(node))) printNode(file, R(node), ++tabCount);
        else printNode(file, R(node), tabCount);
    }

    if (R(node) || L(node)) {
        fprintf(file, "\n");
        tabCount--;
        ADD_TABS();
    }
    else fprintf(file, " ");

    fprintf(file, "}");
}