#include "../lang.h"

int localAreasCount = 0;
bool isLocal = false;
int functionId = -1;

Node_t* readLangFile(const char* fileName, List_t* vars, List_t* funcs, List_t* fParams) {
    ON_ERROR(!fileName, "File name was null", nullptr);
    ON_ERROR(!vars, "Var list is null", nullptr);
    ON_ERROR(!funcs, "Func list is null", nullptr);
    ON_ERROR(!fParams, "Param list is null", nullptr);

    FILE* file = fopen(fileName, "rb");
    ON_ERROR(!file, "File not found", nullptr);

    Node_t* entities = parseFile(file, vars, funcs, fParams);
    fclose(file);

    return entities;
}

Node_t* parseFile(FILE* file, List_t* vars, List_t* funcs, List_t* fParams) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!vars, "Var list is null", nullptr);
    ON_ERROR(!funcs, "Func list is null", nullptr);

    Node_t* curNode = nullptr;
    int symb = EOF;

    while ((symb = fgetc(file)) != EOF) {
        ungetc(symb, file);
        SKIP_SPACES();

        curNode = parseOper(file, curNode);

        curNode = parseWord(file, curNode, vars, funcs, fParams);
        SKIP_SPACES();

        curNode = parseNum(file, curNode);
    }

    curNode = headStart(curNode);

    return curNode;
}

Node_t* parseOper(FILE* file, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);

    char oper[MAX_OPER_LENGTH] = "";
    int symbCount = getOper(file, oper);
    if (symbCount) {
        if (!strcmp(oper, "+"))  return nodeCtor(OPERATOR, {.opt = ADD_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "-"))  return nodeCtor(OPERATOR, {.opt = SUB_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "*"))  return nodeCtor(OPERATOR, {.opt = MUL_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "/"))  return nodeCtor(OPERATOR, {.opt = DIV_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "^"))  return nodeCtor(OPERATOR, {.opt = POW_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "="))  return nodeCtor(OPERATOR, {.opt = ASSIGN_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "==")) return nodeCtor(OPERATOR, {.opt = EQU_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, ">"))  return nodeCtor(OPERATOR, {.opt = BIGGER_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "<"))  return nodeCtor(OPERATOR, {.opt = LESS_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, ">=")) return nodeCtor(OPERATOR, {.opt = BIGGER_EQ_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "<=")) return nodeCtor(OPERATOR, {.opt = LESS_EQ_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "!=")) return nodeCtor(OPERATOR, {.opt = NOT_EQ_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "!"))  return nodeCtor(OPERATOR, {.opt = NOT_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "||")) return nodeCtor(OPERATOR, {.opt = OR_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "&&")) return nodeCtor(OPERATOR, {.opt = AND_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "("))  {
            isLocal = true;
            return nodeCtor(OPERATOR, {.opt = O_CIR_BR_OP}, nullptr, nullptr, prev);
        }
        if (!strcmp(oper, ")"))  return nodeCtor(OPERATOR, {.opt = C_CIR_BR_OP}, nullptr, nullptr, prev);
        if (!strcmp(oper, "{"))  {
            isLocal = true;
            return nodeCtor(OPERATOR, {.opt = O_FIG_BR_OP}, nullptr, nullptr, prev);
        }
        if (!strcmp(oper, "}"))  {
            isLocal = false;
            return nodeCtor(OPERATOR, {.opt = C_FIG_BR_OP}, nullptr, nullptr, prev);
        }
        if (!strcmp(oper, ";"))  return nodeCtor(OPERATOR, {.opt = END_LINE_OP}, nullptr, nullptr, prev);
    }

    return prev;
}

int getOper(FILE* file, char* buffer) {
    ON_ERROR(!file, "File is null", 0);

    int symb = fgetc(file);
    int symbCount = 0;
    while (isOperInList(symb)) {
        *buffer = (char) symb;
        symbCount++;
        buffer++;

        symb = fgetc(file);
    }
    ungetc(symb, file);

    return symbCount;
}

bool isOperInList(int symb) {
    for (size_t i = 0; i < sizeof(baseOpers) / sizeof(char); i++) {
        if (baseOpers[i] == symb) return true;
    }
    return false;
}

Node_t* parseNum(FILE* file, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);

    int symb = fgetc(file);
    int value = 0;
    bool isNeg = false, isValue = false;

    if (symb == '-') {
        symb = fgetc(file);
        isNeg = true;
    }
    while (isdigit(symb)) {
        isValue = true;
        value = value * 10 + (symb - '0');
        symb = fgetc(file);
    }
    if (isNeg) value *= -1;
    if (isNeg && !isValue) {
        ungetc(symb, file);
        ungetc('-', file);
    }
    else ungetc(symb, file);

    if (isValue) return nodeCtor(NUMBER, {.num = value}, nullptr, nullptr, prev);

    return prev;
}

