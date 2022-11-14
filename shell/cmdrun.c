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

int check_file(char *dir, char *fname){
	char path[PATH_MAX];
	sprintf(path, "%s/%s", dir, fname);
	FILE *fd = fopen(path, "r");
	if (fd == NULL) 
		return 0;
	else{
		fclose(fd);
		return 1;
	}
}

int get_full_path(char *cmd, char *buf, size_t buf_cap){
	if ((cmd[0] == '.') || (cmd[0] == '/')) {
		strncpy(buf, cmd, PATH_MAX);
		return 0;
	}
	char *PATH = getenv("PATH");
	char *PATH_END = PATH + strlen(PATH);
	char *str_beg = PATH, *str_end;
	char dir_path[PATH_MAX];
	int f_check = 0;
	do{
		str_end = strstr(str_beg, ":");
		str_end = (str_end) ? str_end : PATH_END;
		size_t str_len = str_end - str_beg;
		memcpy(dir_path, str_beg, str_len);
		dir_path[str_len] = 0;
		f_check = check_file(dir_path, cmd);
		str_beg = str_end + 1;

	} while ((str_end != PATH_END) && (!f_check));
	
	if (!f_check) return -1;
	if (buf_cap < strlen(dir_path) + strlen(cmd) + 1){
		printf("Size error: can't copy full path to the buffer \n(buf size = %ld)\n", buf_cap);
		return -1;
	}
	sprintf(buf, "%s/%s", dir_path, cmd);
	return 0;
}


int change_fd(int old_fd, int new_fd){
	if (old_fd == new_fd) return 0;
	//close(old_fd);
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

int run_cmd(const cmd_t *cmd, int async_fd){
	printf("CMD mode: %d\n", cmd->mode);
	if (strncmp(cmd->path, "cd", 2ul) == 0){
		return cd(cmd->args[1]);
	}

	if (strcmp(cmd->path, "exit") == 0){
		return EXIT_C;
	}
	
	if (cmd->mode == CMD_ASYNC){
		ssize_t len = write(async_fd, cmd, sizeof(*cmd));
		if (len == -1) fprintf(stderr, "Pipe write error\n");
		ssize_t path_len = strlen(cmd->path);
		len = write(async_fd, &path_len, sizeof(path_len));
		if (len == -1) fprintf(stderr, "Pipe write error\n");
		len = write(async_fd, cmd->path, path_len);
		if (len == -1) fprintf(stderr, "Pipe write error\n");
		for (char **arg_p = cmd->args; *arg_p; ++arg_p){
			ssize_t arg_len = strlen(*arg_p);
			len = write(async_fd, &arg_len, sizeof(arg_len));
			if (len == -1) fprintf(stderr, "Pipe write error\n");
			len = write(async_fd, *arg_p, arg_len);
			if (len == -1) fprintf(stderr, "Pipe write error\n");
		}
		return 0;
	}

	pid_t pid = fork();
	if (pid == -1){
		printf("Fork error: errno%d\n", errno);
		return -1;
	}
	if (pid){
		int status = 0;
		if (cmd->mode == CMD_CONTROLLER){
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
		
		//char prog_path[PATH_MAX];
		//int rs = get_full_path(cmd->path, prog_path, PATH_MAX);
		//if (rs != 0){
		//	fprintf(stderr, "Can't find %s\n", cmd->path);
		//	exit(127);
		//}
		//printf("Running %s\n", prog_path);
		printf("Running %s\n", cmd->path);
		
		change_fd(0, cmd->inp_ph);	
		change_fd(1, cmd->out_ph);	
		change_fd(2, cmd->err_ph);	
	
		execvp(cmd->path, cmd->args);
		printf("Execv error: %d\n", errno);
		exit(-1);
	}
}

