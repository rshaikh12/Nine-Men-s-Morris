#ifndef SHARED_MEMORY
#define SHARED_MEMORY

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#define BUFFER 2048


typedef struct {
	char gameID[14];
	char gameName[BUFFER];
	int totalPlayers;
	int availableTokens;
	int captureTokens;
	pid_t pid_connector; 
	pid_t pid_thinker;
    bool thinkNeeded;
    char gameBoard[3][8];
} ServerInfo; 

typedef struct {
	int playerID;
    int opponentID;
	char playerName[BUFFER];
    char opponentName[BUFFER];
    int playerCaptured;
    bool youWin;
    bool opponentWin;
	bool ready;
} PlayerInfo;

int createSHM(size_t size);

void *attachSHM(int shmID);

void detachSHM(void *shmdata);


#endif
