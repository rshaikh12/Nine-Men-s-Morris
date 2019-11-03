#include "config.h"
#include "util.h"

#define BUFSIZE 1024
#define SEPARATOR "="



int getConfig(char* filename, config* confStruct){
    FILE* file = fopen(filename, "r");
    bool hn = false;
    bool pt = false;
    bool gt = false;
    if(file != NULL){
        char line[BUFSIZE];
    	while(fgets(line, sizeof(line), file) != NULL){
    		char* pure_line;
            char* cfline;
            pure_line = removeSpaces((char*) line);
    		cfline = strstr(pure_line, SEPARATOR);
    		cfline = cfline + strlen(SEPARATOR);
            char* type;
            if((type = strstr((char*)pure_line, "hostname")) != NULL){
                memset(confStruct->hostName,'\0',sizeof(char *) * strlen(cfline));
                memcpy(confStruct->hostName, cfline, strlen(cfline));
                hn = true;
            } else if((type = strstr((char*)pure_line, "port")) != NULL){
                memset(confStruct->port,'\0',sizeof(char *) * strlen(cfline));
                memcpy(confStruct->port, cfline, strlen(cfline));
                pt = true;
            } else if ((type = strstr((char*)pure_line, "gametype")) != NULL){
                memset(confStruct->gameType,'\0',sizeof(char *) * strlen(cfline));
                memcpy(confStruct->gameType, cfline, strlen(cfline));
                gt = true;
            }
            free(pure_line);
    	}
    	fclose(file);
    } else {
        // Reading from Config not possible
        return -1;
    }
    if (!hn || !pt || !gt){
        // Config Information incomplete
        return -1;
    }
    return 0;	
}
