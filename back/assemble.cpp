#include "../lang.h"

int curFuncId = 0;
int allArgCnt = 0;

void readTreeFile(const char* inputName, const char* outputName) {
    ON_ERROR(!inputName || !outputName, "File name is null", );

    FILE* readFile = fopen(inputName, "rb");
    ON_ERROR(!readFile, "Can't open readfile", );
    FILE* outputFile = fopen(outputName, "wb");
    ON_ERROR(!readFile, "Can't open output file", );

    Node_t* node = nullptr;
    readHeader(readFile, outputFile);
    fileToTree(readFile, &node);
    addPrevs(node);

    // graphDump(node);

    readNode(node, node, outputFile);

    fclose(readFile);
    fclose(outputFile);
}

void readHeader(FILE* inputFile, FILE* outputFile) {
    ON_ERROR(!inputFile || !outputFile, "File is null", );

    int varCnt = 0;
    fscanf(inputFile, "%d", &varCnt);
    fprintf(outputFile, "PUSH %d\n", varCnt);
    fprintf(outputFile, "PUSH 1\n");
    fprintf(outputFile, "ADD\n");
    fprintf(outputFile, "POP rbx\n");

    allArgCnt = varCnt;
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

void readNode(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!outputFile, "File is null", );

    switch (node->type) {
        case FICTITIOUS:
            if (L(node)) readNode(root, L(node), outputFile);
            if (R(node)) readNode(root, R(node), outputFile);
            break;
        case VAR:
            {
            fprintf(outputFile, "PUSH 0\n");

            fprintf(outputFile, "PUSH %d\n", allArgCnt - node->value.num);
            fprintf(outputFile, "PUSH rbx\n");
            fprintf(outputFile, "SUB\n");
            fprintf(outputFile, "POP rcx\n");
            fprintf(outputFile, "POP [rcx]\n");
            }
            break;
        case VARIABLE:
            {
                fprintf(outputFile, "PUSH %d\n", allArgCnt - node->value.num);
                fprintf(outputFile, "PUSH rbx\n");
                fprintf(outputFile, "SUB\n");
                fprintf(outputFile, "POP rcx\n");
                fprintf(outputFile, "PUSH [rcx]\n");
            }
            break;
        case NUMBER:
            fprintf(outputFile, "PUSH %d\n", node->value.num);
            break;
        case OPERATOR: 
            readOperator(root, node, outputFile);
            break;
        case IF:
            readIf(root, node, outputFile);
            break;
        case WHILE:
            readWhile(root, node, outputFile);
            break;
        case DEF:
            readFunc(root, node, outputFile);
            break;
        case RETURN:
            {
                readNode(root, L(node), outputFile);
                fprintf(outputFile, "POP rax\n");

                // fprintf(outputFile, "PUSH rbx\n");
                // fprintf(outputFile, "OUT\n");

                fprintf(outputFile, "PUSH %d\n", allArgCnt);
                fprintf(outputFile, "PUSH rbx\n");
                fprintf(outputFile, "SUB\n");
                fprintf(outputFile, "POP rbx\n");

                // fprintf(outputFile, "PUSH rbx\n");
                // fprintf(outputFile, "OUT\n");

                fprintf(outputFile, "RET\n");

                curFuncId = 0;
            }
            break;
        case CALL:
            readCall(root, node, outputFile);
            break;
        default:
            SYNTAX_ERROR(true, "Unknown type: %d", node->type);
            break;
    }
}

void readOperator(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    switch (node->value.num) {
        case ADD_OP:
            readNode(root, L(node), outputFile);
            readNode(root, R(node), outputFile);
            fprintf(outputFile, "ADD\n");
            break;
        case SUB_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "SUB\n");
            break;
        case DIV_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "DIV\n");
            break;
        case MUL_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "MUL\n");
            break;
        case SQRT_OP:
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "SQRT\n");
            break;
        case ASSIGN_OP:
            readAssign(root, node, outputFile);
            break;
        case EQU_OP:
            readNode(root, L(node), outputFile);
            readNode(root, R(node), outputFile);
            fprintf(outputFile, "JE ");
            break;
        case NOT_EQ_OP:
            readNode(root, L(node), outputFile);
            readNode(root, R(node), outputFile);
            fprintf(outputFile, "JNE ");
            break;
        case BIGGER_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "JA ");
            break;
        case LESS_EQ_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "JBE ");
            break;
        case LESS_OP:
            readNode(root, R(node), outputFile);
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "JB ");
            break;
        case OUT_OP:
            readNode(root, L(node), outputFile);
            fprintf(outputFile, "OUT\n");
            break;
        default:
            break;
    }
}

