#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int get_ch_mode(const char *ch_p){
    if ((ch_p == NULL) || (*ch_p == 0)) return 0;
    if (*ch_p == '|'){
        if (*(ch_p + 1) == '|') return 1;
        else return 2;
    }
    else if (*ch_p == '&'){
        if (*(ch_p + 1) == '&') return 3;
        else return 4;
    }
    else if (*ch_p == ';') return 5;

    return -1;
}

arg_seq_t *build_cmd_seq(char **args, int argc){
    arg_seq_t *arg_seq = calloc(1, sizeof(arg_seq_t));
    if (arg_seq == NULL){
        fprintf(stderr, "Calloc error\n");
        return NULL;
    }
    arg_seq->args = calloc(argc + 1, sizeof(char*));
    if (arg_seq->args == NULL){
        fprintf(stderr, "Calloc error\n");
        free(arg_seq);
        return NULL;
    }
    for (int i = 0; i < argc; ++i){
        arg_seq->args[i] = args[i];
    }

    return arg_seq;
}

arg_seq_t *parse_args(char **args){
	arg_seq_t *head = NULL, *cur = NULL;
    int argc = 0;
    char **cmd_arg_p = args;
    int mode = 0; // 0 - default, 1 - conveyor
    for(char **arg_p =args; ; ++arg_p) {
        int ch_mode = get_ch_mode(*arg_p);
        if ((ch_mode == -1) || (ch_mode == 4)) {
            ++argc;
            continue;
        }

        arg_seq_t *new_arg_seq = build_cmd_seq(cmd_arg_p, argc);
        if (new_arg_seq == NULL){
            printf("Parser error\n");
            return NULL;
        }
        argc = 0;
        cmd_arg_p = arg_p + 1;

        switch (ch_mode) {
            case 0:
                if (mode == 1) new_arg_seq->next_mode = CMD_CONVEYOR;
                else new_arg_seq->next_mode = CMD_DEFAULT;
                break;
            case 1:
                new_arg_seq->next_mode = CMD_ON_ERROR;
                break;
            case 2:
                new_arg_seq->next_mode = CMD_CONVEYOR;
                break;
            case 3:
                new_arg_seq->next_mode = CMD_ON_SUCCESS;
                break;
            case 5:
                new_arg_seq->next_mode = CMD_DEFAULT;
                break;
            default:
                fprintf(stderr, "Parser default case\n");
                break;
        }
        mode = (ch_mode == 2);

        if (head == NULL){
            head = new_arg_seq;
            cur = new_arg_seq;
        }else{
            cur->next = new_arg_seq;
            cur = new_arg_seq;
        }

        if (*arg_p == NULL) break;
    }


    return head;
}

void free_arg_seq(arg_seq_t *arg_seq){
    if (arg_seq == NULL) return;
    free_arg_seq(arg_seq->next);
    free(arg_seq->args);
    free(arg_seq);
}