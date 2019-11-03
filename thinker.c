#include "thinker.h"

extern ServerInfo* shm_server;
extern PlayerInfo* shm_player;
extern int fd[2];


void think(){
    int ringIdxFrom = 0;
    int ringIdxTo = 0;
    int ringPosFrom = 0;
    int ringPosTo = 0;
    int seed = 0;
    shm_server->thinkNeeded = false;

    bool posFound = false;

    char pstr[2];
    char rpfstr[2];
    char rptstr[2];

    int randomRing;
    int randomField;

    char* sendString = (char*) calloc(128, sizeof(char));


    if (shm_server->availableTokens > 0){
        // stone placing phase
        if (shm_server->captureTokens < 1){
            // no stones to be captured
            // nested loop iterating over gameBoard and finding empty space to put the stone
            for(int i = 0; i < 3; i++){
                if (!posFound){
                    for(int j = 0; j < 8; j++){
                        if (!posFound){
                            if(shm_server->gameBoard[i][j] == -1){
                                ringIdxFrom = i;
                                ringPosFrom = j;
                                posFound = true;
                            }
                        }
                    }
                }
            }
        } else {
            // you made a mill last move
            // an enemy stone needs to be captured
            printf("Mill! Capturing enemy stone...\n");
            for(int i = 0; i < 3; i++){
                if (!posFound){
                    for(int j = 0; j < 8; j++){
                        if (!posFound){
                            if(shm_server->gameBoard[i][j] == shm_player->opponentID){
                                ringIdxFrom = i;
                                ringPosFrom = j;
                                posFound = true;
                            }       
                        }
                    }
                }
            }
        }

        if (ringIdxFrom == 0){
            (void)strcat(sendString, "PLAY A");
        } else if (ringIdxFrom == 1){
            (void)strcat(sendString, "PLAY B");
        } else if (ringIdxFrom == 2){
            (void)strcat(sendString, "PLAY C");
        }
        // convert ring position to string
        snprintf(pstr, 2, "%d", ringPosFrom);

        (void)strcat(sendString, pstr);
        (void)strcat(sendString, "\n");

    } else {
        // moving phase
        // no stones left to place on the board
        // iterate over game board to find field to move to
        if (shm_server->captureTokens > 0){
            // you made a mill last move
            // an enemy stone needs to be captured
            printf("Mill! Capturing enemy stone...\n");
            for(int i = 0; i < 3; i++){
                if (!posFound){
                    for(int j = 0; j < 8; j++){
                        if (!posFound){
                            if(shm_server->gameBoard[i][j] == shm_player->opponentID){
                                ringIdxFrom = i;
                                ringPosFrom = j;
                                posFound = true;
                            }       
                        }
                    }
                }
            }
            if (ringIdxFrom == 0){
            (void)strcat(sendString, "PLAY A");
            } else if (ringIdxFrom == 1){
                (void)strcat(sendString, "PLAY B");
            } else if (ringIdxFrom == 2){
                (void)strcat(sendString, "PLAY C");
            }
            // convert ring position to string
            snprintf(pstr, 2, "%d", ringPosFrom);

            (void)strcat(sendString, pstr);
            (void)strcat(sendString, "\n");
            printf("sendString: %s",sendString);

        } else {
            // no stones to capture
            for (int i = 0; i < 3; i++){
                if (!posFound){
                    for (int j = 0; j < 8; j++){
                        if (!posFound){
                            if (shm_server->gameBoard[i][j] == shm_player->playerID){ //field with a stone of yours
                                ringIdxFrom = i;
                                ringPosFrom = j;
                                if (shm_player->playerCaptured >= 6){
                                    // if you have 3 or less stones left
                                    while (!posFound){
                                        srand(time(NULL) + seed);
                                        randomRing = rand() % 3;
                                        srand(randomRing + seed);
                                        randomField = rand() % 8;
                                        if (shm_server->gameBoard[randomRing][randomField] == -1){
                                            ringIdxTo = randomRing;
                                            ringPosTo = randomField;
                                            posFound = true;
                                        }
                                        seed++;
                                    }
                                } else {
                                    // if you have more than 3 stones left
                                    //check neighbour fields
                                    if (j % 2 == 0){ //edge fields
                                        ringIdxTo = i;
                                        if (j == 0){
                                            // beginning of array
                                            if (shm_server->gameBoard[i][7] == -1){
                                                ringPosTo = 7;
                                                posFound = true;
                                            } else if (shm_server->gameBoard[i][j+1] == -1){
                                                ringPosTo = j+1;
                                                posFound = true;
                                            }
                                        } else {
                                            if (shm_server->gameBoard[i][j+1] == -1){
                                                ringPosTo = j+1;
                                                posFound = true;
                                            } else if (shm_server->gameBoard[i][j-1] == -1){
                                                ringPosTo = j-1;
                                                posFound = true;
                                            }
                                        }
                                    } else { // middle fields
                                        if (i == 0){
                                            // outer ring
                                            if (j > 6){
                                                // end of array
                                                if (shm_server->gameBoard[i][0] == -1){
                                                    ringPosTo = 0;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i+1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i+1;
                                                    posFound = true;
                                                }
                                            } else {
                                                if (shm_server->gameBoard[i][j+1] == -1){
                                                    ringPosTo = j+1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i+1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i+1;
                                                    posFound = true;
                                                }
                                            }
                                        } else if (i == 1){
                                            // middle ring
                                            if (j > 6){
                                                // end of array
                                                if (shm_server->gameBoard[i-1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i-1;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][0] == -1){
                                                    ringPosTo = 0;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i+1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i+1;
                                                    posFound = true;
                                                }

                                            } else {
                                                if (shm_server->gameBoard[i-1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i-1;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j+1] == -1){
                                                    ringPosTo = j+1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i+1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i+1;
                                                    posFound = true;
                                                }
                                            }
                                        } else {
                                            // inner ring
                                            if (j > 6){
                                                // end of array
                                                if (shm_server->gameBoard[i][0] == -1){
                                                    ringPosTo = 0;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i-1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i-1;
                                                    posFound = true;
                                                }
                                            } else {
                                                if (shm_server->gameBoard[i][j+1] == -1){
                                                    ringPosTo = j+1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i][j-1] == -1){
                                                    ringPosTo = j-1;
                                                    ringIdxTo = i;
                                                    posFound = true;
                                                } else if (shm_server->gameBoard[i-1][j] == -1){
                                                    ringPosTo = j;
                                                    ringIdxTo = i-1;
                                                    posFound = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }     
                }  
            }
        if (ringIdxFrom == 0){
            (void)strcat(sendString, "PLAY A");
        } else if (ringIdxFrom == 1){
            (void)strcat(sendString, "PLAY B");
        } else if (ringIdxFrom == 2){
            (void)strcat(sendString, "PLAY C");
        }
        // convert initial ring position to string
        snprintf(rpfstr, 2, "%d", ringPosFrom);

        (void)strcat(sendString, rpfstr);

        if (ringIdxTo == 0){
            (void)strcat(sendString, ":A");
        } else if (ringIdxTo == 1){
            (void)strcat(sendString, ":B");
        } else if (ringIdxTo == 2){
            (void)strcat(sendString, ":C");
        }

        // convert next position to string
        snprintf(rptstr, 2, "%d", ringPosTo);

        (void)strcat(sendString, rptstr);

        (void)strcat(sendString, "\n");


        // for debugging purposes, please remove if errors occur
        (void)strcat(sendString, "\0");
        }
    }
    

    int length = strlen(sendString);
    
    if ((write (fd[1], sendString, length)) != length) { // In Schreibseite schreiben
        perror ("Fehler bei write().");
        exit (EXIT_FAILURE);
    }
    memset(sendString, '\0', 128);
    free(sendString);
}



/*  ToDo:
 *
 *
 *      sleep(2) in sysprakClient -> Thinker muss "richtig" implementiert werden
 *          -> Dauerschleife implementieren, welche dann reagiert wenn ein Signal kommt, und sonst nichts macht
 *          reicht es nicht auch, wenn parent Prozess mit wait(NULL) auf child wartet? 
 *
 *
 */







