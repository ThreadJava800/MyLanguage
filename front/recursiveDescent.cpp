#include "../lang.h"

// G    = E nullptr
// E    = T{['=']T}*
// T    = CP{['*' | '/']CP}*
// CP   = X{[compare operators]X}*
// P    = '(' E ')' | X
// X    = [numbers and words] | AS
// AS   = P{['=']P}*

Node_t* setOper(Node_t* val1, Node_t* val2, Node_t* oper) {
    L(oper) = val1;
    R(oper) = val2;
    return oper;
}

void addPrevs(Node_t* start) {
    if (!start) return;

    if (L(start)) {
        PREV(L(start)) = start;
    }
    if (R(start)) {
        PREV(R(start)) = start;
    }

    if (L(start)) addPrevs(L(start));
    if (R(start)) addPrevs(R(start));
}

Node_t* head(Node_t* cur) {
    ON_ERROR(!cur, "Node is null", nullptr);

    if (PREV(cur)) return head(PREV(cur));

    return cur;
}

Node_t* makeConnections(Node_t* info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* headNode = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    while (info) {
        L(headNode) = getE(&info);
        addPrevs(L(headNode));
        PREV(L(headNode)) = headNode;
        R(headNode) = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, headNode);
        headNode = R(headNode);
        // TODO: check for ;
        info = R(info);
     }
    headNode = head(headNode);

    return headNode;
}

Node_t* getE(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getT(info);
    if (!(whatOper(*info, ASSIGN_OP))) return val1;

    Node_t* operNode = nodeCopy(*info);
    R(operNode) = PREV(operNode) = nullptr;
    *info = R(*info);
    Node_t* val2 = getT(info);

    // val2 = head(val2);
    // graphDump(val1);

    return setOper(val1, val2, operNode);
}

Node_t* getT(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getX(info);
    // graphDump(val1);
    // printf("%d\n", (*info)->value.opt);
    if (!(whatOper(*info, MUL_OP) || whatOper(*info, DIV_OP))) return val1;

    while (whatOper(*info, MUL_OP) || whatOper(*info, DIV_OP)) {
        Node_t* operNode = nodeCopy(*info);
        R(operNode) = PREV(operNode) = nullptr;
        *info = R(*info);
        Node_t* val2 = getX(info);

        val1 = setOper(val1, val2, operNode);
    }

    return val1;
}

Node_t* getCP(Node_t** info) {

}

Node_t* getAS(Node_t** info) {

}

Node_t* getX(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (IS_NUM_VAR(*info)) {
        Node_t* retData = nodeCopy(*info);
        *info = R(*info);
        R(retData) = PREV(retData) = nullptr;
        return retData;
    }
    return nullptr;
}