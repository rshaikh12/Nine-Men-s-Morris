#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include "performConnection.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "config.h"
#include "util.h"
#include "sharedMemory.h"
#include "thinker.h"


#define CONFIGFILE "client.conf"

// Globale SHM-Variablen
ServerInfo* shm_server;
PlayerInfo* shm_player;

// Globaler File-Deskriptor
int fd[2];
bool gameover = false;

int main(int argc, char* argv[]){

    char* gameID;
    gameID = NULL;
    int playerID = -1;
    int option;
    ServerInfo server;
    PlayerInfo player;


    if (argc < 3){
        perror("Zu wenige Parameter.\n");
        exit(EXIT_FAILURE);
    }
    while((option = getopt(argc, argv, "g:p:")) != -1){
        switch(option){
            case 'g':
                gameID = optarg;
                if (strlen(gameID) != 13){
                    puts("Game ID muss 13 Zeichen lang sein.");
                    exit(EXIT_FAILURE);
                }
                memset(server.gameID, 0, sizeof(server.gameID));
                memcpy(server.gameID, gameID, strlen(gameID));
                break;
            case 'p':
                playerID = atoi(optarg) - 1;
                if (playerID < 0 || playerID > 1){
                    puts("PlayerID muss 1 oder 2 sein!");
                    printf("PlayerID: %d\n", playerID + 1);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                perror("Parameter <g> fehlt.\n");
                exit(EXIT_FAILURE);
        }
    }
    server.thinkNeeded = false;
    player.playerID = playerID;
    player.youWin = false;
    player.opponentWin = false;
    player.playerCaptured = 0;
    printIntroScreen();

    
    // initialize GameBoard
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 8; j++){
            server.gameBoard[i][j] = -1;
        }
    }

    // read and populate config file
    config configStruct;
    int cst;
    if ((cst = getConfig(CONFIGFILE, &configStruct)) < 0){
        perror("Reading from config not possible.\n");
        exit(EXIT_FAILURE);
    }
    
    int cport = atoi(configStruct.port);
    
    // output of hostname from the configfile for debug purposes
    printf("Port: %i\nHostname: %s\nGametype: %s\n", cport, configStruct.hostName, configStruct.gameType);


    // create socket with following arguments:
    // 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case)
    int network_socket;
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        perror("Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    // specify address for the socket
    struct sockaddr_in server_address;
    struct hostent *host_info;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(cport);

    memset(server_address.sin_zero, '\0', sizeof server_address.sin_zero);

    // resolve the IP-Adress from the Hostname
    if ((host_info = gethostbyname(configStruct.hostName)) == NULL){
        perror("DNS Fail: Resolve of Hostname not possible.\n");
        exit(EXIT_FAILURE);
    }

    // copy information contained in host_info to server_address
    memcpy((char *) &server_address.sin_addr, host_info->h_addr_list[0], host_info->h_length);

    // connect to Server
    if (connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0){
        perror("No Server Connection possible.\n");
        exit(EXIT_FAILURE);
    }
    printf("Connection successfully established!\n");

    // initialize Shared Memory
    int shmID  = createSHM(sizeof(ServerInfo));
    int shmID_player = createSHM(sizeof(PlayerInfo));


    // Prologue of Server Protocol
    performPrologue(network_socket, &server, &player);

    pid_t pid;
	if (pipe(fd) < 0) {
        perror ("Fehler beim Einrichten der Pipe.");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Fehler bei fork().\n");
        exit(0);

    } else if (pid == 0) {
        //Kindprozess bzw Connector
		// Schreibseite der Pipe schliessen
		close(fd[1]);
        puts("This is Connector.\nNow connecting...");
        shm_server = (ServerInfo *) attachSHM(shmID);
        shm_player = (PlayerInfo *) attachSHM(shmID_player);
        memcpy(shm_server, &server, sizeof(ServerInfo));
        memcpy(shm_player, &player, sizeof(PlayerInfo));
        shm_server->pid_connector = getpid();
        performConnection(network_socket, shm_server, shm_player);

        //after work is done
        detachSHM(shm_server);
        detachSHM(shm_player);
        exit(1);

    } else {
        // Elternprozess bzw. Thinker
		// Leseseite der Pipe schliessen
		close(fd[0]);
        puts("This is Thinker.");
        shm_server = (ServerInfo *) attachSHM(shmID);
        shm_player = (PlayerInfo *) attachSHM(shmID_player);
        shm_server->pid_thinker = getpid();
        shm_server->thinkNeeded = false;


        // get Signal from Connector
        if(signal(SIGUSR1, sigHandle) == SIG_ERR){
            perror("Error while receiving Signal.\n");
        }

        // hier while loop implementieren welcher wartet bis think() ausgeführt wurde
        // sleep() wird benötigt, damit das SHM nicht zu früh detached wird!
        // kann natürlich später weggelassen werden!
        // sleep(2);
        
        // after work is done
        wait(NULL);
        detachSHM(shm_server);
        detachSHM(shm_player);
        shmctl(shmID, IPC_RMID, 0);
        shmctl(shmID_player, IPC_RMID, 0);
        //exit(2);
    }
    close(network_socket);
    printf("Socket closed.\n");
    printf("Game finished. Goodbye.\n");
    return 0;
}







