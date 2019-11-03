
CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=gnu99 -g
OBJ = performConnection.o config.o sysprak-client.o util.o sharedMemory.o thinker.o

all: sysprak-client

sysprak-client: $(OBJ)
	$(CC) $(CFLAGS) -o sysprak-client $(OBJ)
    
%.o: %.c
	$(CC) $(CFLAGS) -c $<   

clean:
	rm -rf *.o *.a *.dSYM sysprak-client 

play: sysprak-client
	./sysprak-client -g $(GAME_ID) -p $(PLAYER)
	
valgrind: sysprak-client
	valgrind -v --track-origins=yes --leak-check=full --trace-children=yes --show-reachable=yes ./$< -g $(GAME_ID) -p $(PLAYER)
