#include "performConnection.h"
#include "util.h"
#include "sharedMemory.h"


#define BUF_SIZE 1024
#define GAMEKINDNAME "NMMorris\n"

static char formatbuf[18];
static char buffer[BUF_SIZE];
extern int fd[2];
extern bool gameover;


void printRcvLine(char* receivedData){
    for(int i = 0; i < (int)strlen(receivedData); i++){
        if(receivedData[i] != '\n'){
            printf("%c", receivedData[i]);
        } else {
            printf("\n");
            return;
        }
    }
}

void sendMsgToSrv(int socketfd, char* msg){
    int bytes_sent, slen;
    slen = strlen(msg); // string length
    bytes_sent = send(socketfd, msg, slen, 0);
    if (bytes_sent < 0){
        perror("Message not sent.\n");
        exit(EXIT_FAILURE);
    }
}

void receiveMsgFromSrv(int socketfd, char* buffer){
    // read message from  server into buffer
    int blen; // buffer length
    if ((blen = recv(socketfd, buffer, BUF_SIZE - 1, 0)) < 0){
        perror("No Message received.\n");
        exit(EXIT_FAILURE);
    }
    if (buffer[0] != '+'){
        printf("Gameserver Error.\n");
        if (strstr(buffer, "- No free player")){
            fprintf(stderr, "Player not available.\n");
        }
        /*
         * evtl noch mehr error messages einfügen
         *
         */

        //printRcvLine(buffer);
        exit(EXIT_FAILURE);
    }
}

void performPrologue(int network_socket, ServerInfo* server, PlayerInfo* player){
    memset(&formatbuf, '\0', 18);
    memset(&buffer, '\0', BUF_SIZE);
    


    // read initial message from server into buffer
    // S: + MNM Gameserver v2.3 accepting connections
    receiveMsgFromSrv(network_socket, buffer);
    printf("S: ");
    printf("%s", buffer);

    sendMsgToSrv(network_socket, "VERSION 2.3\n");
    printf("C: VERSION 2.3\n");

    // S: + Client version accepted - please send Game-ID to join...
    receiveMsgFromSrv(network_socket, buffer);
    printf("S: ");
    printf("%s", buffer);

    (void)strcat(formatbuf, "ID ");
    (void)strcat(formatbuf, server->gameID);
    (void)strcat(formatbuf, "\n");
    printf("C: ID %s\n", server->gameID);
    sendMsgToSrv(network_socket, formatbuf);
    memset(&formatbuf, '\0', 18);

    // S: + PLAYING NMMorris
    receiveMsgFromSrv(network_socket, buffer);
    printf("S: ");
    printRcvLine(buffer);
    if (strncmp(buffer,"+ PLAYING", 9)!= 0 || strncmp(buffer+10, GAMEKINDNAME, 9)!= 0){
        perror("Wrong Game Kind");
        exit(EXIT_FAILURE);
    }
    strcpy(server->gameName, buffer+10);

    // Number of Players
    if (player->playerID != -1){
        // if PlayerID was selected
        char pstr[2];
        snprintf(pstr, 2, "%d",player->playerID);
        (void)strcat(formatbuf, "PLAYER ");
        (void)strcat(formatbuf, pstr);
        (void)strcat(formatbuf, "\n");
        printf("C: PLAYER %d\n",player->playerID);
        sendMsgToSrv(network_socket, formatbuf);
        memset(&formatbuf, '\0', 18);

    } else {
        // if no PlayerID was selected
        sendMsgToSrv(network_socket, "PLAYER\n");
        puts("C: No Player selected.");
    }
    
    

    receiveMsgFromSrv(network_socket, buffer);
    
    char *copy = (char *)malloc(strlen(buffer) + 1);
    strcpy(copy, buffer);
    char *token;

    token = strtok(copy, "\n");

    while(token != NULL){
    	if (strstr(token,"+ YOU")){
        	strcpy(player->playerName, token+8);
        	player->ready = false;
            player->playerID = atoi(&token[6]);
            if (player->playerID == 0){
                player->ready = true;
                // you start the game
            }
            if (player->playerID){
                player->opponentID = 0;
            } else {
                player->opponentID = 1;
            }
        	printf("S: You are Player %d and your Name is: %s\n", player->playerID, player->playerName);
        }
     
    	if (strstr(token,"+ TOTAL")){
    		server->totalPlayers = atoi(token+8);
    		printf("S: Total players: %d\n", server->totalPlayers);
        }

    	token = strtok(NULL, "\n");

    }
    
    free(copy);
    memset(&formatbuf, '\0', 18);
    
    printf("********************Prologue Buffer*******************\n%s", buffer);
    printf("******************************************************\n");
 
}



