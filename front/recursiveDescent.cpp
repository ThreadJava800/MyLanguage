#include "../lang.h"

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

        SYNTAX_ERROR(!whatOper(info, END_LINE_OP), "missing ;!");
        info = R(info);
    }

    return head(headNode);;
}

Node_t* getE(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getT(info);
    if (!(whatOper(*info, ASSIGN_OP))) return val1;

    Node_t* operNode = nodeCopy(*info);
    R(operNode) = PREV(operNode) = nullptr;
    *info = R(*info);
    Node_t* val2 = getT(info);

    return setOper(val1, val2, operNode);
}

Node_t* getT(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getM(info);
    if (!(whatOper(*info, ADD_OP) || whatOper(*info, SUB_OP))) return val1;

    while (whatOper(*info, ADD_OP) || whatOper(*info, SUB_OP)) {
        Node_t* operNode = nodeCopy(*info);
        R(operNode) = PREV(operNode) = nullptr;
        *info = R(*info);
        Node_t* val2 = getM(info);

        val1 = setOper(val1, val2, operNode);
    }

    return val1;
}

Node_t* getM(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getCP(info);
    if (!(whatOper(*info, MUL_OP) || whatOper(*info, DIV_OP))) return val1;

    while (whatOper(*info, MUL_OP) || whatOper(*info, DIV_OP)) {
        Node_t* operNode = nodeCopy(*info);
        R(operNode) = PREV(operNode) = nullptr;
        *info = R(*info);
        Node_t* val2 = getCP(info);

        val1 = setOper(val1, val2, operNode);
    }

    return val1;
}

Node_t* getCP(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* val1 = getX(info);
    if (!(IS_CMP_OPS(*info))) return val1;

    while (IS_CMP_OPS(*info)) {
        Node_t* operNode = nodeCopy(*info);
        R(operNode) = PREV(operNode) = nullptr;
        *info = R(*info);
        Node_t* val2 = getX(info);

        val1 = setOper(val1, val2, operNode);
    }

    return val1;
}

Node_t* getX(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (IS_NUM_VAR(*info)) {
        Node_t* retData = nodeCopy(*info);
        *info = R(*info);
        R(retData) = PREV(retData) = nullptr;
        return retData;
    }

    ON_ERROR(true, "crytical compiler error", nullptr);
}