#include "list.h"
#include "colors.h"

long grDumpCounter = 0;

void _listCtor(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    list->zero = (ListElement_t*) calloc(1, sizeof(ListElement_t));
    CHECK(!list->zero, CANNOT_ALLOC_MEM);
    
    list->zero->value    = POISON;
    list->zero->next     = list->zero;
    list->zero->previous = list->zero;

    list->size       = 0;

    if (err) *err = listVerify(list);
}

int listVerify(List_t *list) {
    if (!list)                          return LIST_NULL;
    if (!list->zero)                    return LIST_DATA_NULL;
    if (list->size == -1)               return LIST_SIZE_POISONED;
    if (list->size < 0)                 return LIST_SIZE_POISONED;

    return LIST_OK;
}

ListElement_t* _listInsertPhys(List_t *list, Elem_t value, ListElement_t *index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!index, INDEX_NULL);

    ListElement_t *pushInd  = elementNew(value, index->next, index, err);

    index->next->previous = pushInd;
    index->next           = pushInd;
    list->size++;

    if (err) *err = listVerify(list);

    return pushInd;
}

ListElement_t* elementNew(Elem_t value, ListElement_t* next, ListElement_t* prev, int *err) {
    if (!next || !prev) {
        if (err) *err = INDEX_NULL;
        return nullptr;
    }

    ListElement_t *newElem  = (ListElement_t *) calloc(1, sizeof(ListElement_t));

    newElem->value    = value;
    newElem->next     = next;
    newElem->previous = prev;

    return newElem;
}

ListElement_t* listInsert(List_t *list, Elem_t value, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->size, INDEX_BIGGER_SIZE);

    ListElement_t *physIndex = logicToPhysics(list, index);
    return _listInsertPhys(list, value, physIndex, err);
}

ListElement_t* listPushBack(List_t *list, Elem_t value, int *err) {
    CHECK(!list, LIST_NULL);

    ListElement_t *lastInd = list->zero->previous;
    return _listInsertPhys(list, value, lastInd, err);
}

ListElement_t* listPushFront(List_t *list, Elem_t value, int *err) {
    CHECK(!list, LIST_NULL);

    return _listInsertPhys(list, value, list->zero, err);
}

Elem_t _listRemovePhys(List_t *list, ListElement_t *index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!index, INDEX_NULL);
    CHECK(list->size == 0, NOTHING_TO_DELETE);

    Elem_t returnValue = index->value;

    index->previous->next = index->next;
    index->next->previous = index->previous;
    elementDelete(index, err);

    list->size--;

    return returnValue;
}

void elementDelete(ListElement_t* element, int *err) {
    if (!element) {
        if (err) *err = INDEX_NULL;
        return;
    }

    free(element);
}

Elem_t listRemove(List_t *list, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->size, INDEX_BIGGER_SIZE);
    CHECK(list->size == 0, NOTHING_TO_DELETE);

    ListElement_t *physIndex = logicToPhysics(list, index);
    return _listRemovePhys(list, physIndex, err);
}

[[nodiscard]] ListElement_t *logicToPhysics(List_t *list, long logicIndex, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(logicIndex > list->size, INDEX_INCORRECT);

    if (list->size == 0) return list->zero;

    ListElement_t *pos = list->zero->next;
    if (logicIndex == 0) return pos;

    for (long i = 0; i < logicIndex; i++) {
        if (pos->next == list->zero) {
            return pos;
        }
        pos = pos->next;
    }

    return pos;
}

void listDtor(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    ListElement_t *ind = list->zero;
    for (long i = 0; i < list->size; i++) {
        ListElement_t *nextInd = ind->next;
        free(ind);
        ind = nextInd;
    }

    list->size       = -1;
}

