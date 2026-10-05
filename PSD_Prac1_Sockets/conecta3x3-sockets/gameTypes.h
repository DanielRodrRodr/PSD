#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>


/** Board width (in number of cells) */
#define BOARD_WIDTH 5

/** Board height (in number of cells) */
#define BOARD_HEIGHT 5

/** Character for player 1 chip */
#define PLAYER_1_CHIP 'o'

/** Character for player 2 chip */
#define PLAYER_2_CHIP 'x'

/** Character for player 3 chip */
#define PLAYER_3_CHIP '-'

/** Character for empty cell */
#define EMPTY_CELL ' '

/** True value */
#define TRUE 1

/** False value */
#define FALSE 0

/** Code for performing a move */
#define TURN_MOVE 70001

/** Code for waiting */
#define TURN_WAIT 70002

/** Code to show that the player wins */
#define GAMEOVER_WIN 50052

/** Code to show a draw game */
#define GAMEOVER_DRAW 50053

/** Code to show that the player loses */
#define GAMEOVER_LOSE 50054

/** Length for tString */
#define STRING_LENGTH 128

/** Board */
typedef char tBoard [BOARD_WIDTH * BOARD_HEIGHT];

/** Type for names, messages and this kind of variables */
typedef char tString [STRING_LENGTH];

/** Players */
typedef enum {player1, player2, player3} tPlayer;

/** Result for moves */
typedef enum {OK_move, wrongColumn_move, fullColumn_move} tMove;

/** Sockets of a game used by a thread in the server */
typedef struct threadArgs{
	int socketPlayer1;
	int socketPlayer2;
	int socketPlayer3;
} tThreadArgs;
