//gsoap conecta3x3ns service name: conecta3x3
//gsoap conecta3x3ns service style: rpc
//gsoap conecta3x3ns service location: http//localhost:10000
//gsoap conecta3x3ns service encoding: encoded
//gsoap conecta3x3ns service namespace: urn:conecta3x3ns

/** Player has been successfully registered */
#define OK_NAME_REGISTERED 10000

/** A player is already registered with the same name */
#define ERROR_NAME_REPEATED -1

/** Server is full. No more games are allowed */
#define ERROR_SERVER_FULL -2

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

/** Character for player 1 chip */
#define PLAYER_1_CHIP 'o'

/** Character for player 2 chip */
#define PLAYER_2_CHIP 'x'

/** Character for player 3 chip */
#define PLAYER_3_CHIP '-'

/** Character for empty cell */
#define EMPTY_CELL ' '

/** Board width (in number of cells) */
#define BOARD_WIDTH 7

/** Board height (in number of cells) */
#define BOARD_HEIGHT 6

/** True value */
#define TRUE 1

/** False value */
#define FALSE 0

/** Length for tString */
#define STRING_LENGTH 256


/** Players */
typedef enum players {player1, player2, player3} conecta3x3ns__tPlayer;

/** Result for moves */
typedef enum moves {OK_move, fullColumn_move} conecta3x3ns__tMove;


/** Dynamic array of chars */
typedef char *xsd__string;

/** Message used to send the player's name and messages sent from server */
typedef struct tMessage{
	int __size;
	xsd__string msg;
}conecta3x3ns__tMessage;

/** Response from the server */
typedef struct tBlock{
	unsigned int code;
	conecta3x3ns__tMessage msgStruct;
	int __size;
	xsd__string board;
}conecta3x3ns__tBlock;

int conecta3x3ns__register  	(conecta3x3ns__tMessage playerName, int* code);
