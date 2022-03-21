#ifndef __MAIN_H__
#define __MAIN_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HashTable.h"
#define LINE_SIZE 500
#define WORD_SIZE 500
#define NUM_DIRS 100

void solve_file(int dirNum, char **directories, int *has_file, char *line,
		struct HashTable *ht, FILE *out, char *helper);
int evaluate_string(char *str);
void solve_nested_define(char *value, char *key, struct HashTable *ht);
char *duplicate(const char *s);
void solve_file(int dirNum, char **directories, int *has_file, char *line,
		struct HashTable *ht, FILE *out, char *helper);
#endif
