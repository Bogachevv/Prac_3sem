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
}

void process(struct List *pl, const char *str){
	struct Node *cur = pl->first;
	struct Node *new_tail = NULL, *last = NULL;
	if (cur == NULL) return;
	while (cur) {
		int cmp = strcmp(cur->elem, str);
		struct Node *next = cur->next;
		if (cmp == 0){
			extract_node(pl, cur);
			free_node(cur);
		}
		else if (cmp > 0){  // cmp > 0
			extract_node(pl, cur);
			if (new_tail == NULL) new_tail = cur;
			else new_tail->next = cur;
			cur->next = NULL;
			last = cur;
		}
		cur = next;
	}
	if (pl->first == NULL) {
		pl->first = new_tail;
	}
	else{
		for (cur = pl->first; cur->next; cur = cur->next) {}
		cur->next = new_tail;
		if (new_tail) new_tail->prev = cur;
	}
	pl->last = last;
}

