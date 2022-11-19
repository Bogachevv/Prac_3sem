#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>

#include "utils.h"
#include "cmdrun.h"

size_t get_argc(char **argv){
	size_t c = 0;
	for (char **arg_p = argv; *arg_p; ++arg_p) ++c;
	return c;
}

void redirect(char ***args_p, cmd_t *cmd){
	char **arg_p = *args_p;
	if (*(arg_p + 1) == NULL){
		fprintf(stderr, "Not enougth args\n");
	}

	if ((*arg_p)[0] == '<'){ //stdin
		int fd = open(*(arg_p + 1), O_RDONLY);
		if (fd == -1){
			fprintf(stderr, "Can't redirect %s to stdin\n", *(arg_p + 1));
		}
		cmd->inp_ph = fd;
		++arg_p;
	}
	else 
	if ((*arg_p)[0] == '>'){ 
		int fd;
		if ((*arg_p)[1] == '>') fd = open(*(arg_p + 1), O_WRONLY | O_APPEND);
		else fd = open(*(arg_p + 1), O_WRONLY | O_CREAT | O_TRUNC, 0666);
		
		if (fd == -1){
			fprintf(stderr, "Can't redirect stdout to %s\n", *(arg_p + 1));
		}
		cmd->out_ph = fd;
		++arg_p;
	}
	*args_p = arg_p;
}

cmd_t *prepare_cmd(char **args, cmd_t *cmd){
	if (cmd == NULL) cmd = calloc(1, sizeof(cmd_t));
	cmd->path = args[0];
	if (cmd->args != NULL) free(cmd->args);
	cmd->args = calloc(cmd->argc + 1, sizeof(char*));
	cmd->mode = CMD_DEFAULT;
		
	cmd->inp_ph = 0;
	cmd->out_ph = 1;
	cmd->err_ph = 2;
	
	char **arg_wr = cmd->args;
	for (char **arg_p = args; *arg_p; ++arg_p){
		if (strchr("<>", (*arg_p)[0])){
			redirect(&arg_p, cmd);
		}
		else if (((*arg_p)[0] == '&') && ((*arg_p)[1] != '&')){
			cmd->mode = CMD_ASYNC;
		}
		else{
			*arg_wr = *arg_p;
			++arg_wr;
		}

	}
	
	cmd->argc = get_argc(cmd->args);	
	cmd->next = NULL;
	return cmd;
}

void free_cmd(cmd_t *cmd){
	if (cmd == NULL) return;
	free(cmd->args);
	free_cmd(cmd->next);
	free(cmd);
}

int change_fd(int old_fd, int new_fd){
	if (old_fd == new_fd) return 0;
	int state = dup2(new_fd, old_fd);
	close(new_fd);
	return state;
}

void parse_status(int status, int *usr_code, int *sys_code){
	*usr_code = 0; *sys_code = 0;
	if (status == 0) return;
	
	if (WIFEXITED(status))
		*usr_code = WEXITSTATUS(status);
	else
		*sys_code = WTERMSIG(status);
}

int wait_async(queue_t *async_queue){
	int pid_c = async_queue->len;
	for (int i = 0; i < pid_c; ++i){
		pid_t pid = pop(async_queue);
		int status;
		pid_t rs = waitpid(pid, &status, WNOHANG);
		if (rs == 0){
			push_back(async_queue, pid);
		}
		else{
			printf("Process %d finished\n", pid);
		}
	}
	return 0;
}

int run_cmd(const cmd_t *cmd, queue_t *async_queue){
	printf("CMD mode: %d\n", cmd->mode);
	wait_async(async_queue);
	if (strncmp(cmd->path, "cd", 2ul) == 0){
		return cd(cmd->args[1]);
	}

	if (strcmp(cmd->path, "exit") == 0){
		return EXIT_C;
	}	

	pid_t pid = fork();
	if (pid == -1){
		printf("Fork error: errno%d\n", errno);
		return -1;
	}
	if (pid){
		int status = 0;
		if (cmd->mode == CMD_ASYNC){
			push_back(async_queue, pid);
			return 0;
		}
		if (cmd->mode == CMD_CONVEYOR){
			//NOT IMPLEMENTED
			return 0;
		}
		else{
			waitpid(pid, &status, 0);
			int usr_code, sys_code;
			parse_status(status, &usr_code, &sys_code);
			printf("Exit status(usr, sys): %d, %d\n", usr_code, sys_code);
			return status;
		}
	}
	else{
		//run command
		printf("Running %s\n", cmd->path);
		
		change_fd(0, cmd->inp_ph);	
		change_fd(1, cmd->out_ph);	
		change_fd(2, cmd->err_ph);	
	
		execvp(cmd->path, cmd->args);
		printf("Execv error: %d\n", errno);
		exit(-1);
	}
}

