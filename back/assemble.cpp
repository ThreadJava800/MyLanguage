#include "../lang.h"

void readTreeFile(const char* inputName, const char* outputName) {
    ON_ERROR(!inputName || !outputName, "File name is null", );

    FILE* readFile = fopen(inputName, "rb");
    ON_ERROR(!readFile, "Can't open readfile", );
    FILE* outputFile = fopen(outputName, "wb");
    ON_ERROR(!readFile, "Can't open output file", );

    Node_t* node = nullptr;
    fileToTree(readFile, &node);
    addPrevs(node);

    graphDump(node);

    readNode(node, outputFile);

    fclose(readFile);
    fclose(outputFile);
}

void fileToTree(FILE* inputFile, Node_t** node) {
    ON_ERROR(!inputFile, "File is null", );

    int symb = fgetc(inputFile);
    while(symb != '{' && symb != EOF) {
        symb = fgetc(inputFile);
    }

    if (symb == EOF) return;

    int type = -1, index = -1;
    fscanf(inputFile, " %d %d ", &type, &index);
    *node = nodeCtor((NodeType) type, {.num = index}, nullptr, nullptr, nullptr);

    symb = fgetc(inputFile);

    if (symb == '}') {
        L(*node) = R(*node) = nullptr;
    } else {
        ungetc('{', inputFile);
        fileToTree(inputFile, &(L(*node)));
        fileToTree(inputFile, &(R(*node)));
    }
}

void readNode(Node_t* node, FILE* outputFile) {
    ON_ERROR(!outputFile, "File is null", );

    switch (node->type) {
        case FICTITIOUS:
            if (L(node)) readNode(L(node), outputFile);
            if (R(node)) readNode(R(node), outputFile);
            break;
        case VAR:
            fprintf(outputFile, "PUSH 0\n");
            fprintf(outputFile, "POP [%d]\n", node->value.num);
            break;
        case VARIABLE:
            fprintf(outputFile, "PUSH [%d]\n", node->value.num);
            break;
        case NUMBER:
            fprintf(outputFile, "PUSH %d\n", node->value.num);
            break;
        case OPERATOR: 
            readOperator(node, outputFile);
            break;
        case IF:
            readIf(node, outputFile);
            break;
        case WHILE:
            readWhile(node, outputFile);
            break;
        case DEF:
            readFunc(node, outputFile);
            break;
        case RETURN:
            readNode(L(node), outputFile);
            fprintf(outputFile, "POP [rax]\n");
            fprintf(outputFile, "RET");
            break;
        case CALL:
            readCall(node, outputFile);
            break;
        default:
            SYNTAX_ERROR(true, "Unknown type: %d", node->type);
            break;
    }
}

void readOperator(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    switch (node->value.num) {
        case ADD_OP:
            readNode(L(node), outputFile);
            readNode(R(node), outputFile);
            fprintf(outputFile, "ADD\n");
            break;
        case SUB_OP:
            readNode(R(node), outputFile);
            readNode(L(node), outputFile);
            fprintf(outputFile, "SUB\n");
            break;
        case ASSIGN_OP:
            readAssign(node, outputFile);
            break;
        case EQU_OP:
            readNode(L(node), outputFile);
            readNode(R(node), outputFile);
            fprintf(outputFile, "JE ");
            break;
        case BIGGER_OP:
            readNode(R(node), outputFile);
            readNode(L(node), outputFile);
            fprintf(outputFile, "JA ");
            break;
        case OUT_OP:
            readNode(L(node), outputFile);
            fprintf(outputFile, "OUT\n");
            break;
        default:
            break;
    }
}

void readIf(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    fprintf(outputFile, "JMP if_main_%p\n", node);

    fprintf(outputFile, "\nif_%p:\n", node);
    readNode(L(R(node)), outputFile);
    fprintf(outputFile, "JMP leave_if_%p\n\n", node);

    fprintf(outputFile, "\nelse_%p:\n", node);
    readNode(R(R(node)), outputFile);
    fprintf(outputFile, "JMP leave_if_%p\n\n", node);

    fprintf(outputFile, "if_main_%p:\n", node);
    readNode(L(node), outputFile);
    fprintf(outputFile, "if_%p\n", node);
    fprintf(outputFile, "JMP else_%p\n", node);

    fprintf(outputFile, "\nleave_if_%p:\n", node);
    if (!R(PREV(node))) fprintf(outputFile, "HLT\n");
}

void readAssign(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    if (IS_IN(R(node))) {
        printf("in");
        fprintf(outputFile, "IN\n");
        fprintf(outputFile, "POP [%d]\n", L(node)->value.num);
    } else if (IS_CALL(R(node))) {
        readNode(R(node), outputFile);
        fprintf(outputFile, "PUSH [rax]\n");
        fprintf(outputFile, "POP [%d]\n", L(node)->value.num);
    } else {
        readNode(R(node), outputFile);
        fprintf(outputFile, "POP [%d]\n", L(node)->value.num);
    }
}

void readWhile(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    fprintf(outputFile, "JMP while_main_%p\n", node);

    fprintf(outputFile, "\nwhile_%p:\n", node);
    readNode(R(node), outputFile);
    fprintf(outputFile, "JMP while_main_%p\n\n", node);

    fprintf(outputFile, "while_main_%p:\n", node);
    readNode(L(node), outputFile);
    fprintf(outputFile, "while_%p\n", node);
    fprintf(outputFile, "JMP leave_while_%p\n\n", node);
    fprintf(outputFile, "\nleave_while_%p:\n", node);
    if (!R(PREV(node))) fprintf(outputFile, "HLT\n");
}

void readFunc(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    fprintf(outputFile, "JMP continue_%p\n", node);

    fprintf(outputFile, "\nfunc_%d:\n", node->value.num);
    parseFuncArgs(L(node), outputFile);
    readNode(R(node), outputFile);
    fprintf(outputFile, "\n\n");

    fprintf(outputFile, "\ncontinue_%p:\n", node);
}

void parseFuncArgs(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    if (IS_VARIABLE(node)) {
        fprintf(outputFile, "POP [%d]\n", node->value.num);
    }

    if (R(node)) parseFuncArgs(R(node), outputFile);
    if (L(node)) parseFuncArgs(L(node), outputFile);
}

void readCall(Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    List_t list = {};
    _listCtor(&list);
    parseCallArgs(node, outputFile, &list);
    for (int i = list.size - 1; i >= 0; i--) {
        ListElement_t* elem = logicToPhysics(&list, i);
        fprintf(outputFile, "%s", elem->value);
    }
    fprintf(outputFile, "CALL func_%d\n", node->value.num);
}

void parseCallArgs(Node_t* node, FILE* outputFile, List_t* list) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );
    ON_ERROR(!list, "File is null", );

    if (IS_VARIABLE(node)) {
        printf("in");
        char com[MAX_WORD_LENGTH] = "";
        sprintf(com, "PUSH [%d]\n", node->value.num);
        listPushBack(list, strdup(com));
    }

    if (R(node)) parseCallArgs(R(node), outputFile, list);
    if (L(node)) parseCallArgs(L(node), outputFile, list);
}