#define EXIT_C -2

#define CMD_ASYNC -1
#define CMD_DEFAULT 0
#define CMD_ON_SUCCES 1
#define CMD_ON_ERROR 2
#define CMD_CONTROLLER 3

typedef struct cmd{
	char *path;
	char **args;
	int argc;
	int inp_ph, out_ph, err_ph;
	struct cmd *next;
	int mode;
} cmd_t;

cmd_t *prepare_cmd(char **args, cmd_t *cmd);

int change_fd(int old_fd, int new_fd);

void parse_status(int status, int *usr_code, int *sys_code);

int run_cmd(const cmd_t *cmd, int async_fd);

