#include "tree.h"

#if _DEBUG
FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

void printElemT(FILE *file, NodeType type, NodeValue value) {
    switch (type) {
        case FICTITIOUS:
            fprintf(file, "FICTITIOUS");
            break;
        case NUMBER:
            fprintf(file, "NUMBER %d", value.num);
            break;
        case VARIABLE:
            fprintf(file, "VARIABLE %d", value.num);
            break;
        case IF:
            fprintf(file, "IF");
            break;
        case IF2:
            fprintf(file, "IF2");
            break;
        case ELSE:
            fprintf(file, "ELSE");
            break;
        case WHILE:
            fprintf(file, "WHILE");
            break;
        case OPERATOR:
            fprintf(file, "OPERATOR %d", value.opt);
            break;
        case VAR:
            fprintf(file, "VARIABLE DECLARATION %d", value.num);
            break;
        case DEF:
            fprintf(file, "DEF");
            break;
        case CALL:
            fprintf(file, "CALL");
            break;
        case RETURN:
            fprintf(file, "RETURN");
            break;
        case NODET_DEFAULT:
        default:
            fprintf(file, "ERR: DEFAULT");
            break;
    }
}

bool whatOper(Node_t* node, OpType_t oper) {
    if (!node) return false;

    if (IS_OP(node)) {
        if ((node)->value.opt == oper) return true;
    }
    return false;
}

Node_t* nodeCopy(Node_t* nodeToCopy) {
    if (!nodeToCopy) return nullptr;

    Node_t* node = (Node_t*) calloc(sizeof(Node_t), 1);
    memcpy(node, nodeToCopy, sizeof(Node_t));

    return node;
}

Node_t* nodeCtor(NodeType type, NodeValue value, Node_t *left, Node_t *right, Node_t *prev,  PrintFunction_t printFunc) {
    Node_t *node = (Node_t*) calloc(1, sizeof(Node_t));
    if (!node) return nullptr;

    node->type      = type;
    node->value     = value;
    node->left      = left;
    node->right     = right;
    node->prev      = prev;

    node->printFunc = printFunc;

    return node;
}

Node_t* addRightLeaf(Node_t *node, NodeType type, NodeValue value) {
    if (!node) return nullptr;

    Node_t *childNode = nodeCtor(type, value, nullptr, nullptr, node, node->printFunc);
    if (!childNode) return nullptr;

    node->right = childNode;

    return childNode;
}

Node_t* addLeftLeaf(Node_t *node, NodeType type, NodeValue value) {
    if (!node) return nullptr;

    Node_t *childNode = nodeCtor(type, value, nullptr, nullptr, node, node->printFunc);
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

    node->printFunc(logFile, node->type, node->value);

    if (node->right) dumpNode(file, node->right);
    fprintf(file, ")");
}

void graphNode(Node_t *node, FILE *tempFile) {
    fprintf(
                tempFile, 
                "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=red, label=\"{ {val: ",
                node
            );

    node->printFunc(tempFile, node->type, node->value);
    
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

    system("dot -Tsvg temp.dot > graph.png && xdg-open graph.png");
}

#if _DEBUG
void closeLogfile(void) {
    if (logFile) fclose(logFile);
}
#endif