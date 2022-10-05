#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arg_t{
	char key;
	char* val;
	char* description;	
} arg_t;

void print_help(int, char**, arg_t*, int);
void parse(int, char**, arg_t*, int);
