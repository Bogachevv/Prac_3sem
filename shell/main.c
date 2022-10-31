#include <stdio.h>
#include <stdlib.h>
#include "lex.h"

void free_parsed(char **parsed){
	for (char **str_ptr = parsed; *str_ptr; ++str_ptr){ 
		free(*str_ptr);
	}
	free(parsed);
}


int run_cmd(char **args){
	char *cmd = args[0];
	if (strncmp(cmd, "cd", 2) == ){
		char *pwd = args[1];
		if (pwd == NULL) pwd = getenv("HOME");
		int status = chdir(pwd);
		if (status == -1){
			//error handling
			switch (errno){
				case EACCES:
					printf("Not enougth permissions to acces %s\n", pwd);
					break;
				case EFAULT:
					printf("Incorrect path: %s\n", pwd);
					break;
				case ENAMETOOLONG:
					printf("Path is too long:\n\tPath len: %d\tMax path len: %d", strlen(path), PATH_MAX);
					break;
				default:
					printf("Unexpected error: %d\n", errno);
			}
		}
	}

}


int main(int argc, char** argv){
	char *str = NULL; size_t str_cap = 0, len = 0;
	while (!feof(stdin)){
		printf(">>");
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		char **parsed = parse_input(str);
		int run_status = run_cmd(parsed);
		free_parsed(parsed);
	}
	free(str);
}