void readIf(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    fprintf(outputFile, "JMP if_main_%p\n", node);

    fprintf(outputFile, "\nif_%p:\n", node);
    readNode(root, L(R(node)), outputFile);
    fprintf(outputFile, "JMP leave_if_%p\n\n", node);

    fprintf(outputFile, "\nelse_%p:\n", node);
    readNode(root, R(R(node)), outputFile);
    fprintf(outputFile, "JMP leave_if_%p\n\n", node);

    fprintf(outputFile, "if_main_%p:\n", node);
    readNode(root, L(node), outputFile);
    fprintf(outputFile, "if_%p\n", node);
    fprintf(outputFile, "JMP else_%p\n", node);

    fprintf(outputFile, "\nleave_if_%p:\n", node);
    if (!R(PREV(node))) fprintf(outputFile, "HLT\n");
}

void readAssign(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    if (IS_IN(R(node))) {
        fprintf(outputFile, "IN\n");

        fprintf(outputFile, "PUSH %d\n", allArgCnt - L(node)->value.num);
        fprintf(outputFile, "PUSH rbx\n");
        fprintf(outputFile, "SUB\n");
        fprintf(outputFile, "POP rcx\n");

        fprintf(outputFile, "POP [rcx]\n");
    } else if (IS_CALL(R(node))) {
        readNode(root, R(node), outputFile);
        fprintf(outputFile, "PUSH rax\n");

        fprintf(outputFile, "PUSH %d\n", allArgCnt - L(node)->value.num);
        fprintf(outputFile, "PUSH rbx\n");
        fprintf(outputFile, "SUB\n");
        fprintf(outputFile, "POP rcx\n");

        fprintf(outputFile, "POP [rcx]\n");
    } else {
        readNode(root, R(node), outputFile);

        fprintf(outputFile, "PUSH %d\n", allArgCnt - L(node)->value.num);
        fprintf(outputFile, "PUSH rbx\n");
        fprintf(outputFile, "SUB\n");
        fprintf(outputFile, "POP rcx\n");

        fprintf(outputFile, "POP [rcx]\n");
    }
}

void readWhile(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    fprintf(outputFile, "JMP while_main_%p\n", node);

    fprintf(outputFile, "\nwhile_%p:\n", node);
    readNode(root, R(node), outputFile);
    fprintf(outputFile, "JMP while_main_%p\n\n", node);

    fprintf(outputFile, "while_main_%p:\n", node);
    readNode(root, L(node), outputFile);
    fprintf(outputFile, "while_%p\n", node);
    fprintf(outputFile, "JMP leave_while_%p\n\n", node);
    fprintf(outputFile, "\nleave_while_%p:\n", node);
    if (!R(PREV(node))) fprintf(outputFile, "HLT\n");
}

void readFunc(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    curFuncId = node->value.num;

    fprintf(outputFile, "JMP continue_%p\n", node);
    fprintf(outputFile, "\nfunc_%d:\n", node->value.num);

    int argCnt = getFuncArgCnt(L(node), VARIABLE);
    int varCnt = getFuncArgCnt(node, VAR);
    parseFuncArgs(root, L(node), outputFile, argCnt + varCnt, argCnt + varCnt);
    readNode(root, R(node), outputFile);
                // fprintf(outputFile, "PUSH rbx\n");
                // fprintf(outputFile, "OUT\n");
    fprintf(outputFile, "\n\n");

    fprintf(outputFile, "\ncontinue_%p:\n", node);
}

