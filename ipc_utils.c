#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/fcntl.h>
#include <stdio.h>

#include "ipc_utils.h"

key_t generate_key(){
	key_t key = ftok("sync.txt", SYNC_KEY);
	if (key == -1){
		perror("Ftok error");
	}

	return key;
}
