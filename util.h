#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "sharedMemory.h"
#include "thinker.h"


char* removeSpaces(char*);

char* getNthLine(char*, char**, int);

void printGameBoard(ServerInfo*);

void sigHandle(int);

void printIntroScreen();
