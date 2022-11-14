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
		cmd.mode = CMD_CONTROLLER;
		exit(2);
		//run_cmd(&cmd, -1);
	}

	while (alive_c > 0) { //waiting childs 
	}

}
