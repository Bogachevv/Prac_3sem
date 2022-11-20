#pragma once

#include <sys/types.h>

#define T pid_t
#define BLOCK_SIZE 64

typedef struct block{
	T *begin, *left, *right;
	struct block *prev, *next;
} block_t;

typedef struct queue{
	block_t *left, *right;
	block_t *free_block;
	size_t len;
} queue_t;

int init(queue_t *deq);

void free_queue(queue_t *deq);

int push_back(queue_t *deq, T elm);

T pop(queue_t *deq);
