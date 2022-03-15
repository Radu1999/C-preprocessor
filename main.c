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

int solve_defines(FILE *in, FILE *out, HashTable *ht, char *line)
{
    char delim[27] = "\t \n[]{}<>=+-*/%!&|^.,:;()\\";
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
        char *start_ptr = ptr;
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
            else if (!strcmp(token, "#if") || (!strcmp(token, "#elif") && !is_true))
            {
                is_true = 0;
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
                    key[strlen(token)] = '\0';
                    completed_def = 2;
                }
                else if (completed_def == 2)
                {
                    memcpy(value, line + offset, strlen(line) - offset - 1);
                    value[strlen(line) - offset - 1] = '\0';
                    put(ht, key, strlen(key), value, strlen(value));
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
                    fprintf(out, "%s", token);
                    fprintf(out, "%.*s", (int)(ptr - prev - initial_len), line + offset + initial_len);
                }
            }

            offset += (int)(ptr - prev);
            prev = ptr;
        }
        free(start_ptr);
    }
    return 0;
}

int main(int argc, char **argv)
{
    char *infile = NULL;
    char *outfile = NULL;
    const int ht_size = 10000;
    char line[LINE_SIZE];

    HashTable *ht = malloc(sizeof(HashTable));
    init_ht(ht, ht_size, hash, compare_function_strings);

    int symbol = 0;
    int out_file = 0;
    char *token;

    for (int i = 1; i < argc; i++)
    {
        if (strlen(argv[i]) == 2 && !strncmp(argv[i], "-D", 2))
        {
            symbol = 1;
        }
        else if (symbol == 1)
        {
            token = strtok(argv[i], "=");
            char *key = strdup(token);
            token = strtok(NULL, "=");
            put(ht, key, strlen(key), token, strlen(token));
            free(key);
            symbol = 0;
        }
        else if (strlen(argv[i]) == 2 && !strncmp(argv[i], "-o", 2))
        {
            out_file = 1;
        }
        else if (out_file == 1)
        {
            outfile = argv[i];
            out_file = 0;
        }
        else if (infile == NULL)
        {
            infile = argv[i];
        }
        else
        {
            outfile = argv[i];
        }
    }
    FILE *in;
    if (infile == NULL)
    {
        in = stdin;
    }
    else
    {
        in = fopen(infile, "r");
        if (in == NULL)
        {
            free_ht(ht);
            return 1;
        }
    }

    FILE *out;
    if (outfile == NULL)
    {
        out = stdout;
    }
    else
    {
        out = fopen(outfile, "w");
        if (out == NULL)
        {
            fclose(in);
            free_ht(ht);
            return 1;
        }
    }

    if (solve_defines(in, out, ht, line) == 1)
    {
        fclose(in);
        fclose(out);
        free_ht(ht);
        return 1;
    }

    fclose(in);
    fclose(out);
    free_ht(ht);
    return 0;
}