#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include "lex.h"
#include "utils.h"

void free_parsed(char **parsed){
	for (char **str_ptr = parsed; *str_ptr; ++str_ptr){ 
		free(*str_ptr);
	}
	free(parsed);
}


int run_cmd(char **args){
	char *cmd = args[0];
	int exit_code;
	if (strncmp(cmd, "cd", 2ul) == 0){
		exit_code = cd(args[1]);
		return exit_code;
	}
	pid_t pid = fork();
	if (pid == -1){
		printf("Fork error: %d\n", errno);
		return errno;
	}
	if (pid){
		int status;
		wait(&status);
		printf("Exit status: %d\n", status);
		return 0;
	}
	else{
		char prog_path[PATH_MAX];
		if ((cmd[0] != '.') && (cmd[0] != '/'))
			sprintf(prog_path, "/usr/bin/%s", cmd);
		else
			sprintf(prog_path, "%s", cmd);
		int rs = execv(prog_path, args);
		printf("Execv error: %d\n", rs);
	}


}


int main(int argc, char** argv){
	char *str = NULL; size_t str_cap = 0, len = 0;
	int run_status = 0;
	while (!feof(stdin)){
		if (run_status) printf("[%d]>>", run_status);
		else printf(">>");
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		char **parsed = parse_input(str);
		run_status = run_cmd(parsed);
		free_parsed(parsed);
	}
	free(str);
}