void performMove(int network_socket, ServerInfo* server, PlayerInfo* player){
    player->playerCaptured = 0;
    (void)network_socket;
    // coordinates of game board
    // ring index 0,1,2 (most outer ring 0)
    // ring position (position in each ring 0-8)
    int ringIndex = 0;
    int ringPosition = 0;

    char *copy = (char *) malloc(strlen(buffer) + 1);
    strcpy(copy , buffer);
    char *token;
    
    // initialize Gameboard
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 8; j++){
            server->gameBoard[i][j] = -1;
        }
    }
    // init
    server->availableTokens = 0;
    token = strtok(copy , "\n");
    printf("\nInfo from server:\n");

    while(token != NULL){
        /// debug
        //printf("token: %s\n",token);
    	
    	if(strstr(token,"+ CAPTURE")){
    		server->captureTokens = atoi(&token[10]);
    		printf("Tokens to be captured: %d\n", atoi(&token[10]));
    	}

      	// process 'PIECE' responds only
      	if (strstr(token,"+ PIECE") && !strstr(token,"+ PIECELIST")){
    		//printf("+++ PIECE%c,%c %c \n",token[7],token[9],token[11]);
        	//printf("length token: %d\n",(int)strlen(token));	

        	// increment available token amount if position set to 'A'
        	if ((token[12] == '\0') && (token[11] == 'A')){
        		//printf("incrementing availableToken count %d plus1 \n",server->availableTokens);
        		server->availableTokens++;
        	}
            if ((token[12] == '\0') && (token[11] == 'C')){
                // check if player's stone has been captured
                if (((atoi(&token[7]) == 0) && (player->playerID == 0)) || ((atoi(&token[7]) == 1) && (player->playerID == 1))){
                    player->playerCaptured++;
                }
            }

        	// determine token coordinates
        	if (token[12] != '\0'){ 
        		switch(token[11]){
        			case 'A': ringIndex = 0; break;
        			case 'B': ringIndex = 1; break;
        			case 'C': ringIndex = 2; break;
        		}
        		ringPosition = atoi(&token[12]);
        		server->gameBoard[ringIndex][ringPosition] = atoi(&token[7]);
        	}
        }
        token = strtok(NULL, "\n");
    }
    
    printf("Still available tokens: %d\n\n", server->availableTokens);
    printf("Gameboard Info from Server:\n");
    printf("\n%s\n", buffer);
    //printf("# %d\n",server->gameBoard[0][0]);
    printf("**********\nGAMEBOARD\n**********\n");
    printGameBoard(server);
    //printf("# %d\n",server->gameBoard[0][0]);


    printf("C: THINKING\n");
    sendMsgToSrv(network_socket, "THINKING\n");
    memset(buffer, '\0', BUF_SIZE);
    // S: OKTHINK
    receiveMsgFromSrv(network_socket, buffer);
    printf("S: %s\n", buffer);
    memset(buffer, '\0', BUF_SIZE);

    // Signal an Thinker schicken
    server->thinkNeeded = true;
    kill(getppid(), SIGUSR1);


    // reading from pipe
    char pipeMsg[128];
    memset(pipeMsg, '\0', 128);
    int n;
    if((n = read(fd[0], pipeMsg, PIPE_BUF)) < 0){
        perror("Failed to read from Pipe.");
        exit(EXIT_FAILURE);
    }
    printf("C: %s", pipeMsg);
    sendMsgToSrv(network_socket, pipeMsg);



    memset(buffer, '\0', BUF_SIZE);

    receiveMsgFromSrv(network_socket, buffer);
    printf("S: %s", buffer);
    // Not sure if this really works, but the random error is definitely
    // due to the fact that the server sends sometimes +MOVEOK and the
    // buffer for +MOVE in one string!!!
    if (strstr(buffer, "+ MOVE ")){
        for(int i = 0; i < 20; i++){
            printf("***************----------------------*****************\n");
        }
        printf("MOVEOKBUFFER:[%s]\n", buffer);
        performMove(network_socket, server, player);
    } else {
        memset(buffer, '\0', BUF_SIZE);
    }
    free(copy);
	
}

void performConnection(int network_socket, ServerInfo* server, PlayerInfo* player){
    // here we use the last assigned buffer
    int blen;

    do {
	    if (strstr(buffer, "+ MOVE ")){
            // performMove including gameBoard parsing
            printf("************\nMove\n************\n");
            performMove(network_socket, server, player);

            
        } else if (strstr(buffer, "+ WAIT")){
            // C: OKWAIT
            printf("S: WAIT\n");
            printf("C: OKWAIT\n");
            sendMsgToSrv(network_socket, "OKWAIT\n");
            memset(buffer, '\0', BUF_SIZE); 

        } else if (strstr(buffer, "+ GAMEOVER")){
            printf("GAMEOVER!\n");
            gameover = true;

            // Parser für PlayerWON
            char *copy = (char *) malloc(strlen(buffer) + 1);
            strcpy(copy , buffer);
            char *token;

            token = strtok(copy , "\n");
            while(token != NULL){
                if (strstr(token, "+ PLAYER0WON")){
                    if (token[13] == 'Y'){
                        if (player->playerID == 0){
                            player->youWin = true;
                        } else {
                            player->opponentWin = true;
                        }
                    }
                }
                if (strstr(token, "+ PLAYER1WON")){
                    if (token[13] == 'Y'){
                        if (player->playerID == 1){
                            player->youWin = true;
                        } else {
                            player->opponentWin = true;
                        }
                    }
                }
                token = strtok(NULL, "\n");
            }
            if (player->youWin == true && player->opponentWin == false){
                printf("**********\nCongratulations, You Win!!!\n**********\n");
            } else if (player->youWin == false && player->opponentWin == true){
                printf("**********\nSorry, You Loose.\nBetter luck next time...\n**********\n");
            } else if (player->youWin == true && player->opponentWin == true){
                printf("**********\nIt's a Draw.\nNobody won...\n**********\n");
            }
            memset(buffer, '\0', BUF_SIZE);
            free(copy);

        } else {
            printf("No action possible.\n");
            printf("It's not your turn.\n");
            sleep(2);
            printf("Buffer: \n************\n%s\n************\n", buffer);
            memset(buffer, '\0', BUF_SIZE);
            // exit(EXIT_FAILURE);
        }

    } while (((blen = recv(network_socket, buffer, BUF_SIZE, 0)) >= 0) && gameover == false);
}












