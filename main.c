#include "main.h"

int evaluate_string(char *str) { return strlen(str) > 0 && str[0] != '0'; }

int if_ended_or_switched(char *word)
{
	return !strcmp(word, "#elif") || !strcmp(word, "#else") ||
	       !strcmp(word, "#endif");
}
unsigned int hash(void *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *(unsigned char *)str) {
		hash = ((hash << 5) + hash) + c;
		str = (char *)str + 1;
	}

	return hash;
}

int compare_function_strings(void *a, void *b)
{
	return strcmp((char *)a, (char *)b);
}
char *portable_strsep(char **str, const char *delim)
{
	char *st = *str;
	char *p;

	p = (st != NULL) ? strpbrk(st, delim) : NULL;

	if (p == NULL) {
		*str = NULL;
	} else {
		*p = '\0';
		*str = p + 1;
	}
	return st;
}

char *duplicate(const char *s)
{
	char *p = malloc(strlen(s) + 1);

	DIE(p == NULL, "alloc in dup");
	memcpy(p, s, strlen(s) + 1);
	return p;
}

void solve_nested_define(char *value, char *key, struct HashTable *ht)
{
	char *token;
	char delim[28] = "\t\n []{}<>=+-*/%!&|^.,:;()\"\\";
	char *ptr = (char *)duplicate(value);
	char *start_ptr = ptr;
	int offset = 0;
	char final[WORD_SIZE] = {0};

	while ((token = portable_strsep(&ptr, delim)) != NULL) {
		char *initial_token = token;
		int initial_len = strlen(token);

		while (has_key(ht, token))
			token = get(ht, token);

		strncat(final + strlen(final), token, strlen(token));
		strncat(final + strlen(final), value + offset + initial_len,
			(int)(ptr - initial_token - initial_len));
		offset += (int)(ptr - initial_token);
	}
	free(start_ptr);
	put(ht, key, strlen(key), final, strlen(final));
}

void solve_defines(FILE *in, FILE *out, struct HashTable *ht, char *line,
		   char **directories, int dirNum)
{
	char delim[28] = "\t\n []{}<>=+-*/%!&|^.,:;()\"\\";
	int completed_def = 0;
	int completed_udef = 0;
	char key[WORD_SIZE];
	char value[WORD_SIZE];

	int in_if = 0;
	char condition[WORD_SIZE];
	int offset;
	int is_true = 0;
	int is_comm = 0;
	int is_multi = 0;

	while (fgets(line, LINE_SIZE, in)) {
		char *ptr = (char *)duplicate(line);
		char *start_ptr = ptr;
		int len = strlen(line);
		char *token = NULL;
		char *prev_token = token;

		offset = 0;

		if (!is_multi)
			value[0] = '\0';
		while ((token = portable_strsep(&ptr, delim)) != NULL) {
			char *initial_token = token;

			if (in_if == 2 && !is_true &&
			    !if_ended_or_switched(token)) {
				break;
			}
			if (prev_token != NULL) {
				char *p = NULL;

				for (p = line + (prev_token - start_ptr);
				     p < line + (prev_token - start_ptr) +
					     (token - prev_token);
				     p++) {
					if (*p == '"')
						is_comm = !is_comm;
				}
			}
			if (!strcmp(token, "#else")) {
				is_true = !is_true;
			} else if (!strcmp(token, "#if") ||
				   (!strcmp(token, "#elif") && !is_true)) {
				is_true = 0;
				in_if = 1;
			} else if (!strcmp(token, "#ifdef")) {
				is_true = 0;
				in_if = 3;
			} else if (!strcmp(token, "#ifndef")) {
				is_true = 1;
				in_if = 3;
			} else if (in_if == 1 || in_if == 3) {
				char *value = condition;

				memcpy(condition, line + offset,
				       strlen(line) - offset - 1);
				condition[strlen(line) - offset - 1] = '\0';
				if (in_if == 3) {
					if (has_key(ht, value))
						is_true = !is_true;
					in_if = 2;
					break;
				}

				while (has_key(ht, value))
					value = get(ht, value);

				is_true = evaluate_string(value);
				in_if = 2;
				break;
			} else if (!strcmp(token, "#endif")) {
				in_if = 0;
				break;
			} else if ((in_if == 2 && is_true) || in_if == 0) {
				if (!strcmp(token, "#include") &&
				    ptr[strspn(ptr, " ")] == '"') {
					char delim_file[6] = "\t\n []";
					char helper[WORD_SIZE];
					int has_file = 0;
					int i = 0;

					token = portable_strsep(&ptr, delim);
					token =
					    portable_strsep(&ptr, delim_file);
					memcpy(helper, token,
					       strlen(token) - 1);
					helper[strlen(token) - 1] = '\0';
					solve_file(dirNum, directories,
						   &has_file, line, ht, out,
						   helper);
					DIE(has_file == 0, "no such file");
					break;
				}
				if (!strcmp(token, "#define")) {
					completed_def = 1;
				} else if (completed_def == 1) {
					memcpy(key, token, strlen(token) + 1);
					key[strlen(token)] = '\0';
					completed_def = 2;
				} else if (completed_def == 2) {
					int spaces = strspn(line + offset, " ");
					int value_size = strlen(value);

					memcpy(value + value_size,
					       line + offset + spaces,
					       strlen(line) - offset - spaces);
					value[value_size + strlen(line) -
					      offset - spaces - 1] = '\0';
					if ((line + offset +
					     spaces)[strlen(line) - offset -
						     spaces - 2] == '\\') {
						is_multi = 1;
						value[value_size +
						      strlen(line) - offset -
						      spaces - 2] = '\0';
						break;
					}
					is_multi = 0;
					solve_nested_define(value, key, ht);
					completed_def = 0;
					break;
				} else if (!strcmp("#undef", token)) {
					completed_udef = 1;
				} else if (completed_udef == 1) {
					remove_ht_entry(ht, token);
					completed_udef = 0;
					break;
				} else if (completed_udef == 0) {
					int initial_len = strlen(token);

					while (!is_comm && has_key(ht, token))
						token = get(ht, token);
					fprintf(out, "%s", token);
					fprintf(out, "%.*s",
						(int)(ptr - initial_token -
						      initial_len),
						line + offset + initial_len);
				}
			}
			prev_token = initial_token;
			offset += (int)(ptr - initial_token);
		}
		free(start_ptr);
	}
}

