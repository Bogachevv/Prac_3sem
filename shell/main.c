#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>

#include "lex.h"
#include "utils.h"

void free_parsed(char **parsed){
	for (char **str_ptr = parsed; *str_ptr; ++str_ptr){ 
		free(*str_ptr);
	}
	free(parsed);
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
	close(old_fd);
	int state = dup2(new_fd, old_fd);
	close(new_fd);
	return state;
}


int run_cmd(char **args, int inp_fd, int out_fd, int err_fd){
	char *cmd = args[0];
	if (strncmp(cmd, "cd", 2ul) == 0){
		return cd(args[1]);
	}
	
	pid_t pid = fork();
	if (pid == -1){
		printf("Fork error: errno%d\n", errno);
		return -1;
	}
	if (pid){
		int status = 0;
		waitpid(pid, &status, 0);
		int sys_code = status & 0xFF;
		int usr_code = (status >> 8) & 0xFF;
		printf("Exit status(usr, sys): %d, %d\n", usr_code, sys_code);
		return status;
	}
	else{
		char prog_path[PATH_MAX];
		int rs = get_full_path(cmd, prog_path, PATH_MAX);
		if (rs != 0){
			printf("Can't find %s\n", cmd);
			exit(127);
		}
		printf("Running %s\n", prog_path);
		
		change_fd(0, inp_fd);	
		change_fd(1, out_fd);	
		change_fd(2, err_fd);	
	
		execv(prog_path, args);
		printf("Execv error: %d\n", rs);
		exit(rs);
	}
}


int main(int argc, char** argv){
	char *str = NULL; size_t str_cap = 0, len = 0;
	int run_status = 0, usr_code = 0, sys_code = 0;
	while (!feof(stdin)){
		if (sys_code) printf("[SIG %s]>>", strsignal(sys_code));
		else if (usr_code) printf("[%d]>>", usr_code);
		else printf(">>");
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		char **parsed = parse_input(str);
		if (*parsed == NULL) continue;
		run_status = run_cmd(parsed, 0, 1, 2);
		if (run_status == -1) {
			printf("Shell error\n");
			usr_code = 0; sys_code = 0;
		}
		else{
			sys_code = run_status & 0xFF;
			usr_code = (run_status >> 8) & 0xFF;
		}
		free_parsed(parsed);
	}
	free(str);

	return 0;
}
