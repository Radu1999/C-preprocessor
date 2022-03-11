#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "HashTable.h"
#define LINE_SIZE 300

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

char *solve_defines(FILE *in, HashTable *ht, char *line)
{
    char delim[26] = "\t \n[]{}<>=+-*/%!&|^.,:;()\\";
    int completed = 0;
    char key[100];
    char value[100];

    int offset;
    while (fgets(line, LINE_SIZE, in))
    {
        offset = 0;
        char *ptr = strdup(line);
        char *prev = ptr;
        int len = strlen(line);
        char *token;
        while ((token = strsep(&ptr, delim)) != NULL)
        {

            if (!strcmp(token, "#define"))
            {
                completed = 1;
            }
            else if (completed == 1)
            {
                memcpy(key, token, strlen(token) + 1);
                completed = 2;
            }
            else if (completed == 2)
            {
                //[TO DO] I can also have an expression
                memcpy(value, line + offset, strlen(line) - offset - 1);
                put(ht, key, strlen(key), value, strlen(line) - offset - 1);
                completed = 0;
                break;
            }
            else
            {
                int initial_len = strlen(token);
                while (has_key(ht, token))
                {
                    token = get(ht, token);
                }
                printf("%s", token);
                printf("%.*s", (int)(ptr - prev - initial_len), line + offset + initial_len);
            }
            offset += (int)(ptr - prev);
            prev = ptr;
        }
    }
}

int main(int argc, char **argv)
{
    char *infile = argv[1];
    char *outfile = "so-cpp.s";
    const int ht_size = 10000;
    char line[LINE_SIZE];

    HashTable *ht = malloc(sizeof(HashTable));
    init_ht(ht, ht_size, hash, compare_function_strings);

    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "w");

    solve_defines(in, ht, line);
}