#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "lex.h"
#include "cmdrun.h"

void free_parsed(char **parsed){
	for (char **str_ptr = parsed; *str_ptr; ++str_ptr){ 
		free(*str_ptr);
	}
	free(parsed);
}

void print_input_prompt(int usr_code, int sys_code){
	if (sys_code) printf("[SIG %s]>>", strsignal(sys_code));
	else if (usr_code) printf("[%d]>>", usr_code);
	else printf(">>");
}

int start_ac(){
	int pipes[2]; // {pipe.read, pipe.write}
	int rs = pipe(pipes);
	if (rs == -1){
		fprintf(stderr, "Pipe error\n");
		return -1;
	}
	pid_t pid = fork();
	if (pid == -1){
		fprintf(stderr, "Fork error\n");
		return -1;
	}
	if (pid){ //Father
		close(pipes[0]); //close pipe.read
		return pipes[1]; //return pipe.write
	}
	else{ //Son
		close(pipes[1]); //close pipe.write
		char buf[16];
		sprintf(buf, "%d", pipes[0]); 
		execl("./async_controller.out", "./async_controller.out", buf, NULL);
		fprintf(stderr, "Exec error\n");
		exit(-2);
	}
}

int main(int argc, char** argv){
	char *str = NULL; size_t str_cap = 0, len = 0;
	int run_status = 0, usr_code = 0, sys_code = 0;
	cmd_t *cmd = NULL;
	int async_fd = start_ac();	

	while (!feof(stdin)){
		print_input_prompt(usr_code, sys_code);
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		char **parsed = parse_input(str);
		if (*parsed == NULL) continue;
		
		cmd = prepare_cmd(parsed, cmd);
		run_status = run_cmd(cmd, async_fd);
		free_parsed(parsed);
		
		if (run_status == EXIT_C) break;
		if (run_status == -1) {
			printf("Shell error\n");
			usr_code = 0; sys_code = 0;
		}
		else {
			parse_status(run_status, &usr_code, &sys_code);
		}
	}
	free(cmd);
	free(str);

	return 0;
}
