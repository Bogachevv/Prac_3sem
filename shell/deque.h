#include <sys/types.h>

#define T pid_t
#define BLOCK_SIZE 64

typedef struct block{
	T *begin, *left, *right;
	struct block *prev, *next;
} block_t;

typedef struct deque{
	block_t *left, *right;
	size_t len;
} deque_t;

int init(deque_t *deq);

int push_back(deque_t *deq, T elm);

T pop(deque_t *deq);
