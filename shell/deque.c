#include <stdlib.h>

#include "deque.h"

int init_block(block_t *block){
	block->begin = calloc(BLOCK_SIZE, sizeof(T));
	if (block->begin == NULL) return -1;
	block->left  = block->begin;
	block->right = block->begin;
	block->prev = NULL;
	block->next = NULL;
	
	return 0;
}

int init(deque_t *deq){
	deq->left = calloc(1, sizeof(block_t));
	if (deq->left == NULL) return -1;	
	deq->right = deq->left;
	if (init_block(deq->left) == -1) return -1;
	deq->len = 0;	

	return 0;
}

int push_back(deque_t *deq, T elm){
	block_t *block = deq->right;
	if (block->right - block->begin >= BLOCK_SIZE){
		//allocate new block
		block_t *nb = calloc(1, sizeof(block_t));
		if (nb == NULL) return -1;
		if (init_block(nb) == -1) return -1;

		block->next = nb;
		nb->prev = block;
		deq->right = nb;
		block = nb;
	}

	*(block->right) = elm;
	++(block->right);
	++(deq->len);

	return 0;
}

T pop(deque_t *deq){
	block_t *block = deq->left;

	T res = *(block->left);
	++(block->left);
	--(deq->len);

	if (block->left - block->begin >= BLOCK_SIZE){
		if (block->next == NULL){
			//reset block
			block->left  = block->begin;
			block->right = block->begin;
		}
		else{
			//delete block
			deq->left = block->next;
			block->next->prev = NULL;
			free(block->begin);
			free(block);
		}
	}

	return res;
}
