#ifndef Connect
#define Connect

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include "sharedMemory.h"

#include <sys/wait.h>
#include <fcntl.h> 
#include <limits.h>

char* formatString(char*, char*);

void printRcvLine(char*);

void sendMsgToSrv(int, char*);

void receiveMsgFromSrv(int, char*);

void performPrologue(int, ServerInfo*, PlayerInfo*);

void performMove(int, ServerInfo*, PlayerInfo*);

void performConnection(int, ServerInfo*, PlayerInfo*);

#endif


