#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "utils.h"

struct Node {
	void *data;
	struct Node *next;
};

struct LinkedList {
	struct Node *head;
	int size;
};

void init_list(struct LinkedList *list);

void add_nth_node(struct LinkedList *list, int n, void *new_data);

struct Node *remove_nth_node(struct LinkedList *list, int n);

int get_size(struct LinkedList *list);

void free_list(struct LinkedList **list);

void print_int_linkedlist(struct LinkedList *list);

void print_string_linkedlist(struct LinkedList *list);

#endif