void visualGraph(List_t *list, const char *action) {
    if (!list) return;

    FILE *tempFile = fopen("graph/temp.dot", "w");
    if (!tempFile) return;

    mprintf(tempFile, "digraph structs {\n");
    mprintf(tempFile, "\trankdir=LR;\n");

    ListElement_t *index = list->zero;
    for (long i = 0; i < list->size + 1; i++) {
        char color[MAX_COMMAND_LENGTH];
        if (index == list->zero) {
            strcpy(color, NULL_BLOCK);
        } else {
            strcpy(color, WORK_BLOCK);
        }

        mprintf(
                    tempFile, 
                    "\tlabel%p[shape=record, style=\"rounded, filled\", fillcolor=\"%s\", label=\"{ {index: %ld | addr: %p} | val: %d | {n: %p | p: %p} }\"];\n", 
                    index, 
                    color,
                    i,
                    index,
                    index->value, 
                    index->next,
                    index->previous
                );

        index = index->next;
    }

    index = list->zero;
    for (long i = 0; i < list->size + 1; i++) {
        ListElement_t *nextIndex =  index->next;
        if (nextIndex == list->zero) break;

        mprintf(tempFile, "\tlabel%p->label%p [color=\"%s\", style=\"dashed\",arrowhead=\"none\"]", index, nextIndex, PHYS_LINK);

        index = nextIndex;
    }

    index = list->zero->next;
    for (long i = 0; i < list->size + 1; i++) {
        ListElement_t *nextIndex =  index->next;
        ListElement_t *prevIndex =  index->previous;

        mprintf(tempFile, "\tlabel%p->label%p [color=\"red\"]\n", index, nextIndex);
        if (prevIndex) mprintf(tempFile, "\tlabel%p->label%p [color=\"blue\"]\n", index, prevIndex);
        index = nextIndex;
    }

    mprintf(tempFile, "}");

    fclose(tempFile);

    char command[MAX_COMMAND_LENGTH] = {};
    sprintf(command, "dot -Tsvg graph/temp.dot > graph/img%ld.svg", grDumpCounter);
    system(command);

    // adding to html
    FILE* graphFile = nullptr;
    if (grDumpCounter == 0) {
        graphFile = fopen("graph/gdump.html", "w");
    } else {
        graphFile = fopen("graph/gdump.html", "a");
    }
    if (!graphFile) return;

    fprintf(graphFile, "<pre>\n");
    fprintf(graphFile, "<hr>\n<h2>%s </h2>\n", action);
    fprintf(graphFile, "<h3>List size: %ld</h3>\n", list->size);
    fprintf(graphFile, "<img src=\"img%ld.svg\" />\n</hr>\n", grDumpCounter);

    fclose(graphFile);

    grDumpCounter++;
}

#if _DEBUG
void mprintf(FILE *file, const char *fmt...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
}

void dumpList(List_t *list, int errorCode, const char *fileName, const char *function, int line) {
    mprintf(stderr, "Assertion failed with code %d\n", errorCode);
    mprintf(stderr, "in %s at %s(%d)\n", function, fileName, line);
    if (!list) {
        mprintf(stderr, "List was null\n");
        return;
    }

    mprintf(stderr, "List_t[%p] '%s' at %s at %s(%d)\n", list, list->debugInfo.name, list->debugInfo.createFunc, list->debugInfo.createFile, list->debugInfo.createLine);
    mprintf(stderr, "\tsize = %ld\n", list->size);

    if (!list->zero) {
        mprintf(stderr, "Values are null\n");
        return;
    }

    mprintf(stderr, "Values:\n\t");
    ListElement_t *index = list->zero;   
    for (long i = 0; i < list->size + 1; i++) {
        mprintf(stderr, "%9d (%p)", index->value, index);
        index = index->next;
    }

    mprintf(stderr, "\nNext:\n\t");
    index = list->zero;   
    for (long i = 0; i < list->size + 1; i++) {
        mprintf(stderr, "%25p ", index->next);
        index = index->next;
    }

    mprintf(stderr, "\nPrevious:\n\t");
    index = list->zero;   
    for (long i = 0; i < list->size + 1; i++) {
        mprintf(stderr, "%25p ", index->previous);
        index = index->next;
    }
    mprintf(stderr, "\n\n");
}
#endif