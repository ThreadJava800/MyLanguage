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
    Node_t* fictNode = nodeCtor(FICTITIOUS, {}, oper, nullptr, nullptr);

    return fictNode;
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

Node_t* makeConnections(Node_t* info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* res = getE(&info);
    addPrevs(res);

    return res;
}

Node_t* getE(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (whatOper(*info, ASSIGN_OP)) {
        return setOper(getT(&PREV(*info)), getT(&R(*info)), *info);
    }
    return getT(info);
}

Node_t* getT(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (whatOper(*info, MUL_OP) || whatOper(*info, DIV_OP)) {
        return setOper(getX(&PREV(*info)), getX(&R(*info)), *info);
    }
    return getX(info);
}

Node_t* getCP(Node_t** info) {

}

Node_t* getAS(Node_t** info) {

}

Node_t* getX(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (IS_NUM_VAR(*info)) {
        Node_t* infoCopy = nodeCopy(*info);
        infoCopy->right = nullptr;
        *info = (*info)->right;
        return infoCopy;
    }
    return nullptr;
}