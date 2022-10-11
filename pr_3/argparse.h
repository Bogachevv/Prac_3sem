#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum arg_types {string, boolean} arg_type_t;

typedef struct arg_t{
	char* key;
	char* val;
	char* description;
	arg_type_t arg_type;
} arg_t;

void print_help(int argc, char** argv, arg_t* args, int args_len);

void parse(int argc, char** argv, arg_t* args, int args_len);

