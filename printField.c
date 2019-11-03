#include "printField.h"

void printOddLine(uint8_t line){
    if(line == 1 || line == 11){
        puts("|           |           |");
    } else if(line == 3 || line == 9){
        puts("|   |       |       |   |");

    } else if(line == 5 || line == 7){
        puts("|   |   |       |   |   |"); 
    } else {
        perror("Wrong odd line input");
    }
}
void printEvenLine(uint8_t stoneID1, uint8_t stoneID2, uint8_t stoneID3, uint8_t line){
    char ch1 = '+';
    char ch2 = '+';
    char ch3 = '+';
    if (stoneID1 == 1) {
        ch1 = '1';
    } else if (stoneID1 == 2){
        ch1 = '2';
    }
    if (stoneID2 == 1) {
        ch2 = '1';
    } else if (stoneID2 == 2){
        ch2 = '2';
    }
    if (stoneID3 == 1) {
        ch3 = '1';
    } else if (stoneID3 == 2){
        ch3 = '2';
    }

    if (line % 2 != 0){
        puts("Line is not even");
    } else if (line == 6){
        printf("%c---%c---%c", ch1, ch2, ch3);
    } else {
        if (line == 0 || line == 12){
            printf("%c-----------%c-----------%c\n", ch1, ch2, ch3);
        } else if(line == 2 || line == 10){
            printf("|   %c-------%c-------%c   |\n", ch1, ch2, ch3);
        } else if(line == 4 || line == 8){
            printf("|   |   %c---%c---%c   |   |\n", ch1, ch2, ch3);
        } else {
            puts("Wrong even line input");
            
        }
    }
}

void printField(uint8_t *stoneID){
    int j = 0;
    for(int i = 0; i <= 12; i++){
        if (i % 2 == 0){
            if (i != 6){
                printEvenLine(stoneID[j], stoneID[j+1], stoneID[j+2], i);
                j += 3;
            } else {
                printEvenLine(stoneID[j], stoneID[j+1], stoneID[j+2], i);
                printf("       ");
                printEvenLine(stoneID[j+3], stoneID[j+4], stoneID[j+5], i);
                printf("\n");
                j += 6;
            }
        } else {
            printOddLine(i);
        }
    }
}

