#define POP stackPop(&cpu->stack, &errorCode)


#define JUMP_HELPER(sign)   \
    if (POP sign POP) {      \
        jmp(cpu);             \
    } else {                   \
        cpu->ip += sizeof(int); \
    }                            \


DEF_CMD(HLT, 0, {
    exit(0);
}, 0)

DEF_CMD(PUSH, 1, {
    errorCode = push(cpu, args);
}, 1)

DEF_CMD(ADD, 2, {
    errorCode = add(&cpu->stack);
}, 0)

DEF_CMD(DIV, 3, {
    div(&cpu->stack);
}, 0)

DEF_CMD(OUT, 4, {
    errorCode = out(&cpu->stack);
}, 0)

DEF_CMD(IN, 5, {
    errorCode = in(&cpu->stack);
}, 0)

DEF_CMD(MUL, 6, {
    errorCode = mul(&cpu->stack);
}, 0)

DEF_CMD(SUB, 7, {
    errorCode = sub(&cpu->stack);
}, 0)

DEF_CMD(DUMP, 8, {
    DUMP_CPU(cpu, errorCode);
}, 0)

DEF_CMD(POP, 9, {
    errorCode = pop(cpu, args);
}, 1)

DEF_CMD(JMP, 10, {
    errorCode = jmp(cpu);
}, 2)

DEF_CMD(CALL, 11, {
    stackPush(&cpu->callStack, (size_t) cpu->ip + sizeof(char) + sizeof(int), &errorCode);
    errorCode = jmp(cpu);
}, 2)

DEF_CMD(RET, 12, {
    cpu->ip = stackPop(&cpu->callStack, &errorCode) - 1;
}, 0)

DEF_CMD(JB, 13, {
    JUMP_HELPER(<)
}, 2)

DEF_CMD(JA, 14, {
    JUMP_HELPER(>)
}, 2)

DEF_CMD(JAE, 15, {
    JUMP_HELPER(>=)
}, 2)

DEF_CMD(JBE, 16, {
    JUMP_HELPER(<=)
}, 2)

DEF_CMD(JE, 17, {
    JUMP_HELPER(==)
}, 2)

DEF_CMD(JNE, 18, {
    JUMP_HELPER(!=)
}, 2)

DEF_CMD(SQRT, 19, {
    double val = sqrt(POP);
    stackPush(&cpu->stack, floor(val) / 100, &errorCode);
}, 0)

DEF_CMD(PRINF, 20, {
    fprintf(stderr, "INFINITE ROOTS\n");
}, 0)

DEF_CMD(PRNO, 21, {
    fprintf(stderr, "NO ROOTS\n");
}, 0)

DEF_CMD(JMPMND, 22, {
    tm *calendar; // календарьььь с удобным временем и датами
    time_t timestamp = time(NULL);

    calendar = localtime(&timestamp);
    if (calendar->tm_wday == 1) {
        jmp(cpu);
    }
}, 2)

DEF_CMD(COS, 23, {
    double val = cos(POP);
    stackPush(&cpu->stack, floor(val * 100), &errorCode);
}, 0)
