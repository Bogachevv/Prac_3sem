#include <stdio.h>

typedef struct link{
	int leaf;
	struct link *left, *right;

} link_t;

link_t* search(link_t* tree, int x){
	if (tree != NULL){
		return search((x < tree->leaf) ? tree->left : tree->right);
	}
	return *tree;
}

int main(){
	printf("abba\n");
}