Node_t* parseWord(FILE* file, Node_t* prev, List_t* vars, List_t* funcs, List_t* fParams) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!vars || !funcs, "List is null", nullptr);

    char command[MAX_WORD_LENGTH] = "";
    bool isFunc = false;
    int symbCount = getWord(file, command, &isFunc);
    if (symbCount) {
        if(!strcmp(command, ifCom))    return nodeCtor(IF, {}, nullptr, nullptr, prev);
        if(!strcmp(command, whileCom)) return nodeCtor(WHILE, {}, nullptr, nullptr, prev);
        if(!strcmp(command, elseCom))  return nodeCtor(ELSE, {}, nullptr, nullptr, prev);
        if(!strcmp(command, varCom))   return newVar(file, vars, prev);
        if(!strcmp(command, sqrtCom))  return nodeCtor(OPERATOR, {.opt = SQRT_OP}, nullptr, nullptr, prev);
        if(!strcmp(command, defCom))   {
            localAreasCount++;
            return newDef(file, vars, funcs, fParams, prev);
        }
        if(!strcmp(command, assCom))   return nodeCtor(OPERATOR, {.opt = ASSIGN_OP}, nullptr, nullptr, prev);
        if(!strcmp(command, outCom))   return nodeCtor(OPERATOR, {.opt = OUT_OP}, nullptr, nullptr, prev);
        if(!strcmp(command, inCom))    return nodeCtor(OPERATOR, {.opt = IN_OP}, nullptr, nullptr, prev);
        if(!strcmp(command, retCom))   {
            return nodeCtor(RETURN, {.num = functionId}, nullptr, nullptr, prev);
        }
        if(!strcmp(command, callCom))  return newCall(file, funcs, vars, prev);
        if(!strcmp(command, equCom))   return nodeCtor(OPERATOR, {.opt = EQU_OP}, nullptr, nullptr, prev);
        if(!strcmp(command, cosCom))   return nodeCtor(OPERATOR, {.opt = COS_OP}, nullptr, nullptr, prev);

        return checkVariable(strdup(command), vars, funcs, prev, isFunc);
    }

    return prev;
}

Node_t* newVar(FILE* file, List_t* vars, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!vars, "List is null", nullptr);

    SKIP_SPACES();

    char command[MAX_WORD_LENGTH] = "";
    int symbCount = getWord(file, command, nullptr);
    SYNTAX_ERROR(!symbCount, "Need variable name after var declaration!");
    char* varCpy = strdup(command);
    if (isLocal) {
        int moves = 0;
        moves += sprintf(command, "%d_", localAreasCount);
        moves += sprintf(command + moves, "%s", varCpy);
    }
    listPushBack(vars, strdup(command));

    Node_t* rightNode  = nodeCtor(VARIABLE, {.num = (int) vars->size - 1}, nullptr, nullptr, nullptr);
    Node_t* returnNode = nodeCtor(VAR, {.num = (int) vars->size - 1}, nullptr, rightNode, prev);
    PREV(rightNode) = returnNode;
    
    return rightNode;
}

Node_t* newDef(FILE* file, List_t* vars, List_t* funcs, List_t* fParams, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!funcs || !vars || !fParams, "List is null", nullptr);

    SKIP_SPACES();

    char name[MAX_WORD_LENGTH] = "";
    int symbCount = getWord(file, name, nullptr);
    SYNTAX_ERROR(!symbCount, "Need func name after its declaration!");
    listPushBack(funcs, strdup(name));
    functionId = (int) funcs->size - 1;

    char params[MAX_WORD_LENGTH] = "";
    Node_t* defNode = nodeCtor(DEF, {.num = (int) funcs->size - 1}, nullptr, nullptr, prev);
    Node_t* paramNode = nodeCtor(OPERATOR, {.opt = O_CIR_BR_OP}, nullptr, nullptr, defNode);
    symbCount = getFuncParams(file, params, vars, &paramNode);
    listPushBack(fParams, strdup(params));

    R(paramNode) = nodeCtor(OPERATOR, {.opt = C_CIR_BR_OP}, nullptr, nullptr, paramNode);
    
    return R(paramNode);
}

int getFuncParams(FILE* file, char* buffer, List_t* vars, Node_t** node) {
    ON_ERROR(!file, "File is null", 0);
    ON_ERROR(!buffer, "Buffer null", 0);
    ON_ERROR(!node, "Node is null", 0);
    ON_ERROR(!vars, "List is null", 0);

    int symb = fgetc(file);
    SYNTAX_ERROR(symb != '(', "Func params must be in brackets!");
    fgetc(file);

    char* args[MAX_ARG_COUNT] = {};

    bool isFunc = false;
    int paramCount = 0;
    while (!isFunc) {
        char paramName[MAX_WORD_LENGTH] = "";
        int symbCount = getWord(file, paramName, &isFunc);
        if (!symbCount) break;

        char toPush[MAX_WORD_LENGTH] = "";
        int readNums = sprintf(toPush, "%d_", localAreasCount);
        sprintf(&(toPush[readNums]), "%s", paramName);
        listPushBack(vars, strdup(toPush));
        R(*node) = nodeCtor(VARIABLE, {.num = (int) vars->size - 1}, nullptr, nullptr, *node);
        *node = R(*node);

        args[paramCount] = strdup(paramName);
        paramCount++;
    }

    buffer += sprintf(buffer, "%d ", paramCount);
    for (int i = 0; i < paramCount; i++) {
        buffer += sprintf(buffer, "%s ", args[i]);
    }

    return paramCount;
}

