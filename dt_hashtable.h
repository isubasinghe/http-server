#ifndef DT_HASHTABLE_H
#define DT_HASHTABLE_H


#define SEED 73802
#ifndef INITAL_SIZE
#define INITAL_SIZE 2
#endif // INITIAL_SIZE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


unsigned int xorhash(char *s, int seed, int m);

typedef struct {
    char **keys;
    char **vals;
    int cap;
    int written;
} DT_Slot;

typedef struct {
    DT_Slot *slots;
    int nslots;
    int seed;
} DT_HashTable;

DT_HashTable *DT_CreateHashTable(int size);


bool DT_SlotHas(DT_Slot *slot, char *key);

char *DT_SlotGets(DT_Slot *slot, char *key, int *err);

bool DT_HashTable_Has(DT_HashTable *table, char *key);

char *DT_HashTable_Gets(DT_HashTable *table, char *key, int *error);

void DT_SlotInsert(DT_Slot *slot, char *key, char *val);

void DT_HashTable_Put(DT_HashTable *table, char *key, char *val);

void DT_FreeSlots(DT_HashTable *table);

void DT_FreeHashTable(DT_HashTable *table);


#endif // DT_HASHTABLE_H