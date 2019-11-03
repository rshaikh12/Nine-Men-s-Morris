#include "util.h"
#include "sharedMemory.h"


char* removeSpaces(char* source){
    int length = (int) strlen(source);
    int isSpace;
    int j = 0;
    char* dest = (char*) calloc(length, sizeof(char));
    for(int i = 0; i < length; i++){
        if ((isSpace = isspace(source[i])) == 0){
            dest[j] = source[i];
            j++;
        }
    }
    return dest;
}


void printGameBoard(ServerInfo* gameInfo) {

	/*
	 * initial values:
	 * -1 : token not set (set to 0 on gameboard)
	 * 0  : player 0      (set to 1 on gameboard)
	 * 1  : player 1      (set to 2 on gameboard)
	 *
	 * each of above values get incremented by 1 
	 * to achieve a better visualisation on the 
	 * gameboard (see below addition of 1 to array values)
	 */

	
        printf("%i--------------%i--------------%i\n",    gameInfo->gameBoard[0][0]+1, gameInfo->gameBoard[0][1]+1, gameInfo->gameBoard[0][2]+1);
        printf("|              |              |\n");
        printf("|    %i---------%i---------%i    |\n",    gameInfo->gameBoard[1][0]+1, gameInfo->gameBoard[1][1]+1, gameInfo->gameBoard[1][2]+1);
        printf("|    |         |         |    |\n");
        printf("|    |    %i----%i----%i    |    |\n",    gameInfo->gameBoard[2][0]+1, gameInfo->gameBoard[2][1]+1, gameInfo->gameBoard[2][2]+1);
        printf("|    |    |         |    |    |\n");
        printf("%i----%i----%i         %i----%i----%i\n", gameInfo->gameBoard[0][7]+1, gameInfo->gameBoard[1][7]+1, gameInfo->gameBoard[2][7]+1, 
                                                          gameInfo->gameBoard[2][3]+1, gameInfo->gameBoard[1][3]+1, gameInfo->gameBoard[0][3]+1);
        printf("|    |    |         |    |    |\n");
        printf("|    |    %i----%i----%i    |    |\n",    gameInfo->gameBoard[2][6]+1, gameInfo->gameBoard[2][5]+1, gameInfo->gameBoard[2][4]+1);
        printf("|    |         |         |    |\n");
        printf("|    %i---------%i---------%i    |\n",    gameInfo->gameBoard[1][6]+1, gameInfo->gameBoard[1][5]+1, gameInfo->gameBoard[1][4]+1);
        printf("|              |              |\n");
        printf("%i--------------%i--------------%i\n",    gameInfo->gameBoard[0][6]+1, gameInfo->gameBoard[0][5]+1, gameInfo->gameBoard[0][4]+1);
    
}


// Signal-Handler for the connector
void sigHandle(int sig){
    if(sig == SIGUSR1){
        // printf("SIGUSR1 wurde ausgelöst\n");
        // hier flag setzen und think() in sysprak-client aufrufen
        think();
    }
}



void printIntroScreen(){
    puts("\033c"); // Clear Terminal Screen
    puts("***********************");
    puts("*                     *");
    puts("*  Nine Man's Morris  *");
    puts("*                     *");
    puts("***********************");
    printf("\n");
}

/*
    Server buffer zeilenweise lesen(read until newline)
*/
