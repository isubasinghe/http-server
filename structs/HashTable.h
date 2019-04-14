#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEED 73802
#ifndef INITAL_SIZE
#define INITAL_SIZE 2

typedef struct {
    char **Keys;
    char **Vals;
    size_t Cap;
    size_t Written;
} Slot;

typedef struct {
    Slot *Slots;
    size_t NumSlots;
    size_t Seed;
} HashTable;

static unsigned int xorhash(char *s, size_t seed, size_t m) {
    unsigned int h = seed;
    char c;

    for(size_t i=0; (c = s[i]) != 0; i++) {
        h = h ^ ( (h << 5) + c + (h >> 2) );
    }
    return h%m;
}




HashTable *HashTable_Create(int);

void HashTable_SlotInsert(Slot *slot, char *key, char *value);

char HashTable_SlotHas(Slot *slot, char *key);

int HashTable_SlotGets(Slot slot, char *key, int *err);

int HashTable_Get(HashTable *hashtable, char *key, int *err);

void HashTable_Put(HashTable *hashtable, char *key, char *value);

bool HashTable_Has(HashTable *hashtable, char *key);

void FreeSlot(Slot slot);

void FreeSlots(HashTable *hashtable);

void HashTable_Free(HashTable *hashtable);


#endif // HASHTABLE