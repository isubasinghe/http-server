#include "dt_warray.h"

DT_WArray *DT_CreateWArray() {
    // Create a new array structure in the heap
    DT_WArray *array = malloc(sizeof(DT_WArray));
    if(array == NULL) {
        return NULL;
    }

    // Add some memory to it
    array->cap = INITIAL_SIZE;
    array->data = malloc(array->cap * sizeof(char *));
    memset(array->data, 0, array->cap*sizeof(char *));
    if(array->data == NULL) {
        free(array);
        return NULL;
    }

    array->written = 0;
    return array;
}

void DT_WArray_Append(DT_WArray *w, char *s) {
    if(w->written >= w->cap) {
        // Run out of memory so realloc some more
        w->cap *= 2;
        void *tmp = realloc(w->data, w->cap*sizeof(char *));
        if(tmp == NULL) {
            DT_FreeWArray(w);
            return;
        }
        w->data = tmp;
        
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