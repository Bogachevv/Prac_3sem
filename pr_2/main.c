#include <stdio.h>
#include <string.h>
#include <stdlib.h>
<<<<<<< HEAD
=======
#include "argparse.h"
>>>>>>> 32970be695d7af7424745a441474701b3e38a3f2

#define BUF_BLOCK_SIZE 64
#define ALLOC_ERR 111
#define FILE_ERR 112
#define EMPTY_STR_ERR 113

#define RESIZE(buf, buf_len){                                                       \
        buf_len += BUF_BLOCK_SIZE;                                                  \
        buf = realloc(buf, buf_len * sizeof(*buf));                                 \
        if (!(buf)){                                                                \
            fprintf(stderr, "Allocation error: not enough memory\n");               \
            exit(ALLOC_ERR);                                                        \
        }                                                                           \
    }

#define PUSH_BACK(buf, buf_len, len, elm) {                                         \
	if (len >= (buf_len)){                                                          \
		RESIZE(buf, (buf_len));                                                     \
	}                                                                               \
	(buf)[(len)++] = (elm);                                                         \
}

typedef struct node_t {
    char* key;
<<<<<<< HEAD
    size_t count;
    struct node_t *left, *right;
} node_t;

size_t search(node_t* node, char* key){
    if (node == NULL)
        return 0;
    int cmp = strcmp(node->key, key);
    if (cmp == 0)
        return node->count;
    else if (cmp > 0)
        return search(node->right, key);
    else
        return search(node->left, key);
=======
    int count;
    struct node_t *left, *right;
} node_t;

int get_tree_len(node_t* tree){
    if (tree == NULL)
        return 0;
    else return 1 + get_tree_len(tree->left) + get_tree_len(tree->right);
>>>>>>> 32970be695d7af7424745a441474701b3e38a3f2
}

node_t* add_node(node_t* node, char* key, size_t len){
    if (!key)
        exit(EMPTY_STR_ERR);

    if (!node){
        //generate new leaf
        node_t* new_node = calloc(1, sizeof(node_t));
        new_node->key = malloc(len);
        strncpy(new_node->key, key, len);
        new_node->count = 1;
        return new_node;
    }

    int cmp = strcmp(node->key, key);
    if (cmp == 0){
        ++(node->count);
    }
    else if (cmp > 0){
        node_t* new_node = add_node(node->right, key, len);
        node->right = (new_node) ? new_node : node->right;
    }else{
        node_t* new_node = add_node(node->left, key, len);
        node->left = (new_node) ? new_node : node->left;
    }
    return NULL;
}

node_t* parse_input(node_t* tree, FILE* inp, size_t* word_c){
    int ch;
    char* buf = NULL;
    size_t buf_size = 0, str_len = 0;
    *word_c = 0;

    while (!feof(inp)){
        while (((ch = fgetc(inp)) != EOF) && (ch != '\n') && (ch != ' ')){
            PUSH_BACK(buf, buf_size, str_len, ch);
        }
        PUSH_BACK(buf, buf_size, str_len, 0);

        if (str_len <= 1){
            str_len = 0;
            continue;
        }

        ++(*word_c);
        node_t* res = add_node(tree, buf, str_len);
        tree = (tree) ? tree : res;
        str_len = 0;
    }

    free(buf);
    return tree;
}

void clear_tree(node_t* tree){
    if (!tree)
        return;
    free(tree->key);
    clear_tree(tree->left);
    clear_tree(tree->right);
    free(tree);
}

<<<<<<< HEAD
void print_words(node_t* tree, size_t word_c){
    if (!tree)
        return;
    printf("%s %zu %f\n", tree->key, tree->count, ((float)tree->count) / word_c);
    print_words(tree->left, word_c);
    print_words(tree->right, word_c);
}

int main(int argc, char** argv){
    FILE* inp = stdin;
    FILE* out = stdout;
=======
int fill_nodes(node_t** nodes, int offset, node_t* tree){
    if (tree == NULL){
        return offset;
    }
    nodes[offset++] = tree;
    offset = fill_nodes(nodes, offset, tree->left);
    offset = fill_nodes(nodes, offset, tree->right);
    return offset;
}

int compare(const void* left, const void* right){
    const node_t **lp = (const node_t**)left, **rp = (const node_t**)right;
    return ((int)((*lp)->count) - (int)((*rp)->count));
}

node_t** sort_tree(node_t* tree, size_t word_c){
    node_t* *nodes = malloc(word_c * sizeof(node_t*));
    fill_nodes(nodes,0, tree);
    qsort(nodes, word_c, sizeof(node_t*), compare);
    return nodes;
}

void print_words(node_t* tree, size_t word_c, FILE* out){
    int tree_len = get_tree_len(tree);
    node_t** nodes = sort_tree(tree, tree_len);
    for (size_t i = 0; i < tree_len; ++i){
        fprintf(out,"%s\t%d\t%f\n", nodes[i]->key, nodes[i]->count, (double)nodes[i]->count / word_c);
    }
    free(nodes);
}

int main(int argc, char** argv){
    arg_t args[] = {{'i', NULL, "Input file"},
                    {'o', NULL, "Output file"}};
    parse(argc, argv, args, 2);

    FILE* inp = stdin;
    FILE* out = stdout;
    if (args[0].val != NULL){
        inp = fopen(args[0].val, "r");
    }
    if (args[1].val != NULL){
        out = fopen(args[1].val, "w");
    }
    if ((inp == NULL) || (out == NULL)){
        fprintf(stderr,"Can't open file\n");
        exit(FILE_ERR);
    }
>>>>>>> 32970be695d7af7424745a441474701b3e38a3f2

    size_t word_c;
    node_t* tree = parse_input(NULL, inp, &word_c);
    printf("Parsed\n");

    printf("Tree: %p\n", tree);
<<<<<<< HEAD
    print_words(tree, word_c);
    clear_tree(tree);

=======
    print_words(tree, word_c, out);
    clear_tree(tree);

    fclose(inp);
    fclose(out);
>>>>>>> 32970be695d7af7424745a441474701b3e38a3f2
	return 0;
}
