#ifndef TREE_H
#define TREE_H

#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

#if _DEBUG
void closeLogfile(void);

FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

enum Type_t {
    OP            =  0,
    VAR           =  1,
    NUM           =  2,
    NODET_DEFAULT = -1,
};

typedef Type_t NodeType;

const NodeType defaultValue = NODET_DEFAULT;

typedef void (*PrintFunction_t)(FILE *file, NodeType value);

void printElemT(FILE *file, NodeType val);

struct Node_t {
    NodeType val = defaultValue;

    Node_t *left  = nullptr;
    Node_t *right = nullptr;
    Node_t *prev  = nullptr;


    PrintFunction_t printFunc = printElemT;
};

// DECLARATIONS

Node_t* nodeCtor(NodeType value, Node_t *left, Node_t *right, Node_t *previous, PrintFunction_t printFunc);                                                                                                 \

Node_t* addRightLeaf(Node_t *node, NodeType value);

Node_t* addLeftLeaf(Node_t *node, NodeType value);

void nodeDtor(Node_t *node);

void dumpNode(FILE* file, Node_t *node);

void graphNode(Node_t *node, FILE *tempFile);

void graphDump(Node_t *node);

// REALIZATIONS

void printElemT(FILE *file, NodeType val) {
    fprintf(file, "%d", val);
}

Node_t* nodeCtor(NodeType val, Node_t *left, Node_t *right, Node_t *prev,  PrintFunction_t printFunc) {
    Node_t *node = (Node_t*) calloc(1, sizeof(Node_t));
    if (!node) return nullptr;

    node->val    = val;
    node->left     = left;
    node->right    = right;
    node->prev = prev;

    node->printFunc = printFunc;

    return node;
}

Node_t* addRightLeaf(Node_t *node, NodeType val) {
    if (!node) return nullptr;

    Node_t *childNode = nodeCtor(val, nullptr, nullptr, node, node->printFunc);
    if (!childNode) return nullptr;

    node->right = childNode;

    return childNode;
}

Node_t* addLeftLeaf(Node_t *node, NodeType val) {
    if (!node) return nullptr;

    Node_t *childNode = nodeCtor(val, nullptr, nullptr, node, node->printFunc);
    if (!childNode) return nullptr;

    node->left = childNode;

    return childNode;
}

void nodeDtor(Node_t *node) {
    if (!node) return;

    if (node->left) nodeDtor(node->left);
    if (node->right) nodeDtor(node->right);

    free(node);
}

void dumpNode(FILE* file, Node_t *node) {
    if (!node) return;

    fprintf(file, "(");

    if (node->left) dumpNode(file, node->left);

    node->printFunc(logFile, node->val);

    if (node->right) dumpNode(file, node->right);
    fprintf(file, ")");
}

void graphNode(Node_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: ",
                node
            );

    node->printFunc(tempFile, node->val);
    
    fprintf(
                tempFile, 
                " | {left: %p | right: %p} | prev: %p }}\"];\n", 
                node->left,
                node->right,
                node->prev
            );

    if (node->prev) {
        fprintf(tempFile, "\tlabel%p->label%p [color=\"red\", style=\"dashed\",arrowhead=\"none\"]", node->prev, node);
    }

    if (node->left) graphNode(node->left, tempFile);

    if (node->right) graphNode(node->right, tempFile);
}

void graphDump(Node_t *node) {
    if (!node) return;

    FILE *tempFile = fopen("temp.dot", "w");
    fprintf(tempFile, "digraph tree {\n");
    fprintf(tempFile, "\trankdir=HR;\n");
    if (!tempFile) return;

    graphNode(node, tempFile);

    fprintf(tempFile, "}");
    fclose(tempFile);

    system("dot -Tsvg temp.dot > graph.png");
}

#if _DEBUG
void closeLogfile(void) {
    if (logFile) fclose(logFile);
}
#endif

#endif