#ifndef LANG_H
#define LANG_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#include "tree/tree.h"
#include "LinkedList/list.h"
#include "syntax.h"

const int MAX_WORD_LENGTH = 4096;
const int MAX_OPER_LENGTH = 4;

#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \

#define SYNTAX_ERROR(expr, fmt, ...) {                                   \
    if (expr) {                                                           \
        fprintf(stderr, "SYNTAX ERROR: " fmt "\n", ##__VA_ARGS__);         \
        abort();                                                            \
    }                                                                        \
}                                                                             \

#define SKIP_SPACES() {         \
    int c = fgetc(file);         \
    while (isspace(c)) {          \
        c = fgetc(file);           \
    }                               \
    ungetc(c, file);                 \
}                                     \

// FRONT-END

Node_t* readLangFile(const char* fileName);

Node_t* parseFile(FILE* file);

Node_t* parseOper(FILE* file, Node_t* prev);

int getOper(FILE* file, char* buffer);

bool isOperInList(int symb);

Node_t* parseNum(FILE* file, Node_t* prev);

Node_t* parseWord(FILE* file, Node_t* prev, List_t* vars);

int getWord(FILE* file, char* buffer);

Node_t* headStart(Node_t* end);

#endif