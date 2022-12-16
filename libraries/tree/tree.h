#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

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

    // NOT STANDART
    ELSE          = -2,       
};

enum OpType_t {
    ADD_OP         =  1,
    SUB_OP         =  2,
    MUL_OP         =  3,
    DIV_OP         =  4,
    POW_OP         =  5,
    OUT_OP         =  6,
    IN_OP          =  7,
    EQU_OP         =  8,
    BIGGER_OP      =  9,
    LESS_OP        =  10,
    BIGGER_EQ_OP   =  11,
    LESS_EQ_OP     =  12,
    NOT_EQ_OP      =  13,
    NOT_OP         =  14,
    OR_OP          =  15,
    AND_OP         =  16,
    ASSIGN_OP      =  17,
    SQRT_OP        =  18,
    
    OPT_DEFAULT    = -1,

    // NOT STANDART
    END_LINE_OP    = -2,
    O_CIR_BR_OP    = -3,
    C_CIR_BR_OP    = -4,
    O_FIG_BR_OP    = -5,
    C_FIG_BR_OP    = -6,
};

typedef Type_t NodeType;

const NodeType defaultValue = NODET_DEFAULT;

union NodeValue 
{
    int         num;
    OpType_t    opt;
    char*       var;
};

typedef void (*PrintFunction_t)(FILE *file, NodeType type, NodeValue value);

void printElemT(FILE *file, NodeType type, NodeValue value);

struct Node_t {
    NodeType type  = defaultValue;
    NodeValue value;
    int argCnt = 0;

    Node_t *left  = nullptr;
    Node_t *right = nullptr;
    Node_t *prev  = nullptr;


    PrintFunction_t printFunc = printElemT;
};

#define IS_OP(node) (node)->type == OPERATOR

#define L(node) (node)->left
#define LL(node) (node)->left->left
#define LR(node) (node)->left->right

#define R(node) (node)->right

#define PREV(node) (node)->prev

#define IS_NUM_VAR(node) (node)->type == NUMBER || (node)->type == VARIABLE
#define IS_IF(node) (node)->type == IF
#define IS_RET(node) (node)->type == RETURN
#define IS_CALL(node) (node)->type == CALL
#define IS_WHILE(node) (node)->type == WHILE
#define IS_ELSE(node) (node)->type == ELSE
#define IS_VAR(node) (node)->type == VAR
#define IS_DEF(node) (node)->type == DEF
#define IS_VARIABLE(node) (node)->type == VARIABLE
#define IS_NUM(node) (node)->type == NUMBER

#define IS_OUT(node) ((node)->type == OPERATOR) && ((node)->value.opt == OUT_OP)
#define IS_SQRT(node) ((node)->type == OPERATOR) && ((node)->value.opt == SQRT_OP)
#define IS_IN(node) ((node)->type == OPERATOR) && ((node)->value.opt == IN_OP)
#define IS_O_CR_BR(node) ((node)->type == OPERATOR) && ((node)->value.opt == O_CIR_BR_OP)
#define IS_C_CR_BR(node) ((node)->type == OPERATOR) && ((node)->value.opt == C_CIR_BR_OP)
#define IS_O_FIG_BR(node) ((node)->type == OPERATOR) && ((node)->value.opt == O_FIG_BR_OP)
#define IS_C_FIG_BR(node) ((node)->type == OPERATOR) && ((node)->value.opt == C_FIG_BR_OP)
#define IS_CMP_OPS(node) whatOper((node), EQU_OP) ||       whatOper((node), BIGGER_OP) ||  whatOper((node), LESS_OP)  \
                      || whatOper((node), BIGGER_EQ_OP) || whatOper((node), LESS_EQ_OP) || whatOper((node), NOT_EQ_OP) \
                      || whatOper((node), EQU_OP)

// DECLARATIONS

Node_t* nodeCopy(Node_t* nodeToCopy);

bool whatOper(Node_t* node, OpType_t oper);

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