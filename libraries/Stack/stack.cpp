#include "stack.h"

#if CANARY_PROTECT
    void canaryData(Elem_t **data, size_t capacity, int *err) {
        if (err) *err = OK;
        if (!data && err) *err = DATA_NULL;
        if (!*data && err) *err = DATA_NULL;

        char *temp = (char *) *data;
        *data = (Elem_t*) memcpy(temp, &CANARY_CONSTANT, sizeof(CANARY_CONSTANT));
        *data = (Elem_t*) (temp + sizeof(CANARY_CONSTANT));

        temp += capacity * sizeof(Elem_t) + sizeof(CANARY_CONSTANT);
        memcpy(temp, &CANARY_CONSTANT, sizeof(CANARY_CONSTANT));
    }
#endif

#if HASH_PROTECT
    void updateHashes(Stack_t *stack, int *err) {
        if (!stack && err) *err = STACK_NULL;
        if (!stack->data && err) *err = DATA_NULL;

        stack->stackHash = 0;

        #if CANARY_PROTECT
            stack->bufferHash = countHash(((char *)stack->data) - sizeof(CANARY_CONSTANT), stack->capacity * sizeof(Elem_t) + 2 * sizeof(CANARY_CONSTANT));
        #else
            stack->bufferHash = countHash(stack->data, stack->capacity * sizeof(Elem_t));
        #endif

        stack->stackHash = countHash(stack, sizeof(Stack_t));
    }
#endif

void _stackCtor(Stack_t *stack, size_t capacity, PrintFunction func, Elem_t poisonValue, int *err) {
    if (err) *err = OK;
    if (!stack && err) {
        *err = STACK_NULL;
        return;
    }
    if (capacity < 1 && err) {
        *err = INVALID_CAPACITY;
        return;
    }

    #if _DEBUG
        stack->debug.printFunc = func;
    #endif

    #if CANARY_PROTECT
        stack->data = (Elem_t *) calloc(1, sizeof(Elem_t) * capacity + 2 * sizeof(CANARY_CONSTANT));
    #else
        stack->data = (Elem_t *) calloc(1, sizeof(Elem_t) * capacity);
    #endif

    stack->poisonValue = poisonValue;
    stack->size = 0;
    stack->capacity = capacity;

    if (!stack->data) {
        if (err) *err = UNABLE_TO_ALLOCATE_MEMORY;
    } else {
        #if CANARY_PROTECT
        canaryData(&stack->data, capacity, err);
        #endif
    }

    #if HASH_PROTECT
    updateHashes(stack, err);
    #endif

    ASSERT_OK(stack);
}


void stackPush(Stack_t *stack, Elem_t elem, int *err) {
    if (err) *err = OK;
    if (!stack && err) {
        *err = STACK_NULL;
        return;
    }

    ASSERT_OK(stack);

    stack->data[stack->size] = elem;
    stack->size++;

    #if HASH_PROTECT
    updateHashes(stack, err);
    #endif

    if (stack->size >= stack->capacity - 1) {
        size_t coef = (size_t)ceil((double)stack->size * RESIZE_COEFFICIENT);
        stackResize(stack, coef, err);
    }
}

Elem_t stackPop(Stack_t *stack, int *err) {
    if (err) *err = OK;
    if (!stack && err) {
        *err = STACK_NULL;
        return DEFAULT_POISON_VALUE;
    }
    if (stack->size == 0) {
        if (err) *err = STACK_EMPTY;
        return stack->poisonValue;
    }
    ASSERT_OK(stack);

    stack->size--;
    Elem_t value = stack->data[stack->size];
    stack->data[stack->size] = stack->poisonValue;

    size_t toLower = (size_t)(floor((double)stack->capacity / (RESIZE_COEFFICIENT * RESIZE_COEFFICIENT)));

    #if HASH_PROTECT
    updateHashes(stack, err);
    #endif

    if (stack->size < toLower) {
        stackResize(stack, (size_t)floor((double)stack->capacity / RESIZE_COEFFICIENT), err);
    }

    return value;
}

void stackResize(Stack_t *stack, size_t size, int *err) {
    if (err) *err = OK;
    if (!stack && err) {
        *err = STACK_NULL;
        return;
    }
    ASSERT_OK(stack);

    #if CANARY_PROTECT
        char *temp = (char *) stack->data;
        stack->data = (Elem_t *) (temp - sizeof(CANARY_CONSTANT));
        stack->data = (Elem_t *) recalloc(stack->data, 1, (size + 1) * sizeof(Elem_t) + 2 * sizeof(CANARY_CONSTANT), stack->size);
    #else
        stack->data = (Elem_t *) recalloc(stack->data, 1, (size + 1) * sizeof(Elem_t), stack->size);
    #endif


    stack->capacity = size + 1;

    #if CANARY_PROTECT
    canaryData(&stack->data, stack->capacity, err);
    #endif

    if (!stack->data) *err = UNABLE_TO_ALLOCATE_MEMORY;

    #if HASH_PROTECT
    updateHashes(stack, err);
    #endif
    
    ASSERT_OK(stack);
}

void stackShrinkToFit(Stack_t *stack, int *err) {
    stackResize(stack, stack->size, err);
}

void stackDtor(Stack_t *stack, int *err) {
    if (err) *err = OK;
    if (!stack && err) {
        *err = STACK_NULL;
        return;
    }
    ASSERT_OK(stack);

    if (stack->data) {
        #if CANARY_PROTECT
            stack->data = (Elem_t *)(((char *)stack->data) - sizeof(CANARY_CONSTANT));
        #endif

        free(stack->data);
        stack->data = nullptr;
    }

    stack->size = 0;
    stack->capacity = 0;

    #if _DEBUG
        stack->debug.createFunc = nullptr;
        stack->debug.createFile = nullptr;
        stack->debug.name = nullptr;
        stack->debug.createLine = 0;
        stack->debug.printFunc = nullptr;
    #endif

    #if HASH_PROTECT
        stack->stackHash = 0;
        stack->bufferHash = 0;
    #endif
}

void *recalloc(void *ptr, size_t amount, size_t elemSize, size_t currentAmount, int *err) {
    if (err) *err = OK;
    if (!ptr) {
        return calloc(amount, elemSize);
    }

    char *newPtr = (char *) realloc(ptr, amount * elemSize);
    if (!newPtr) {
        if (err) {
            *err = UNABLE_TO_ALLOCATE_MEMORY;
        }

        return ptr;
    }

    if (currentAmount < amount) {
        for (size_t i = currentAmount; i < amount; i++) {
            newPtr[i * elemSize] = 0;
        }
    }

    ptr = newPtr;

    return ptr;
}