Node_t* newCall(FILE* file, List_t* funcs, List_t* vars, Node_t* prev) {
    ON_ERROR(!file, "File is null", nullptr);
    ON_ERROR(!funcs, "List is null", nullptr);

    bool isFunc = false;
    char command[MAX_WORD_LENGTH] = "";
    int symbCount = getWord(file, command, &isFunc);
    SYNTAX_ERROR(!symbCount, "Need func name after its call!");

    Node_t* node = checkVariable(strdup(command), funcs, funcs, prev, true);
    R(node) = nodeCtor(OPERATOR, {.num = O_CIR_BR_OP}, nullptr, nullptr, node);
    node = R(node);
    parseCallArgs(file, vars, funcs, &node);
    R(node) = nodeCtor(OPERATOR, {.num = C_CIR_BR_OP}, nullptr, nullptr, node);

    SYNTAX_ERROR(!isFunc, "Func name are same with var!: %s", command);
    return R(node);
}

void parseCallArgs(FILE* file, List_t* vars, List_t* funcs, Node_t** node) {
    ON_ERROR(!file, "File is null", );
    ON_ERROR(!node, "Node is null", );

    int symb = fgetc(file);
    SYNTAX_ERROR(symb != '(', "Func params must be in brackets!");
    fgetc(file);

    bool isFunc = false;
    int paramCount = 0;
    while (!isFunc) {
        char paramName[MAX_WORD_LENGTH] = "";
        int symbCount = getWord(file, paramName, &isFunc);
        if (!symbCount) break;

        R(*node) = checkVariable(paramName, vars, funcs, *node, false);
        *node = R(*node);

        paramCount++;
    }
}

Node_t* checkVariable(char* varName, List_t* vars, List_t* funcs, Node_t* prev, bool isFunc) {
    ON_ERROR(!varName, "Nullable name", nullptr);
    ON_ERROR(!vars || !funcs, "List is null", nullptr);

    bool isMet = false;
    int varIndex = -1;

    if (isFunc) {
        for (int i = 0; i < funcs->size; i++) {
            ListElement_t* value = logicToPhysics(funcs, i);
            if (!strcmp(value->value, varName)) {
                varIndex = i;
                isMet = true;
                break;
            }
        }
        return nodeCtor(CALL, {.num = varIndex}, nullptr, nullptr, prev);
    }

    if (isLocal) {
        char* varCopy = strdup(varName);
        int moves = 0;
        moves += sprintf(varName, "%d_", localAreasCount);
        moves += sprintf((varName + moves), "%s", varCopy);
    }

    for (int i = 0; i < vars->size; i++) {
        ListElement_t* value = logicToPhysics(vars, i);
        if (!strcmp(value->value, varName)) {
            varIndex = i;
            isMet = true;
            break;
        }
    }

    if (!isMet) {
        char* global = varName + 2;
        for (int i = 0; i < vars->size; i++) {
            ListElement_t* value = logicToPhysics(vars, i);
            if (!strcmp(value->value, global)) {
                varIndex = i;
                isMet = true;
                break;
            }
        }
    }

    SYNTAX_ERROR(!isMet, "Unknown variable name: %s", varName);

    return nodeCtor(VARIABLE, {.num = varIndex}, nullptr, nullptr, prev);
}

int getWord(FILE* file, char* buffer, bool* isFunc) {
    ON_ERROR(!file, "File is null", 0);
    ON_ERROR(!buffer, "Buffer is null", 0);

    int symb = fgetc(file);
    if (symb == ')') return 0;
    if (isdigit(symb)) {
        int nextSymb = fgetc(file);
        SYNTAX_ERROR((isalpha(nextSymb) || nextSymb == '_'), "Incorrect var name! %c", symb);

        ungetc(nextSymb, file);
        ungetc(symb, file);
        return 0;
    }

    int symbCount = 0;

    while (!isspace(symb) && symb != EOF && symb != '(' && symb != ')' && symb != ';') {
        SYNTAX_ERROR(!(isalnum(symb) || symb == '_'), "Incorrect var name! %c", symb);

        *buffer = (char) symb;
        buffer++;
        symbCount++;

        symb = fgetc(file);
    }
    ungetc(symb, file);
    SKIP_SPACES();
    if (isFunc) *isFunc = (symb == '(' || symb == ')');

    return symbCount;
}

Node_t* headStart(Node_t* end) {
    ON_ERROR(!end, "Node is null", nullptr);

    if (!end->prev) return end;

    end->prev->right = end;
    return headStart(end->prev);
}