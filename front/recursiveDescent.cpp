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

    Node_t* res = getG(&info);

    return res;
}

Node_t* getG(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* headNode = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    while (*info) {
        L(headNode) = getVar(info);
        addPrevs(L(headNode));
        PREV(L(headNode)) = headNode;

        R(headNode) = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, headNode);
        headNode = R(headNode);

        SYNTAX_ERROR(!whatOper(*info, END_LINE_OP), "missing ;!");
        *info = R(*info);
    }

    return head(headNode);
}

Node_t* getVar(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (!(IS_VAR(*info))) return getDef(info);

    Node_t* varNode = nodeCtor(VAR, {.num = (*info)->value.num}, nullptr, nullptr, nullptr);
    *info = R(*info);
    Node_t* assNode = getDef(info);

    return nodeCtor(FICTITIOUS, {}, varNode, assNode, nullptr);
}

Node_t* getDef(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (!(IS_DEF(*info))) return getE(info);

    int funIndex = (*info)->value.num;
    Node_t* varNode = parseVars(info);
    *info = R(*info);

    Node_t* assNode = getDoNode(info);

    return nodeCtor(DEF, {.num = funIndex}, varNode, assNode, nullptr);
}

Node_t* parseVars(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    *info = R(*info);
    SYNTAX_ERROR(!(IS_O_CR_BR(*info)), "Params provided incorrectly");
    *info = R(*info);

    Node_t* returnNode = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    Node_t* start = returnNode;
    while (!(IS_C_CR_BR(*info))) {
        R(returnNode) = nodeCopy(*info);
        R(R(returnNode)) = nullptr;
        L(returnNode) = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
        returnNode = L(returnNode);
        *info = R(*info);
    }
    addPrevs(start);

    return head(start);
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

    if (IS_NUM_VAR(*info) || IS_IN(*info)) {
        Node_t* retData = nodeCopy(*info);
        *info = R(*info);
        R(retData) = PREV(retData) = nullptr;
        return retData;
    }
    if (IS_IF(*info))    return getIF(info);
    if (IS_OUT(*info))   return getOut(info);
    if (IS_WHILE(*info)) return getWhile(info);
    if (IS_RET(*info))   return getRet(info);
    if (IS_CALL(*info))  return getCall(info);
    if (IS_SQRT(*info))  {
        *info = R(*info);
        return nodeCtor(OPERATOR, {.opt = SQRT_OP}, getX(info), nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr), nullptr);
    }
    if (IS_COS(*info)) return getCos(info);

    return nullptr;
}

Node_t* getIF(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    *info = R(*info);
    SYNTAX_ERROR(!(IS_O_CR_BR(*info)), "Missing '(' bracket!");
    *info = R(*info);

    Node_t* caseNode = getE(info);
    SYNTAX_ERROR(!(IS_C_CR_BR(*info)), "Missing ')' bracket!");
    *info = R(*info);

    Node_t* trueNode = getDoNode(info);
    Node_t* falseNode = getElse(info);
    Node_t* if2 = nodeCtor(IF2, {}, trueNode, falseNode, nullptr);
    addPrevs(if2);

    return nodeCtor(IF, {}, caseNode, if2, nullptr);
}

Node_t* getElse(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    if (!(IS_ELSE(*info))) return nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);

    *info = R(*info);
    return getDoNode(info);
}

Node_t* getDoNode(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    SYNTAX_ERROR(!(IS_O_FIG_BR(*info)), "Missing '{' bracket!");
    *info = R(*info);

    Node_t* toDoNode = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    while (!(IS_C_FIG_BR(*info))) {
        L(toDoNode) = getVar(info);
        addPrevs(L(toDoNode));
        PREV(L(toDoNode)) = toDoNode;

        R(toDoNode) = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, toDoNode);
        toDoNode = R(toDoNode);

        SYNTAX_ERROR(!whatOper(*info, END_LINE_OP), "missing ;!");
        *info = R(*info);
    }
    SYNTAX_ERROR(!(IS_C_FIG_BR(*info)), "Missing '}' bracket!");
    *info = R(*info);

    return head(toDoNode);
}

Node_t* getOut(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    *info = R(*info);
    Node_t* toPrintNode = getX(info);
    SYNTAX_ERROR(!toPrintNode, "Out command is used incorrectly!");

    Node_t* retNode  =  nodeCtor(OPERATOR, {.num = OUT_OP}, toPrintNode, 
                                            nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr), nullptr);
    addPrevs(retNode);
    return head(retNode);
}

Node_t* getWhile(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    *info = R(*info);
    SYNTAX_ERROR(!(IS_O_CR_BR(*info)), "Missing '(' bracket!");
    *info = R(*info);

    Node_t* caseNode = getE(info);
    SYNTAX_ERROR(!(IS_C_CR_BR(*info)), "Missing ')' bracket!");
    *info = R(*info);

    Node_t* toDoNode = getDoNode(info);

    return nodeCtor(WHILE, {}, caseNode, toDoNode, nullptr);
}

Node_t* getRet(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    *info = R(*info);
    Node_t* returnNode = getX(info);
    Node_t* fict = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    return nodeCtor(RETURN, {}, returnNode, fict, nullptr);
}

Node_t* getCall(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    int index = (*info)->value.num;
    Node_t* varNode = parseVars(info);
    *info = R(*info);
    return nodeCtor(CALL, {.num = index}, L(varNode), R(varNode), nullptr);
}

Node_t* getCos(Node_t** info) {
    ON_ERROR(!info, "Node is null", nullptr);

    Node_t* cosNode = nodeCopy(*info);
    *info = R(*info);
    Node_t* fict = nodeCtor(FICTITIOUS, {}, nullptr, nullptr, nullptr);
    return setOper(getX(info), fict, cosNode);
}