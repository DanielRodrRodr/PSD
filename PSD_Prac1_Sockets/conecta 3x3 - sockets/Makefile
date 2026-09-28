CC = gcc
CFLAGS = -c -g -Wall

EXECS = clientGame serverGame

all: game.o $(EXECS)

game.o: game.c
	$(CC) $(CFLAGS) game.c

clientGame: game.o clientGame.o 
	$(CC) game.o clientGame.o -o clientGame

clientGame.o: clientGame.c
	$(CC) $(CFLAGS) clientGame.c

serverGame: game.o serverGame.o 
	$(CC) game.o serverGame.o -lpthread -o serverGame

serverGame.o: serverGame.c
	$(CC) $(CFLAGS) serverGame.c
	
clean:
	rm -f  *.o
	rm -f $(EXECS)
