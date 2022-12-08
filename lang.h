#ifndef LANG_H
#define LANG_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#include "tree/tree.h"

#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \

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

Node_t* parseNum(FILE* file, Node_t* prev);

Node_t* headStart(Node_t* end);

#endif