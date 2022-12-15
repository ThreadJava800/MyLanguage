#ifndef SECURED_STACK_H
#define SECURED_STACK_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#ifndef HASH_PROTECT
#define HASH_PROTECT 0
#endif

#ifndef CANARY_PROTECT
#define CANARY_PROTECT 0
#endif

#ifndef _DEBUG
#define _DEBUG 0
#endif

typedef int Elem_t;

/**
 *
 * Annotation of function to print elements in stack
 *
 **/
typedef void (*PrintFunction)(FILE *file, Elem_t value);

/**
 *
 * I use this value to replace old values with it
 *
 **/
const Elem_t DEFAULT_POISON_VALUE = 2147483646;

/**
 *
 * I use this coefficient to resize stack when needed.
 * I used this link while searching info:
 * http://artem.sobolev.name/posts/2013-02-10-std-vector-growth.html
 *
 **/
const double RESIZE_COEFFICIENT = 1.61;

#if CANARY_PROTECT
/**
 *
 * I use this constant for CANARY protect, i put it to the beginning of array and struct
 *
 **/
const size_t CANARY_CONSTANT = 8350650019957897075; // 0x1ADEB1L
#endif

/**
 *
 * This function checks if stack OK and DUMPS stack values on error (if _DEBUG)
 *
 *
 * @param stack - Stack_t structure with all needed staff for stack
 * @param printEl - function to print element in array
 **/
#if _DEBUG

    #define ASSERT_OK(stack) {\
        int errorCode = verifyStack(stack);\
        \
        if (errorCode) {\
            FILE *file = fopen("log.txt", "a");\
            \
            if (errorCode == SIZE_BIGGER_CAPACITY) exit(0);\
            \
            fclose(file);\
        }\
    }\

#else

    #define ASSERT_OK(stack) {\
        int errorCode = verifyStack(stack);\
        \
        if (errorCode) {\
            if (errorCode == SIZE_BIGGER_CAPACITY) exit(0);\
        }\
    }\

#endif

#define getType()

#if _DEBUG
/**
 *
 * Contains all neccessary info for debug
 *
 * @param createFunc - name of function where stack was created
 * @param createFile - name of file where stack was created
 * @param name - name of stack variable
 * @param createLine - number of line where stack was created in @createFunc
 * @param printFunc - pointer to function that prints element
 **/
struct StackDebug {
    const char *createFunc = nullptr; 
    const char *createFile = nullptr; 
    const char *name = nullptr;
    int createLine = 0;
    PrintFunction printFunc = nullptr;
};
#endif

/**
 *
 * Stack itself
 *
 * @param stackCanary1 - canary constant in the beginning
 * @param data - stack values (array of them)
 * @param size - amount of values in stack (and pointer to nearest free place)
 * @param capacity - all of capacity available to data
 * @param poisonValue - poison value for Elem_t type, may be provided with constructor
 * @param debug - structure for debug
 * @param capacity - all of capacity available to data
 * @param stackHash - hash value of stack
 * @param bufferHash - hash value of data
 * @param stackCanary2 - canary constant in the end
**/
struct Stack_t {
    #if CANARY_PROTECT
    size_t stackCanary1 = CANARY_CONSTANT;
    #endif

    Elem_t *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
    Elem_t poisonValue = DEFAULT_POISON_VALUE; 

    #if _DEBUG
    StackDebug debug = {};
    #endif

    #if HASH_PROTECT
    size_t stackHash = 0;
    size_t bufferHash = 0;
    #endif

    #if CANARY_PROTECT
    size_t stackCanary2 = CANARY_CONSTANT;
    #endif
};

/**
 *
 * Error codes returned from functions
 *
**/
enum ErrorCodes {
    OK                          =  0,
    UNABLE_TO_ALLOCATE_MEMORY   = -1,
    DATA_NULL                   = -2,
    INVALID_CAPACITY            = -3,
    SIZE_BIGGER_CAPACITY        = -4,
    STACK_NULL                  = -5,
    INVALID_SIZE                = -6,
    STACK_EMPTY                 = -7,
    STACK_CANARY_BROKEN         = -8,
    BUFFER_CANARY_BROKEN        = -9,
    BUFFER_HASH_CHANGED         = -10,
    STACK_HASH_CHANGED          = -11,
};

/**
 *
 * Ads canaries to end and beginning of array data
 *
 * @param data - elements
 * @param capacity - capacity of array
 * @param err - pointer to int where error code is saved
 **/
