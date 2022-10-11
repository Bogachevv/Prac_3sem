#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_node(struct Node* node){
	free(node->elem);
	free(node);
}

void extract_node(struct List *list, struct Node *node){
	if (node->prev) node->prev->next = node->next;
	else list->first = node->next;
	if (node->next) node->next->prev = node->prev;
	else list->last = node->prev;
}

void add_back(struct List *list, struct Node *node){
	node->prev = list->last;
	node->next = NULL;
	list->last->next = node;
	list->last = node;
}

void process(struct List *pl, const char *str){
	struct Node *last = pl->last, *cur = pl->first;
	if (pl->first == NULL) return;
	if (pl->last == NULL) return;
	int flag = 1;
	while (flag) {
		flag = cur != last;
		int cmp = strcmp(cur->elem, str);
		struct Node *next = cur->next;
		if (cmp == 0){
			extract_node(pl, cur);
			free_node(cur);
		}
		else if (cmp > 0){  // cmp > 0
			extract_node(pl, cur);
			add_back(pl, cur);	
		}
		cur = next;
	}
}

