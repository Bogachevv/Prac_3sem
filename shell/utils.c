#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "utils.h"

char *cd(const char *path){
	char old_path[PATH_MAX];
	if (get_
	int res = chdir(path);
	return (res == 0) ? old_path : NULL;
}