void canaryData(Elem_t **data, size_t capacity, int *err = nullptr);

/**
 *
 * Updates hashes for stack and data
 *
 * @param stack - pointer to stack to hash
 * @param err - pointer to int where error code is saved
 **/
void updateHashes(Stack_t *stack, int *err = nullptr);

/**
 *
 * Prints element according to its type
 *
 * @param file - pointer to file
 * @param value - value itself
 **/
void printElemT(FILE *file, Elem_t value);

/**
 *
 * Pushes value of Elem_t type to stack
 *
 * @param stack - pointer to stack where to push element
 * @param capacity - wanted capacity of stack
 * @param err - pointer to int where error code is saved
 **/
void _stackCtor(Stack_t *stack, size_t capacity, PrintFunction func = printElemT, Elem_t poisonValue = DEFAULT_POISON_VALUE, int *err = nullptr);

/**
 *
 * Interlayer between main and _stackCtor, used for debug to get variable name
 *
 **/
#if _DEBUG

    #define stackCtor(stack, ...) {\
        if (stack) {\
            (stack)->debug.createFunc = __PRETTY_FUNCTION__;\
            (stack)->debug.createFile = __FILE__;\
            (stack)->debug.createLine = __LINE__;\
            (stack)->debug.name = #stack;\
        }\
        _stackCtor(stack, ##__VA_ARGS__);\
    }\

#else 

    #define stackCtor(stack, ...) {\
        _stackCtor(stack, ##__VA_ARGS__);\
    }\

#endif

/**
 *
 * Pushes value of Elem_t type to stack
 *
 * @param stack - pointer to stack where to push element
 * @param elem - value to push
 * @param err - pointer to int where error code is saved
 **/
void stackPush(Stack_t *stack, Elem_t elem, int *err = nullptr);

/**
 *
 * Takes the last value of stack and returns it
 *
 * @param stack - pointer to stack where to pop element
 * @param err - pointer to int where error code is saved
 *
 * @return Elem_t - value 
 **/
Elem_t stackPop(Stack_t *stack, int *err = nullptr);

/**
 *
 * Resizes stack
 *
 * @param stack - pointer to stack to resize
 * @param size - new size of stack
 * @param err - pointer to int where error code is saved
 **/
void stackResize(Stack_t *stack, size_t size, int *err = nullptr);

/**
 *
 * Resizes stack to its actual size
 *
 * @param stack - pointer to stack to resize
 * @param err - pointer to int where error code is saved
 **/
void stackShrinkToFit(Stack_t *stack, int *err = nullptr);

/**
 *
 * Kills stack: frees all allocated memory, nulls all arguments
 *
 * @param stack - pointer to stack to kill
 * @param err - pointer to int where error code is saved
 **/
void stackDtor(Stack_t *stack, int *err = nullptr);

/**
 *
 * Checks if canaries in array are not broken
 *
 * @param data - array to check
 * @param err - pointer to int where error code is saved
 *
 * @return int - 1 if everything ok and 0 if canary broken
 **/
int proveCanary(Elem_t *data, size_t capacity);

/**
 *
 * Counts hash of a value
 *
 * @param ptr - pointer to value
 * @param size - size of value in bytes
 *
 * @return size_t - hash value
 **/
size_t countHash(void *ptr, size_t size);

/**
 *
 * Checks if array hash is not broken
 *
 * @param stack - pointer to stack where hash saved
 *
 * @return int - 1 if everything ok and 0 if canary broken
 **/
int checkDataHash(Stack_t *stack);

/**
 *
 * Checks if stack hash is not broken
 *
 * @param stack - pointer to stack
 *
 * @return int - 1 if everything ok and 0 if hash broken
 **/
int checkStackHash(Stack_t *stack);

/**
 *
 * Checks stack and returns OK(0) if stack is ok.
 * Else returns errorCode (from -1 to ... -inf)
 *
 * @param stack - pointer to stack to check
 *
 * @return int - errorCode
 **/
int verifyStack(Stack_t *stack);

/**
 *
 * Reallocates memory and sets all new values to 0
 *
 * @param ptr - pointer to object to realloc
 * @param size - new size to realloc
 * @param err - pointer to int where error code is saved
 *
 * @return void * - pointer to new reallocated memory
 **/
void *recalloc(void *ptr, size_t amount, size_t elemSize, size_t currentAmount, int *err = nullptr);

#endif