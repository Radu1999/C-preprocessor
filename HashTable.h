#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include "LinkedList.h"
#include <stddef.h>

struct HashTable {
	struct LinkedList *buckets;
	int size;
	int hmax;
	unsigned int (*hash_function)(void *a);
	int (*compare_function)(void *a, void *b);
};

void init_ht(struct HashTable *ht, int hmax,
	     unsigned int (*hash_function)(void *),
	     int (*compare_function)(void *, void *));

void put(struct HashTable *ht, void *key, size_t key_size_bytes, void *value,
	 size_t value_size_bytes);

void *get(struct HashTable *ht, void *key);

int has_key(struct HashTable *ht, void *key);

void remove_ht_entry(struct HashTable *ht, void *key);

int get_ht_size(struct HashTable *ht);

int get_ht_hmax(struct HashTable *ht);

void free_ht(struct HashTable *ht);

#endif
