#include "DT_WArray.h"

DT_WArray *DT_CreateWArray() {
    // Create a new array structure in the heap
    DT_WArray *DT_WArray = malloc(sizeof(DT_WArray));
    // Add some memory to it
    DT_WArray->cap = INITIAL_SIZE;
    DT_WArray->data = malloc(DT_WArray->cap * sizeof(char *));
    DT_WArray->written = 0;
    return DT_WArray;
}

void DT_WArray_Append(DT_WArray *w, char *s) {
    if(w->written >= w->cap) {
        // Run out of memory so realloc some more
        w->cap *= 2;
        w->data = realloc(w->data, w->cap*sizeof(char *));
    }
    // Write to the next spot
    w->data[w->written] = s;
    w->written++;
}

void DT_FreeWArray(DT_WArray *w) {
    for(int i=0; i < w->written; i++) {
        free(w->data[i]);
    }
    free(w->data);
    free(w);
}