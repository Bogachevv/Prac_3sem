#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    size_t word_c;
    node_t* tree = parse_input(NULL, inp, &word_c);
    printf("Parsed\n");

    printf("Tree: %p\n", tree);
    print_words(tree, word_c);
    clear_tree(tree);

	return 0;
}
