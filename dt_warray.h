#ifndef DT_WArray_H
#define DT_WArray_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INITIAL_SIZE
#define INITIAL_SIZE 128
#endif // INITIAL_SIZE

typedef struct {
    int cap;
    int written;
    char **data;
} DT_WArray;

DT_WArray *DT_CreateWArray();

void DT_WArray_Append(DT_WArray *, char *);

void DT_FreeWArray(DT_WArray *);

#endif
