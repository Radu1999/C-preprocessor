#include "HashTable.h"

struct info {
	void *key;
	void *value;
};

void init_ht(struct HashTable *ht, int hmax,
	     unsigned int (*hash_function)(void *),
	     int (*compare_function)(void *, void *))
{
	int i = 0;

	ht->buckets = malloc(hmax * sizeof(struct LinkedList));
	DIE(ht->buckets == NULL, "Buckets allocation");
	for (i = 0; i < hmax; i++)
		init_list(&ht->buckets[i]);
	ht->compare_function = compare_function;
	ht->hash_function = hash_function;
	ht->hmax = hmax;
	ht->size = 0;
}

void put(struct HashTable *ht, void *key, size_t key_size_bytes, void *value,
	 size_t value_size_bytes)
{
	int index = ht->hash_function(key) % ht->hmax;
	struct Node *it = NULL;
	struct info *new_entry = NULL;

	for (it = ht->buckets[index].head; it != NULL; it = it->next) {
		if (!ht->compare_function(((struct info *)it->data)->key,
					  key)) {
			free(((struct info *)it->data)->value);
			((struct info *)it->data)->value =
			    malloc(value_size_bytes);
			DIE(((struct info *)it->data)->value == NULL,
			    "PUT update value allocation");
			memcpy(((struct info *)it->data)->value, value,
			       value_size_bytes);

			return;
		}
	}

	new_entry = malloc(sizeof(struct info));
	DIE(new_entry == NULL, "New entry allocation");
	new_entry->key = malloc(key_size_bytes + 1);
	DIE(new_entry->key == NULL, "KEY for new entry in ht allocation");
	memcpy(new_entry->key, key, key_size_bytes + 1);
	new_entry->value = malloc(value_size_bytes + 1);
	DIE(new_entry->value == NULL, "Value for new entry in ht allocation");
	memcpy(new_entry->value, value, value_size_bytes + 1);
	add_nth_node(&ht->buckets[index], ht->buckets[index].size, new_entry);
	ht->size++;
}

void *get(struct HashTable *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	struct Node *it = NULL;

	for (it = ht->buckets[index].head; it != NULL; it = it->next) {
		if (!ht->compare_function(((struct info *)it->data)->key,
					  key)) {
			return ((struct info *)it->data)->value;
		}
	}
	return NULL;
}

int has_key(struct HashTable *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	struct Node *it = NULL;

	for (it = ht->buckets[index].head; it != NULL; it = it->next) {
		if (!ht->compare_function(((struct info *)it->data)->key,
					  key)) {
			return 1;
		}
	}
	return 0;
}

void remove_ht_entry(struct HashTable *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	struct Node *rm = NULL;
	struct Node *it = NULL;
	int position = 0;

	for (it = ht->buckets[index].head; it != NULL;
	     it = it->next, position++) {
		if (!ht->compare_function(((struct info *)it->data)->key,
					  key)) {
			rm = remove_nth_node(&ht->buckets[index], position);
			free(((struct info *)rm->data)->key);
			free(((struct info *)it->data)->value);
			free(rm->data);
			free(rm);
			ht->size--;
			break;
		}
	}
}

void free_ht(struct HashTable *ht)
{
	struct Node *it;
	int i = 0;

	for (i = 0; i < ht->hmax; i++) {
		while (get_size(&ht->buckets[i])) {
			it = remove_nth_node(&ht->buckets[i], 0);
			free(((struct info *)it->data)->key);
			free(((struct info *)it->data)->value);
			free(it->data);
			free(it);
		}
	}
	free(ht->buckets);
	free(ht);
}

int get_ht_size(struct HashTable *ht)
{
	if (ht == NULL)
		return -1;
	return ht->size;
}

int get_ht_hmax(struct HashTable *ht)
{
	if (ht == NULL)
		return -1;
	return ht->hmax;
}
