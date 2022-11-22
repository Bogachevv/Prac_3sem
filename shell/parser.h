#pragma once

#include "cmd_mode.h"

typedef struct arg_seq{
	char **args;
	int next_mode;
	struct arg_seq *next;
} arg_seq_t;

arg_seq_t *parse_args(char **args);

void free_arg_seq(arg_seq_t *arg_seq);
