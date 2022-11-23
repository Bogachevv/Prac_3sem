#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "lex.h"
#include "parser.h"
#include "cmdrun.h"
#include "queue.h"

int silent = 0;

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

int disable_sigint(){
    __sighandler_t prev = signal(SIGINT, SIG_IGN);
    if (prev == SIG_ERR){
        fprintf(stderr, "signal() error\n");
        return -1;
    }
    return 0;
}

int process_parsing(char **parsed, int *run_status, int *usr_code, int *sys_code, queue_t *async_queue){
    cmd_t *cmd = NULL;

    arg_seq_t *arg_seq = parse_args(parsed);
    cmd = prepare_cmd_seq(arg_seq);
    *run_status = run_cmd(cmd, async_queue);
    free_arg_seq(arg_seq);
    free_cmd(cmd);
    free_parsed(parsed);

    if (*run_status == EXIT_C) return EXIT_C;
    if (*run_status == -1) {
        fprintf(stderr, "Shell error\n");
        *usr_code = 0; *sys_code = 0;
    }
    else {
        parse_status(*run_status, usr_code, sys_code);
    }

    return 0;
}

int main(int argc, char** argv){
    disable_sigint();

	char *str = NULL; size_t str_cap = 0, len;
	int run_status, usr_code = 0, sys_code = 0;
	queue_t async_queue;
	init(&async_queue);

    if (argc > 1){
        silent = 1;
        char **parsed = calloc(argc, sizeof(char*));
        for (int i = 0; i < argc - 1; ++i){
            parsed[i] = strdup(argv[i+1]);
        }
        process_parsing(parsed, &run_status, &usr_code, &sys_code, &async_queue);
        free_queue(&async_queue);

        exit(0);
    }

	while (!feof(stdin)){
		print_input_prompt(usr_code, sys_code);
		len = getline(&str, &str_cap, stdin);
		if (len == -1) break;
		char **parsed = parse_input(str);
		if (*parsed == NULL) {
            free(parsed);
            continue;
        }

        int rs = process_parsing(parsed, &run_status, &usr_code, &sys_code, &async_queue);
        if (rs == EXIT_C) break;
	}
	free(str);
    free_queue(&async_queue);

	return 0;
}
