#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dt_hashtable.h"

unsigned int xorhash(char *s, int seed, int m) {
    unsigned int h = seed;
    char c;

    for(int i=0; (c = s[i]) != '\0'; i++) {
        h = h ^ ( (h << 5) + c + (h >> 2) );
    }
    return h%m;
}


DT_HashTable *DT_CreateHashTable(int size) {
    DT_HashTable *table = malloc(sizeof(DT_HashTable));
    if(table == NULL) {
        return NULL;
    }
    
    table->nslots = size;
    table->slots = malloc(size*sizeof(DT_Slot));
    if(table->slots == NULL) {
        free(table);
        return NULL;
    }
    // Hashing seed
    table->seed = SEED;

    for(int i=0; i < table->nslots; i++) {
        table->slots[i].keys = NULL;
        table->slots[i].vals = NULL;
        table->slots[i].written = 0;
        table->slots[i].cap = 0;
    }
    return table;
}

bool DT_SlotHas(DT_Slot *slot, char *key) {
    // If we dont find a key this will return 0
    bool in = 0;
    for(int i=0; i < slot->written; i++) {
        if(strcmp(slot->keys[i], key)==0) {
            // we have found the key so return True
            return 1;
        }
    }
    return in;
}


char *DT_SlotGets(DT_Slot *slot, char *key, int *err) {
    // Error handler indicates if a key was found
    *err = -1;
    for(int i = 0; i < slot->written; i++) {
        if(strcmp(slot->keys[i], key) == 0) {
            // No error
            *err = 0;
            // Found value
            return slot->vals[i];
        }
    }
    return NULL;
}

bool DT_HashTable_Has(DT_HashTable *table, char *key) {
    bool in = 0;
    // Obtain the slot number
    int slot = xorhash(key, table->seed, table->nslots);
    // Check if the key exists
    in = DT_SlotHas(&table->slots[slot], key);
    return in;
}

char *DT_HashTable_Gets(DT_HashTable *table, char *key, int *error) {
    // tmp variables to write to
    // in case the error pointer is NULL
    int tmp = 0;
    int *write = error;
    if(error == NULL) {
        // write to the tmp variable instead of NULL.
        write = &tmp;
    }
    // Obtain slot
    int slot = xorhash(key, table->seed, table->nslots);
    // Reurn value
    return DT_SlotGets(&table->slots[slot], key, write);

}

void DT_SlotInsert(DT_Slot *slot, char *key, char *val) {
    // slots are set to NULL initially to save space
    // when we need to write to that slot a malloc call 
    // is done
    if(slot->keys == NULL) {
        // We need to fill this slot with memory
        slot->cap = INITAL_SIZE;
        // Allocate memory
        slot->keys = malloc(INITAL_SIZE*sizeof(char *));
        slot->vals = malloc(INITAL_SIZE*sizeof(char *));
        // nothing was written yet
        slot->written = 0;
    }
    if(slot->cap <= slot->written) {
        // We have run out of memory so allocate some more
        // using power of 2.
        slot->cap *= 2;
        slot->keys = realloc(slot->keys, slot->cap*sizeof(char *));
        slot->vals = realloc(slot->vals, slot->cap*sizeof(char *));
    }
    
    // Fill the slots
    slot->keys[slot->written] = key;
    slot->vals[slot->written] = val;
    slot->written++;
    
}

void DT_HashTable_Put(DT_HashTable *table, char *key, char *val) {
    // Get the slot
    int slotn = xorhash(key, table->seed, table->nslots);
    DT_SlotInsert(&table->slots[slotn], key, val);
}

void DT_FreeSlots(DT_HashTable *table) {
    for(int i=0; i < table->nslots; i++) {
        if(table->slots[i].keys) {
            free(table->slots[i].keys);
            free(table->slots[i].vals);
        }
    }
}

void DT_FreeHashTable(DT_HashTable *table) {
    DT_FreeSlots(table);
    free(table->slots);
    free(table);
}