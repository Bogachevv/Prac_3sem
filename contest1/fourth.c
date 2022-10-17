#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{
	struct Node *prev, *next;
	char *elem;
} node_t;

typedef struct List{
	struct Node *first, *last;
} list_t;

void free_node(node_t* node){
	free(node->elem);
	free(node);
}

void extract_node(list_t *list, node_t *node){
	if (node->prev) node->prev->next = node->next;
	else list->first = node->next;
	if (node->next) node->next->prev = node->prev;
}

void process(struct List *pl, const char *str){
	node_t *cur = pl->first;
	node_t *new_tail = NULL, *last = NULL;
	if (cur == NULL) return;
	while (cur) {
		int cmp = strcmp(cur->elem, str);
		node_t *next = cur->next;
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

void print_list(list_t *list){
	node_t *cur = list->first;
	while (cur != NULL){
		printf("prev = %16p\tnext = %16p\telem = %s\n", cur->prev, cur->next, cur->elem);
		cur = cur->next;
	}
}

int main(){
	node_t *node1 = malloc(sizeof(node1));
	node_t *node2 = malloc(sizeof(node2));
	node_t *node3 = malloc(sizeof(node3));
	node_t *node4 = malloc(sizeof(node4));
	char *str1 = malloc(2); str1[0] = 'A'; str1[1] = 0;
	char *str2 = malloc(2); str2[0] = 'A'; str2[1] = 0;
	char *str3 = malloc(2); str3[0] = 'C'; str3[1] = 0;
	char *str4 = malloc(2); str4[0] = 'A'; str4[1] = 0;
	node1->elem = str1; node2->elem = str2; node3->elem = str3; node4->elem = str4;
	node1->prev = NULL; node2->prev = node1; node3->prev = node2; node4->prev = node3;
	node1->next = node2; node2->next = node3; node3->next = node4; node4->next = NULL;
	list_t list = {node1, node4};	
	print_list(&list);
	printf("\n");
	process(&list, "A");
	print_list(&list);
}
