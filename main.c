#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HashTable.h"

#define HASH_MAX 100000

unsigned int
hash(void *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *(unsigned char *)str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int compare_function_strings(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

int main(int argc, char **argv)
{
    HashTable *ht = malloc(sizeof(HashTable));
    init_ht(ht, HASH_MAX, hash, compare_function_strings);
    put(ht, "Radu", 5, "Gabriel");
    printf("%s\n", (char *)get(ht, "Radu"));
    printf("Tema 1 SO");
}