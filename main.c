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

int evaluate_string(char *str)
{
    return strlen(str) > 0 && str[0] != '0';
}

int if_ended_or_switched(char *word)
{
    return !strcmp(word, "#elif") || !strcmp(word, "#else") || !strcmp(word, "#endif");
}

char *solve_defines(FILE *in, HashTable *ht, char *line)
{
    char delim[26] = "\t \n[]{}<>=+-*/%!&|^.,:;()\\";
    int completed_def = 0;
    int completed_udef = 0;
    char key[100];
    char value[100];

    int in_if = 0;
    char condition[100];
    int offset;
    int is_true = 0;

    while (fgets(line, LINE_SIZE, in))
    {
        offset = 0;
        char *ptr = strdup(line);
        char *prev = ptr;
        int len = strlen(line);
        char *token;
        while ((token = strsep(&ptr, delim)) != NULL)
        {
            if (in_if == 2 && !is_true && !if_ended_or_switched(token))
            {
                break;
            }
            if (!strcmp(token, "#else"))
            {
                is_true = !is_true;
            }
            else if (!strcmp(token, "#if"))
            {
                in_if = 1;
            }
            else if (in_if == 1)
            {
                memcpy(condition, line + offset, strlen(line) - offset - 1);
                condition[strlen(line) - offset - 1] = '\0';

                char *value = condition;
                while (has_key(ht, value))
                {
                    value = get(ht, value);
                }

                is_true = evaluate_string(value);
                in_if = 2;
                break;
            }
            else if (!strcmp(token, "#endif"))
            {
                in_if = 0;
            }
            else if ((in_if == 2 && is_true) || in_if == 0)

            {
                if (!strcmp(token, "#define"))
                {
                    completed_def = 1;
                }
                else if (completed_def == 1)
                {
                    memcpy(key, token, strlen(token) + 1);
                    key[strlen(token) + 1] = '\0';
                    completed_def = 2;
                }
                else if (completed_def == 2)
                {
                    memcpy(value, line + offset, strlen(line) - offset - 1);
                    value[strlen(line) - offset - 1] = '\0';

                    put(ht, key, strlen(key), value, strlen(line) - offset - 1);
                    completed_def = 0;
                    break;
                }
                else if (!strcmp("#undef", token))
                {
                    completed_udef = 1;
                }
                else if (completed_udef == 1)
                {
                    remove_ht_entry(ht, token);
                    completed_udef = 0;
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