void parseFuncArgs(Node_t* root, Node_t* node, FILE* outputFile, int argCount, int parsedCount) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    if (IS_VARIABLE(node)) {
        printf("%d\n", argCount);
        fprintf(outputFile, "PUSH rbx\n");
        fprintf(outputFile, "PUSH %d\n", parsedCount);
        fprintf(outputFile, "PUSH %d\n", argCount);
        fprintf(outputFile, "SUB\n");
        fprintf(outputFile, "ADD\n");

        fprintf(outputFile, "POP rbx\n");

        fprintf(outputFile, "PUSH %d\n", allArgCnt - node->value.num);
        fprintf(outputFile, "PUSH rbx\n");
        fprintf(outputFile, "SUB\n");
        fprintf(outputFile, "POP rcx\n");
        fprintf(outputFile, "POP [rcx]\n");

        parsedCount--;
    }

    if (R(node)) parseFuncArgs(root, R(node), outputFile, argCount, parsedCount);
    if (L(node)) parseFuncArgs(root, L(node), outputFile, argCount, parsedCount);
}

void readCall(Node_t* root, Node_t* node, FILE* outputFile) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );

    List_t list = {};
    _listCtor(&list);

    curFuncId = node->value.num;

    Node_t* funcNode = nullptr;
    getNodeByFuncId(root, &funcNode, node->value.num);
    int argCnt = getFuncArgCnt(L(funcNode), VARIABLE);

    parseCallArgs(root, node, outputFile, &list, argCnt, argCnt);

    for (int i = list.size - 1; i >= 0; i--) {
        ListElement_t* elem = logicToPhysics(&list, i);
        fprintf(outputFile, "%s", elem->value);
    }

    // move pointer
    int varCnt = getFuncArgCnt(funcNode, VAR);
    fprintf(outputFile, "PUSH rbx\n");
    fprintf(outputFile, "PUSH %d\n", allArgCnt);
    fprintf(outputFile, "ADD\n");
    fprintf(outputFile, "POP rbx\n");

    fprintf(outputFile, "CALL func_%d\n", node->value.num);
}

void parseCallArgs(Node_t* root, Node_t* node, FILE* outputFile, List_t* list, int argCount, int parsedCount) {
    ON_ERROR(!node, "Node is null", );
    ON_ERROR(!outputFile, "File is null", );
    ON_ERROR(!list, "File is null", );

    if (IS_VARIABLE(node)) {
        char com[MAX_WORD_LENGTH] = "";
        int moves = 0;

        moves += sprintf(com + moves, "PUSH rbx\n");
        moves += sprintf(com + moves, "PUSH %d\n", parsedCount);
        moves += sprintf(com + moves, "PUSH %d\n", argCount);
        moves += sprintf(com + moves, "SUB\n");
        moves += sprintf(com + moves, "ADD\n");
        moves += sprintf(com + moves, "POP rbx\n");

        moves += sprintf(com + moves, "PUSH %d\n", allArgCnt - node->value.num);
        moves += sprintf(com + moves, "PUSH rbx\n");
        moves += sprintf(com + moves, "SUB\n");
        moves += sprintf(com + moves, "POP rcx\n");
        moves += sprintf(com + moves, "PUSH [rcx]\n");
        listPushBack(list, strdup(com));

        parsedCount--;
    }

    if (R(node)) parseCallArgs(root, R(node), outputFile, list, argCount, parsedCount);
    if (L(node)) parseCallArgs(root, L(node), outputFile, list, argCount, parsedCount);
}

int getFuncArgCnt(Node_t* root, NodeType type) {
    ON_ERROR(!root, "Node is null", 0);

    if (root->type == type) {
        return 1;
    }
    if (!(L(root)) && !(R(root))) {
        return 0;
    }
    return getFuncArgCnt(L(root), type) + getFuncArgCnt(R(root), type);
}

void getNodeByFuncId(Node_t* root, Node_t** cur, int id) {
    ON_ERROR(!root, "Node is null", );

    if ((root->type == DEF) && (root->value.num == id)) {
        *cur = root;
    }

    if (L(root)) getNodeByFuncId(L(root), cur, id);
    if (R(root)) getNodeByFuncId(R(root), cur, id);
}