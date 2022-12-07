#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/fcntl.h>

#include "settings.h"
#include "ipc_utils.h"

int main(int argc, char **argv){
	key_t key = generate_key();
	if (key == -1) exit(-1);

	int shm = shmget(key, SHM_SIZE, 0666);
	if (shm == -1){
		perror("shmget error");
		exit(-1);
	}	

	char *shm_addr = shmat(shm, NULL, 0);
	if (shm_addr == NULL){
		perror("semat error");
		shmctl(shm, IPC_RMID, NULL);
		exit(-1);
	}

	int sem = semget(key, 2, 0666);
	if (sem == -1){
		perror("semget error");
		shmdt(shm_addr);
		shmctl(shm, IPC_RMID, NULL);
		exit(-1);
	}

	struct sembuf sem_down = {1, -1, 0};
	struct sembuf sem_up = {0, 1, 0};

	int write_ptr = 0;

	for (;;) {
		char ch = getchar();

		if (semop(sem, &sem_down, 1) == -1){
			perror("Semop error");
			break;
		}

		shm_addr[write_ptr] = ch;
		write_ptr = (write_ptr + 1) % SHM_SIZE;
		//printf("producer: write_ptr = %d\n", write_ptr);
			
		if (semop(sem, &sem_up, 1) == -1){
			perror("Semop error");
			break;
		}

		if (ch == -1) break; //EOF
	}

	shmdt(shm_addr);
	return 0;
}
