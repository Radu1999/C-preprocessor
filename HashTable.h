#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stddef.h>
#include "LinkedList.h"

typedef struct
{
    struct LinkedList *buckets;
    int size;
    int hmax;
    unsigned int (*hash_function)(void *);
    int (*compare_function)(void *, void *);
} HashTable;

void init_ht(HashTable *ht, int hmax, unsigned int (*hash_function)(void *), int (*compare_function)(void *, void *));

void put(HashTable *ht, void *key, size_t key_size_bytes, void *value, size_t value_size_bytes);

void *get(HashTable *ht, void *key);

int has_key(HashTable *ht, void *key);

void remove_ht_entry(HashTable *ht, void *key);

int get_ht_size(HashTable *ht);

int get_ht_hmax(HashTable *ht);

void free_ht(HashTable *ht);

HashTable *resizeS(HashTable **ht);

#endif