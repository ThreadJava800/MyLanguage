#include "lang.h"

Node_t* readLangFile(const char* fileName) {
    ON_ERROR(!fileName, "File name was null", nullptr);

    FILE* file = fopen(fileName, "rb");
    ON_ERROR(!file, "File not found", nullptr);

    Node_t* entities = parseFile(file);
    fclose(file);

    return entities;
}

Node_t* parseFile(FILE* file) {
    ON_ERROR(!file, "File is null", nullptr);

    Node_t* curNode = nullptr;
    int symb = EOF;
    while ((symb = fgetc(file)) != EOF) {
        ungetc(symb, file);
        SKIP_SPACES();

        List_t list = {};
        _listCtor(&list, 1, 0);
        curNode = parseWord(file, curNode, &list);
        SKIP_SPACES();

        curNode = parseNum(file, curNode);
        curNode = parseOper(file, curNode);
    }

    curNode = headStart(curNode);
    graphDump(curNode);

    return curNode;
}

Node_t* parseOper(FILE* file, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);

    int symb = fgetc(file);
    switch (symb) {
        case '+':
            return nodeCtor(OPERATOR, {.opt = ADD_OP}, nullptr, nullptr, prev);
        case '-':
            return nodeCtor(OPERATOR, {.opt = SUB_OP}, nullptr, nullptr, prev);
        case '*':
            return nodeCtor(OPERATOR, {.opt = MUL_OP}, nullptr, nullptr, prev);
        case '/':
            return nodeCtor(OPERATOR, {.opt = DIV_OP}, nullptr, nullptr, prev);
        default:
            ungetc(symb, file);
            break;
    }
    return prev;
}

Node_t* parseNum(FILE* file, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);

    int symb = fgetc(file);
    int value = 0;
    bool isNeg = false, isValue = false;

    if (symb == '-') {
        symb = fgetc(file);
        isNeg = true;
    }
    while (isdigit(symb)) {
        isValue = true;
        value = value * 10 + (symb - '0');
        symb = fgetc(file);
    }
    if (isNeg) value *= -1;
    if (isNeg && !isValue) {
        ungetc(symb, file);
        ungetc('-', file);
    }
    else ungetc(symb, file);

    if (isValue) return nodeCtor(NUMBER, {.num = value}, nullptr, nullptr, prev);

    return prev;
}

Node_t* parseWord(FILE* file, Node_t* prev, List_t* vars) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!vars, "List is null", nullptr);

    char command[MAX_WORD_LENGTH] = "";
    int symbCount = getWord(file, command);
    if (symbCount) {
        if (!strcmp(command, ifCom)) {
            return nodeCtor(IF, {}, nullptr, nullptr, prev);
        } else if(!strcmp(command, whileCom)) {
            return nodeCtor(WHILE, {}, nullptr, nullptr, prev);
        } else {
            char* varName = strdup(command);
            listPushBack(vars, varName);
            return nodeCtor(VARIABLE, {.var = varName}, nullptr, nullptr, prev);
        }
    }

    return prev;
}

int getWord(FILE* file, char* buffer) {
    ON_ERROR(!file, "File is null", 0);
    ON_ERROR(!buffer, "Buffer is null", 0);

    int symb = fgetc(file);
    if (isdigit(symb)) {
        int nextSymb = fgetc(file);
        SYNTAX_ERROR((isalpha(nextSymb) || nextSymb == '_'), "Incorrect var name! %c", symb);

        ungetc(nextSymb, file);
        ungetc(symb, file);
        return 0;
    }

    int symbCount = 0;

    while (!isspace(symb) && symb != EOF) {
        SYNTAX_ERROR(!(isalnum(symb) || symb == '_'), "Incorrect var name! %c", symb);

        *buffer = (char) symb;
        buffer++;
        symbCount++;

        symb = fgetc(file);
    }
    ungetc(symb, file);

    return symbCount;
}

Node_t* headStart(Node_t* end) {
    ON_ERROR(!end, "Node is null", nullptr);

    if (!end->prev) return end;

    end->prev->right = end;
    return headStart(end->prev);
}