void solve_file(int dirNum, char **directories, int *has_file, char *line,
		struct HashTable *ht, FILE *out, char *helper)
{
	int i = 0;

	for (i = 0; i < dirNum; i++) {
		char *path =
		    malloc(strlen(directories[i]) + strlen(helper) + 1);
		FILE *incl = NULL;

		DIE(path == NULL, "path allocation");

		memcpy(path, directories[i], strlen(directories[i]));
		memcpy(path + strlen(directories[i]), helper, strlen(helper));
		path[strlen(directories[i]) + strlen(helper)] = '\0';

		incl = fopen(path, "r");
		if (incl == NULL) {
			free(path);
			continue;
		}
		solve_defines(incl, out, ht, line, directories, dirNum);
		fclose(incl);
		*has_file = 1;
		free(path);
		break;
	}
}

int main(int argc, char **argv)
{
	char *infile = NULL;
	char *outfile = NULL;
	const int ht_size = 10000;
	char line[LINE_SIZE];
	FILE *in = NULL;
	FILE *out = NULL;

	struct HashTable *ht = malloc(sizeof(struct HashTable));

	int symbol = 0;
	int out_file = 0;
	int direct = 0;
	char *token;
	int dirNum = 1;
	int i = 0;
	char **directories = NULL;

	DIE(ht == NULL, "struct HashTable allocation");
	init_ht(ht, ht_size, hash, compare_function_strings);
	directories = malloc(NUM_DIRS * sizeof(char *));
	DIE(directories == NULL, "directories alloc");
	directories[0] = NULL;

	for (i = 1; i < argc; i++) {
		if (strlen(argv[i]) == 2 && !strncmp(argv[i], "-D", 2))
			symbol = 1;
		else if (symbol == 1) {
			char *key = NULL;

			token = strtok(argv[i], "=");
			key = (char *)duplicate(token);
			token = strtok(NULL, "=");
			if (token == NULL)
				put(ht, key, strlen(key), "", 0);
			else
				put(ht, key, strlen(key), token, strlen(token));
			free(key);
			symbol = 0;
		} else if (!strncmp(argv[i], "-D", 2)) {
			char *key = NULL;

			token = strtok(argv[i], "=");
			key = (char *)duplicate(token + 2);
			token = strtok(NULL, "=");
			if (token == NULL)
				put(ht, key, strlen(key), "", 0);
			else
				put(ht, key, strlen(key), token, strlen(token));
			free(key);
		} else if (strlen(argv[i]) == 2 && !strncmp(argv[i], "-o", 2))
			out_file = 1;
		else if (out_file == 1) {
			outfile = argv[i];
			out_file = 0;
		} else if (!strncmp(argv[i], "-o", 2))
			outfile = argv[i] + 2;
		else if (strlen(argv[i]) == 2 && !strncmp(argv[i], "-I", 2))
			direct = 1;
		else if (direct == 1) {
			directories[dirNum] = malloc(strlen(argv[i]) + 2);
			DIE(directories[dirNum] == NULL, "Directory alloc");
			memcpy(directories[dirNum], argv[i], strlen(argv[i]));
			memcpy(directories[dirNum] + strlen(argv[i]), "/", 1);
			directories[dirNum++][strlen(argv[i]) + 1] = '\0';
			direct = 0;
		} else if (infile == NULL) {
			char *copy = NULL, *copy_start = NULL;
			char *token = NULL, *aux = NULL;
			char delim[2] = "/";
			char path[WORD_SIZE] = {0};

			infile = argv[i];
			copy = (char *)duplicate(infile);
			copy_start = copy;
			token = strtok(copy, delim);

			while (token != NULL) {
				aux = token;
				token = strtok(NULL, delim);
				if (token != NULL) {
					memcpy(path + strlen(path), aux,
					       strlen(aux));
					memcpy(path + strlen(path), "/", 1);
				}
			}
			directories[0] = malloc(strlen(path) + 1);
			DIE(directories[0] == NULL, "directory alloc");
			memcpy(directories[0], path, strlen(path));
			directories[0][strlen(path)] = '\0';
			free(copy_start);
		} else if (outfile == NULL)
			outfile = argv[i];
		else {
			exit(11);
		}
	}

	if (directories[0] == NULL) {
		directories[0] = malloc(3);
		DIE(directories[0] == NULL, "Directory alloc");
		memcpy(directories[0], "./\0", 3);
	}

	if (infile == NULL)
		in = stdin;
	else {
		in = fopen(infile, "r");
		DIE(in == NULL, "open input file error");
	}

	if (outfile == NULL)
		out = stdout;
	else {
		out = fopen(outfile, "w");
		DIE(out == NULL, "open output file error");
	}

	solve_defines(in, out, ht, line, directories, dirNum);

	fclose(in);
	fclose(out);
	free_ht(ht);
	for (i = 0; i < dirNum; i++)
		free(directories[i]);
	free(directories);
	return 0;
}
