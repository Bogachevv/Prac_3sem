#include <stdlib.h>

#include "queue.h"

int init_block(block_t *block){
	block->begin = calloc(BLOCK_SIZE, sizeof(T));
	if (block->begin == NULL) return -1;
	block->left  = block->begin;
	block->right = block->begin;
	block->prev = NULL;
	block->next = NULL;
	
	return 0;
}

int init(queue_t *deq){
	deq->left = calloc(1, sizeof(block_t));
	if (deq->left == NULL) return -1;	
	deq->right = deq->left;
	if (init_block(deq->left) == -1) return -1;
	deq->len = 0;	
	deq->free_block = NULL;	

	return 0;
}

void free_block(block_t *block){
	free(block->begin);
	free(block);
}

void reset_block(block_t *block){
	block->left  = block->begin;
	block->right = block->begin;
	block->prev = NULL;
	block->next = NULL;
}

void free_queue(queue_t *deq){
    if (deq == NULL) return;
    block_t *block = deq->left;
    while (block){
        block_t *nxt = block->next;
        free_block(block);
		block = nxt;
    }
	if (deq->free_block != NULL) free_block(deq->free_block);
}

int push_back(queue_t *deq, T elm){
	block_t *block = deq->right;
	if (block->right - block->begin >= BLOCK_SIZE){
		//allocate new block
		block_t *nb;
		if (deq->free_block == NULL){
			nb = calloc(1, sizeof(block_t));
			if (nb == NULL) return -1;
			if (init_block(nb) == -1) return -1;
		}
		else{
			nb = deq->free_block;
			deq->free_block = NULL;
		}

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

T pop(queue_t *deq){
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
			if (deq->free_block == NULL){
				reset_block(block);
				deq->free_block = block;				
			}
			else free_block(block);
		}
	}

	return res;
}
