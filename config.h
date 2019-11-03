#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
 char hostName[1024];
 //u_short port;
 //unsigned short port;
 char port[1024];
 char gameType[1024];
}config;


int getConfig(char*, config*);
