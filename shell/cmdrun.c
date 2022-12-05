#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "utils.h"
#include "cmdrun.h"

extern int silent;

void redirect(char ***args_p, cmd_t *cmd){
	char **arg_p = *args_p;
	if (*(arg_p + 1) == NULL){
		fprintf(stderr, "Not enough args\n");
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

int get_argc(char **args){
    int c = 0;
    for (char **arg_p = args; *arg_p; ++arg_p) ++c;
    return c;
}

int to_devnull(cmd_t *cmd, int mode){
	int fd;
	if (mode == 0){ //dev/null --> stdin
		fd = open("/dev/null", O_RDONLY);
	}
	else if (mode == 1){ //stdout --> /dev/null
		fd = open("/dev/null", O_WRONLY);
	}
	if (fd == -1){
		perror("Can't redirect to /dev/null");
		return -1;
	}
	if (mode == 0) cmd->inp_ph = fd;
	else if (mode == 1) cmd->out_ph = fd;
	
	return 0;
}


cmd_t *prepare_cmd(char **args){
    int argc = get_argc(args);
	cmd_t *cmd = calloc(1, sizeof(cmd_t));
    if (cmd == NULL) {
        fprintf(stderr, "Allocation error: %d\n", errno);
        return NULL;
    }
	cmd->path = args[0];
	cmd->args = calloc(argc + 1, sizeof(char*));
    if (cmd->args == NULL) {
        fprintf(stderr, "Allocation error: %d\n", errno);
        return NULL;
    }
	cmd->mode = CMD_DEFAULT;
		
	cmd->inp_ph = 0;
	cmd->out_ph = 1;
	cmd->err_ph = 2;

	cmd->fd_to_close = -1;
	cmd->father_fd_to_close[0] = -1;
	cmd->father_fd_to_close[1] = -1;
	
	char **arg_wr = cmd->args;
	int i = 0;
	for (char **arg_p = args; (*arg_p) && (i < argc); ++arg_p, ++i){
		if (strchr("<>", (*arg_p)[0])){
			redirect(&arg_p, cmd);
		}
		else if (((*arg_p)[0] == '&') && ((*arg_p)[1] != '&')){
			cmd->mode = CMD_ASYNC;
			to_devnull(cmd, 0);
		}
		else{
			*arg_wr = *arg_p;
			++arg_wr;
		}

	}
	
	cmd->argc = argc;	
	cmd->next = NULL;
	return cmd;
}

int prepare_conveyor(char **cmd_arg_p, cmd_t **head_ptr, cmd_t **cur_ptr){
    cmd_t *head = *head_ptr;
    cmd_t *cur = *cur_ptr;

    cmd_t *new_cmd = prepare_cmd(cmd_arg_p);
    if (new_cmd == NULL){
        fprintf(stderr, "Prepare cmd error\n");
        return -1;
    }
    new_cmd->mode = CMD_CONVEYOR;
    if (head == NULL){
        head = new_cmd;
    }
    else{
        // cur == NULL <==> head == NULL
        cur->next = new_cmd;
        //make pipe
        int fd[2]; //fd[0] = pipe.read, fd[1] = pipe.write
        if (pipe(fd) == -1){
            fprintf(stderr, "Pipe error: %d\n", errno);
            return -1;
        }
        cur->out_ph = fd[1];
        new_cmd->inp_ph = fd[0];
        cur->fd_to_close = fd[0];
        new_cmd->fd_to_close = fd[1];

        cur->father_fd_to_close[0] = fd[1];
        new_cmd->father_fd_to_close[0] = fd[0];
        new_cmd->father_fd_to_close[1] = fd[1];
    }

    cur = new_cmd;

    *head_ptr = head;
    *cur_ptr = cur;

    return 0;
}

int prepare_default(char **cmd_arg_p, cmd_t **head_ptr, cmd_t **cur_ptr, int next_mode){
    cmd_t *head = *head_ptr;
    cmd_t *cur = *cur_ptr;

    cmd_t *new_cmd = prepare_cmd(cmd_arg_p);
	if (new_cmd->mode != CMD_ASYNC) new_cmd->mode = next_mode;

    if (new_cmd == NULL){
        fprintf(stderr, "Prepare cmd error\n");
        return -1;
    }

    if (head == NULL){
        head = new_cmd;
    }
    else{
        cur->next = new_cmd;
    }

    cur = new_cmd;

    *head_ptr = head;
    *cur_ptr = cur;

    return 0;
}

cmd_t *prepare_cmd_seq(arg_seq_t *arg_seq){
	cmd_t *head = NULL, *cur = NULL;
    for (; arg_seq; arg_seq = arg_seq->next){
        if (arg_seq->next_mode == CMD_CONVEYOR) prepare_conveyor(arg_seq->args, &head, &cur);
        else{
            prepare_default(arg_seq->args, &head, &cur, arg_seq->next_mode);
        }
    }

    return head;
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
	size_t pid_c = async_queue->len;
	for (size_t i = 0; i < pid_c; ++i){
		pid_t pid = pop(async_queue);
		int status;
		pid_t rs = waitpid(pid, &status, WNOHANG);
		if (rs == 0){
			push_back(async_queue, pid);
		}
		else{
            int usr, sys;
            parse_status(status, &usr, &sys);
            if (!silent) printf("Process %d finished(usr:%d, sys:%d)\n", pid, usr, sys);
		}
	}
	return 0;
}

int run_cmd(cmd_t *cmd, queue_t *async_queue){
    if (!silent) printf("CMD mode: %d\n", cmd->mode);
	wait_async(async_queue);
	if (strncmp(cmd->path, "cd", 2ul) == 0){
		return cd(cmd->args[1]);
	}

	if (strcmp(cmd->path, "exit") == 0){
		return EXIT_C;
	}	

	pid_t pid = fork();
	if (pid == -1){
		fprintf(stderr, "Fork error: errno%d\n", errno);
		return -1;
	}
	if (pid){
		if (cmd->father_fd_to_close[0] != -1){
            if (!silent) printf("Father closing %d\n", cmd->father_fd_to_close[0]);
            close(cmd->father_fd_to_close[0]);
        }
		if (cmd->father_fd_to_close[1] != -1){
            if (!silent) printf("Father closing %d\n", cmd->father_fd_to_close[1]);
            close(cmd->father_fd_to_close[1]);
        }

		int status = 0;
		if (cmd->mode == CMD_ASYNC){
			push_back(async_queue, pid);
			return 0;
		}
		if (cmd->mode == CMD_CONVEYOR){
			if (cmd->next != NULL){
				run_cmd(cmd->next, async_queue);
			}
            if (!silent) printf("Waiting for %d\n", pid);
			waitpid(pid, &status, 0);
			return 0;
		}
		else{
			waitpid(pid, &status, 0);
			int usr_code, sys_code;
			parse_status(status, &usr_code, &sys_code);
            if (!silent) printf("Exit status(usr, sys): %d, %d\n", usr_code, sys_code);
            if (cmd->next != NULL){
                if (cmd->mode == CMD_DEFAULT)
                    run_cmd(cmd->next, async_queue);
                else if ((cmd->mode == CMD_ON_SUCCESS) && (usr_code == 0) && (sys_code == 0))
                    run_cmd(cmd->next, async_queue);
                else if ((cmd->mode == CMD_ON_ERROR) && ((usr_code != 0) || (sys_code != 0)))
                    run_cmd(cmd->next, async_queue);
            }
			return status;
		}
	}
	else{
		//run command
        signal(SIGINT, SIG_DFL);
        if (!silent) printf("Running %s\n", cmd->path);
		
		change_fd(0, cmd->inp_ph);	
		change_fd(1, cmd->out_ph);	
		change_fd(2, cmd->err_ph);
        if (cmd->fd_to_close != -1){
            if (!silent) printf("Process %d closing %d\n", getpid(), cmd->fd_to_close);
            close(cmd->fd_to_close);
        }

        if (cmd->path[0] == '('){
//            cmd->args[0] = strdup("./shell.out");

//            printf("SKOBKA!!!!:\n");
//            for (char **arg_p = cmd->args; *arg_p; ++arg_p){
//                printf("\t%s\n", *arg_p);
//            }

//            printf("\tfd[0] = %d\n", cmd->inp_ph);
//            printf("\tfd[1] = %d\n", cmd->out_ph);

            execvp("./shell.out", cmd->args);
        }
        else execvp(cmd->path, cmd->args);
		fprintf(stderr, "Execvp error: %d\n", errno);
		exit(-1);
	}
}

