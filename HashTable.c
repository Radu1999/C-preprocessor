#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "HashTable.h"

struct info
{
    void *key;
    void *value;
};

void init_ht(HashTable *ht, int hmax, unsigned int (*hash_function)(void *), int (*compare_function)(void *, void *))
{
    ht->buckets = malloc(hmax * sizeof(struct LinkedList));
    for (int i = 0; i < hmax; i++)
    {
        init_list(&ht->buckets[i]);
    }
    ht->compare_function = compare_function;
    ht->hash_function = hash_function;
    ht->hmax = hmax;
    ht->size = 0;
}

void put(HashTable *ht, void *key, size_t key_size_bytes, void *value, size_t value_size_bytes)
{
    int index = ht->hash_function(key) % ht->hmax;
    for (struct Node *it = ht->buckets[index].head; it != NULL; it = it->next)
    {
        if (!ht->compare_function(((struct info *)it->data)->key, key))
        {
            ((struct info *)it->data)->value = value;
            return;
        }
    }
    struct info *new_entry = malloc(sizeof(struct info));
    new_entry->key = malloc(key_size_bytes);
    memcpy(new_entry->key, key, key_size_bytes);
    new_entry->value = malloc(value_size_bytes);
    memcpy(new_entry->value, value, value_size_bytes);
    add_nth_node(&ht->buckets[index], ht->buckets[index].size, new_entry);
    ht->size++;
}

void *get(HashTable *ht, void *key)
{
    int index = ht->hash_function(key) % ht->hmax;
    for (struct Node *it = ht->buckets[index].head; it != NULL; it = it->next)
    {
        if (!ht->compare_function(((struct info *)it->data)->key, key))
        {
            return ((struct info *)it->data)->value;
        }
    }
    return NULL;
}

int has_key(HashTable *ht, void *key)
{
    int index = ht->hash_function(key) % ht->hmax;
    for (struct Node *it = ht->buckets[index].head; it != NULL; it = it->next)
    {
        if (!ht->compare_function(((struct info *)it->data)->key, key))
        {
            return 1;
        }
    }
    return 0;
}

void remove_ht_entry(HashTable *ht, void *key)
{
    int index = ht->hash_function(key) % ht->hmax;
    struct Node *rm;
    int position = 0;
    for (struct Node *it = ht->buckets[index].head; it != NULL; it = it->next, position++)
    {
        if (!ht->compare_function(((struct info *)it->data)->key, key))
        {
            rm = remove_nth_node(&ht->buckets[index], position);
            free(((struct info *)rm->data)->key);
            free(rm->data);
            free(rm);
            ht->size--;
            break;
        }
    }
}

void free_ht(HashTable *ht)
{
    struct Node *it;
    for (int i = 0; i < ht->hmax; i++)
    {
        while (get_size(&ht->buckets[i]))
        {
            it = remove_nth_node(&ht->buckets[i], 0);
            free(((struct info *)it->data)->key);
            free(it->data);
            free(it);
        }
    }
    free(ht->buckets);
    free(ht);
}

int get_ht_size(HashTable *ht)
{
    if (ht == NULL)
    {
        return -1;
    }

    return ht->size;
}

int get_ht_hmax(HashTable *ht)
{
    if (ht == NULL)
    {
        return -1;
    }

    return ht->hmax;
}