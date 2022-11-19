#pragma once

#include "queue.h"

#define EXIT_C -2

#define CMD_ASYNC -1
#define CMD_DEFAULT 0
#define CMD_ON_SUCCESS 1
#define CMD_ON_ERROR 2
#define CMD_CONVEYOR 3

typedef struct cmd{
	char *path;
	char **args;
	int argc;
	int inp_ph, out_ph, err_ph;
	int fd_to_close;
	int father_fd_to_close[2];
	struct cmd *next;
	int mode;
} cmd_t;

cmd_t *prepare_cmd(char **args, int argc);

cmd_t *prepare_cmd_seq(char **args);

int change_fd(int old_fd, int new_fd);

void parse_status(int status, int *usr_code, int *sys_code);

int run_cmd(cmd_t *cmd, queue_t *async_queue);

void free_cmd(cmd_t *cmd);
