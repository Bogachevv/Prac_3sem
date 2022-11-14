#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmdrun.h"

int alive_c = 0;

void chld_handler(int sig){
	int status;
	wait(&status);
	--alive_c;
}

int main(int argc, char **argv){
	int rd_fd = 0;
	if (argc > 1){
		int tmp;
		if (sscanf(argv[1], "%d", &tmp)) rd_fd = tmp;
	}
	printf("rd_fd = %d\n", rd_fd);
	for (;;) {
		cmd_t cmd;
		ssize_t bc = read(rd_fd, &cmd, sizeof(cmd_t));
		if (bc == 0) break;
		if (bc != sizeof(cmd_t)){
			fprintf(stderr, "Controller: read error: %ld, %ld\n", bc, sizeof(cmd_t));
			exit(-1);
		}
		printf("Controller: argc = %d\n", cmd.argc);
		ssize_t path_len;
		ssize_t len = read(rd_fd, &path_len, sizeof(path_len));		
		if (len == -1) fprintf(stderr, "Pipe write error\n");
		cmd.path = calloc(path_len + 1, 1);
		len = read(rd_fd, cmd.path, path_len);
		if (len == -1) fprintf(stderr, "Pipe write error\n");
		printf("cmd.path = %s\n", cmd.path);

		cmd.args = calloc(cmd.argc + 1, sizeof(char*));
		for (int i = 0; i < cmd.argc; ++i){
			ssize_t arg_len;
			len = read(rd_fd, &arg_len, sizeof(arg_len));
			if (len == -1) fprintf(stderr, "Pipe write error\n");
			char *buf = calloc(arg_len + 1, 1);			
			len = read(rd_fd, buf, arg_len);
			if (len == -1) fprintf(stderr, "Pipe write error\n");
			printf("arg[%d] = %s\n", i, buf);
			(cmd.args)[i] = buf;
		}

		cmd.mode = CMD_CONTROLLER;
		run_cmd(&cmd, -1);
	}

	while (alive_c > 0) { //waiting childs 
	}

}
