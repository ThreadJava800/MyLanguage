#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

enum Type_t {
    NODET_DEFAULT = -1,
    FICTITIOUS    =  0,
    NUMBER        =  1,
    VARIABLE      =  2,
    IF            =  3,
    IF2           =  4,
    WHILE         =  5,
    OPERATOR      =  6,
    VAR           =  7,
    DEF           =  8,
    CALL          =  9,
    RETURN        =  10,         
};

enum OpType_t {
    MUL_OP         =  0,
    ADD_OP         =  1,
    DIV_OP         =  2,
    SUB_OP         =  3,
    OPT_DEFAULT    = -1,
};

typedef Type_t NodeType;

const NodeType defaultValue = NODET_DEFAULT;

union NodeValue 
{
    int         num;
    OpType_t    opt;
    char        var;
};

typedef void (*PrintFunction_t)(FILE *file, NodeType type, NodeValue value);

void printElemT(FILE *file, NodeType type, NodeValue value);

struct Node_t {
    NodeType type  = defaultValue;
    NodeValue value;

    Node_t *left  = nullptr;
    Node_t *right = nullptr;
    Node_t *prev  = nullptr;


    PrintFunction_t printFunc = printElemT;
};

// DECLARATIONS

Node_t* nodeCtor(NodeType type, NodeValue value, Node_t *left, Node_t *right, Node_t *previous, PrintFunction_t printFunc = printElemT);

Node_t* addRightLeaf(Node_t *node, NodeType type, NodeValue value);

Node_t* addLeftLeaf(Node_t *node, NodeType type, NodeValue value);

void nodeDtor(Node_t *node);

void dumpNode(FILE* file, Node_t *node);

void graphNode(Node_t *node, FILE *tempFile);

void graphDump(Node_t *node);

#if _DEBUG
void closeLogfile(void);
#endif

#endif