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

	int shm = shmget(key, SHM_SIZE, 0666 | IPC_CREAT | IPC_EXCL);
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

	int sem = semget(key, 2, 0666 | IPC_CREAT | IPC_EXCL);
	if (sem == -1){
		perror("semget error");
		shmdt(shm_addr);
		shmctl(shm, IPC_RMID, NULL);
		exit(-1);
	}
	semctl(sem, 1, SETVAL, SHM_SIZE - 1);

	struct sembuf sem_down = {0, -1, 0};
	struct sembuf sem_up = {1, 1, 0};

	int read_ptr = 0;

	for (;;){
		if (semop(sem, &sem_down, 1) == -1){
			perror("Semop error");
			break;
		}

		char byte = shm_addr[read_ptr];
		read_ptr = (read_ptr + 1) % SHM_SIZE;		
		if (byte == -1) break; // EOF

		if (semop(sem, &sem_up, 1) == -1){
			perror("Semop error");
			break;
		}

		printf("%c", byte);
		//printf("concumer: %c\n", byte); 
		//printf("concumer: read_ptr = %d\n", read_ptr);
	}

	shmdt(shm_addr);
	shmctl(shm, IPC_RMID, NULL);
	semctl(sem, 0, IPC_RMID, 0);

	return 0;
}
