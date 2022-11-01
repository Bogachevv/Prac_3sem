#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "utils.h"


int cd(char *pwd){
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
				printf("Path is too long:\n\tPath len: %ld\tMax path len: %d", strlen(pwd), PATH_MAX);
				break;
			default:
				printf("Unexpected error: %d\n", errno);
		}
	}

	return status;
}
