#ifndef LANG_H
#define LANG_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#include "libraries/tree/tree.h"
#include "libraries/LinkedList/list.h"
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

// SYNTAX ANALYZER

Node_t* readLangFile(const char* fileName, List_t* vars);

Node_t* parseFile(FILE* file, List_t* vars);

Node_t* parseOper(FILE* file, Node_t* prev);

int getOper(FILE* file, char* buffer);

bool isOperInList(int symb);

Node_t* parseNum(FILE* file, Node_t* prev);

Node_t* parseWord(FILE* file, Node_t* prev, List_t* vars);

Node_t* newVar(FILE* file, List_t* vars, Node_t* prev);

Node_t* checkVariable(char* varName, List_t* vars, Node_t* prev);

int getWord(FILE* file, char* buffer);

Node_t* headStart(Node_t* end);

// RECURSIVE DESCENT

Node_t* setOper(Node_t* val1, Node_t* val2, Node_t* oper);

void addPrevs(Node_t* start);

Node_t* head(Node_t* cur);

Node_t* makeConnections(Node_t* info);

Node_t* getG(Node_t** info);

Node_t* getVar(Node_t** info);

Node_t* getE(Node_t** info);

Node_t* getT(Node_t** info);

Node_t* getM(Node_t** info);

Node_t* getCP(Node_t** info);

Node_t* getX(Node_t** info);

Node_t* getIF(Node_t** info);

Node_t* getElse(Node_t** info);

Node_t* getDoNode(Node_t** info);

// TREE BUILDER

void printTree(const char* outputName, Node_t* tree, List_t* vars);

void printHeader(FILE* file, List_t* vars);

#define ADD_TABS() {                                        \
    for (int i = 0; i < tabCount; i++) fprintf(file, "\t");  \
}                                                             \

void printNode(FILE* file, Node_t* node, int tabCount = 0);

// BACK_END

void readTreeFile(const char* inputName, const char* outputName);

void fileToTree(FILE* inputFile, Node_t** node);

void readNode(Node_t* node, FILE* outputFile);

void readOperator(Node_t* node, FILE* outputFile);

void readIf(Node_t* node, FILE* outputFile);

#endif