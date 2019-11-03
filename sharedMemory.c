#include "sharedMemory.h"



int createSHM(size_t size){
	int shmID = -1;
	if((shmID = shmget(IPC_PRIVATE, size, IPC_CREAT| SHM_R | SHM_W)) == -1){
		perror("Could not create shared memory segment");
		exit(EXIT_FAILURE);
	}
	return shmID;
}

void *attachSHM(int shmID){
	void *shmdata = shmat(shmID, NULL, 0);
	if (shmdata == (void*) -1){
		perror("Could not attach shared memory segment");
		exit(EXIT_FAILURE);
	}
	return shmdata;
}

void detachSHM(void *shmdata){
	int del = 0;
	del = shmdt(shmdata);
	if(del == -1){
		perror("shared memory segment could not be detached!");
		exit(EXIT_FAILURE);
	}
}
