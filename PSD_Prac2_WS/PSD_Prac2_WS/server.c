#include "soapH.h"
#include "game.h"
#include "conecta3x3.nsmap"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/** Maximum number of active games in the server */
#define MAX_GAMES 5

/** Flag to enable debugging */
#define DEBUG_SERVER 1

/** Type for game status */
typedef enum {gameEmpty, gameWaitingPlayer, gameReady} tGameState;

/**
 * Struct that contains a game for the players
 */
typedef struct game{

    tGameState state;
    tBoard board;
    conecta3x3ns__tPlayer currentPlayer;
    char player1Name[256];
    char player2Name[256];
    char player3Name[256];
    int numPlayers;
    int activePlayers;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
}tGame;

/** Array with games */
tGame games[MAX_GAMES];

/** Global mutex for safe game searching and allocation */
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Initializes server structures
 * @param soap Soap environment
 */
void initServerStructures (struct soap *soap);

/**
 * Calculates the next player
 *
 * @param currentPlayer Current player
 * @return Player that is currently awaiting.
 */
conecta3x3ns__tPlayer calculateNextPlayer (conecta3x3ns__tPlayer currentPlayer);

/**
 * Calculates a random player to start the game
 *
 * @return Player that starts the game.
 */
conecta3x3ns__tPlayer calculateRandomPlayer ();


/**
 * Search for a game.
 *
 * This function returns a number between [0-MAX_GAMES] if there
 * exists an empty game, or ERROR_SERVER_FULL in other case.
 */
int searchEmptyGame ();

/**
 * Thread function to process client requests
 */
void *processRequest(void*);

void copyGameStatusStructure (conecta3x3ns__tBlock* status, char* message, char* board, int newCode);



void initServerStructures (struct soap *soap){

	if (DEBUG_SERVER)
		printf ("Initializing...\n");

	// Init seed
	srand (time(NULL));

	// Init each game
	for (int i=0; i<MAX_GAMES; i++){
		games[i].state = gameEmpty;
        games[i].numPlayers = 0;
        games[i].activePlayers = 0;
        pthread_mutex_init(&games[i].mutex, NULL);
        pthread_cond_init(&games[i].cond, NULL);
        initBoard(games[i].board);
        memset(games[i].player1Name, 0, sizeof(games[i].player1Name));
        memset(games[i].player2Name, 0, sizeof(games[i].player2Name));
        memset(games[i].player3Name, 0, sizeof(games[i].player3Name));
	
	}
}

conecta3x3ns__tPlayer calculateNextPlayer (conecta3x3ns__tPlayer currentPlayer){
	if (currentPlayer == player1)
        return player2;
    else if (currentPlayer == player2)
        return player3;
    else
        return player1;
}

conecta3x3ns__tPlayer calculateRandomPlayer (){
	return (conecta3x3ns__tPlayer)(rand() % 3);
}

int searchEmptyGame (){
	for (int i = 0; i < MAX_GAMES; i++) {
        if (games[i].state == gameEmpty || (games[i].state == gameWaitingPlayer && games[i].numPlayers < 3)) {
            return i;
        }
    }
    return ERROR_SERVER_FULL;
}

void copyGameStatusStructure (conecta3x3ns__tBlock* status, char* message, char* board, int newCode){

	// Copy the message
	if (message != NULL) {
        strncpy((status->msgStruct).msg, message, 255);
        (status->msgStruct).msg[255] = '\0';
        (status->msgStruct).__size = strlen ((status->msgStruct).msg);
    } else {
        (status->msgStruct).msg[0] = '\0';
        (status->msgStruct).__size = 0;
    }

	// Copy the board, only if it is not NULL
	if (board == NULL){
		status->board = NULL;
		status->__size = 0;
	}
	else{
		if (status->board == NULL) {
            status->board = (xsd__string) malloc(BOARD_WIDTH * BOARD_HEIGHT);
        }
        memcpy(status->board, board, BOARD_WIDTH * BOARD_HEIGHT);
        status->__size = BOARD_WIDTH * BOARD_HEIGHT;
	}

	// Set the new code
	status->code = newCode;
}

int conecta3x3ns__register (struct soap *soap, conecta3x3ns__tMessage playerName, int* code){
	pthread_mutex_lock(&global_mutex);

    int gameId = searchEmptyGame();
    if (gameId < 0) {
        pthread_mutex_unlock(&global_mutex);
        *code = ERROR_SERVER_FULL;
        return SOAP_OK;
    }

    pthread_mutex_lock(&games[gameId].mutex);
    pthread_mutex_unlock(&global_mutex); // Safe to release global once individual game is locked

    // Check if player name is already repeated in this game
    if ((games[gameId].numPlayers > 0 && strcmp(games[gameId].player1Name, playerName.msg) == 0) ||
        (games[gameId].numPlayers > 1 && strcmp(games[gameId].player2Name, playerName.msg) == 0) ||
        (games[gameId].numPlayers > 2 && strcmp(games[gameId].player3Name, playerName.msg) == 0)) {
        
        pthread_mutex_unlock(&games[gameId].mutex);
        *code = ERROR_PLAYER_REPEATED;
        return SOAP_OK;
    }

    // Register player in the next available slot
    if (games[gameId].numPlayers == 0) {
        strcpy(games[gameId].player1Name, playerName.msg);
    } else if (games[gameId].numPlayers == 1) {
        strcpy(games[gameId].player2Name, playerName.msg);
    } else if (games[gameId].numPlayers == 2) {
        strcpy(games[gameId].player3Name, playerName.msg);
    }

    games[gameId].numPlayers++;
    games[gameId].activePlayers++;

    if (games[gameId].numPlayers == 3) {
        games[gameId].state = gameReady;
        games[gameId].currentPlayer = calculateRandomPlayer();
    } else {
        games[gameId].state = gameWaitingPlayer;
    }

    *code = gameId;

    if (DEBUG_SERVER) {
        printf("Player [%s] registered in Game ID [%d]. Total players: %d\n", playerName.msg, gameId, games[gameId].numPlayers);
    }

    pthread_mutex_unlock(&games[gameId].mutex);
    return SOAP_OK;
	
}

/**
 * Service: Get game status
 */
int conecta3x3ns__getStatus (struct soap *soap, int gameId, conecta3x3ns__tMessage playerName, conecta3x3ns__tBlock *status) {
    if (gameId < 0 || gameId >= MAX_GAMES) {
        status->code = ERROR_PLAYER_NOT_FOUND;
        return SOAP_OK;
    }

    pthread_mutex_lock(&games[gameId].mutex);

    // Verify player exists in this game
    int isRegistered = (strcmp(games[gameId].player1Name, playerName.msg) == 0) ||
                       (strcmp(games[gameId].player2Name, playerName.msg) == 0) ||
                       (strcmp(games[gameId].player3Name, playerName.msg) == 0);

    if (!isRegistered) {
        pthread_mutex_unlock(&games[gameId].mutex);
        status->code = ERROR_PLAYER_NOT_FOUND;
        return SOAP_OK;
    }

    // Determine player enum
    conecta3x3ns__tPlayer myRole;
    if (strcmp(games[gameId].player1Name, playerName.msg) == 0) myRole = player1;
    else if (strcmp(games[gameId].player2Name, playerName.msg) == 0) myRole = player2;
    else myRole = player3;

    // Wait until it's our turn or game is ready/waiting
    while (games[gameId].state == gameReady && games[gameId].currentPlayer != myRole) {
        pthread_cond_wait(&games[gameId].cond, &games[gameId].mutex);
    }

    // Check if game is waiting for more players
    if (games[gameId].state == gameWaitingPlayer) {
        copyGameStatusStructure(status, "Waiting for other players to join...", (char*)games[gameId].board, TURN_WAIT);
    } else {
        // Game is ready
        if (games[gameId].currentPlayer == myRole) {
            copyGameStatusStructure(status, "It is your turn!", (char*)games[gameId].board, TURN_MOVE);
        } else {
            copyGameStatusStructure(status, "Waiting for other player's move...", (char*)games[gameId].board, TURN_WAIT);
        }
    }

    pthread_mutex_unlock(&games[gameId].mutex);
    return SOAP_OK;
}

/**
 * Service: Insert chip into the board
 */
int conecta3x3ns__insertChip (struct soap *soap, int gameId, conecta3x3ns__tMessage playerName, unsigned int column, conecta3x3ns__tBlock *status) {
    if (gameId < 0 || gameId >= MAX_GAMES) {
        status->code = ERROR_PLAYER_NOT_FOUND;
        return SOAP_OK;
    }

    pthread_mutex_lock(&games[gameId].mutex);

    int isRegistered = (strcmp(games[gameId].player1Name, playerName.msg) == 0) ||
                       (strcmp(games[gameId].player2Name, playerName.msg) == 0) ||
                       (strcmp(games[gameId].player3Name, playerName.msg) == 0);

    if (!isRegistered) {
        pthread_mutex_unlock(&games[gameId].mutex);
        status->code = ERROR_PLAYER_NOT_FOUND;
        return SOAP_OK;
    }

    conecta3x3ns__tPlayer myRole;
    if (strcmp(games[gameId].player1Name, playerName.msg) == 0) myRole = player1;
    else if (strcmp(games[gameId].player2Name, playerName.msg) == 0) myRole = player2;
    else myRole = player3;

    // Check move validity
    tMove moveResult = checkMove(games[gameId].board, column);
    if (moveResult != MOVE_OK) {
        copyGameStatusStructure(status, "Invalid move! Column is full or out of bounds.", (char*)games[gameId].board, TURN_MOVE);
        pthread_mutex_unlock(&games[gameId].mutex);
        return SOAP_OK;
    }

    // Perform move
    insertChip(games[gameId].board, myRole, column);

    // Check winner
    int winner = checkWinner(games[gameId].board, myRole);
    if (winner) {
        copyGameStatusStructure(status, "Congratulations! You won the game!", (char*)games[gameId].board, GAMEOVER_WIN);
        // Clean up / reset game for future reuse
        games[gameId].activePlayers--;
        if (games[gameId].activePlayers <= 0) {
            games[gameId].state = gameEmpty;
            games[gameId].numPlayers = 0;
            initBoard(games[gameId].board);
        }
        pthread_cond_broadcast(&games[gameId].cond);
        pthread_mutex_unlock(&games[gameId].mutex);
        return SOAP_OK;
    }

    // Check board full (Draw)
    if (isBoardFull(games[gameId].board)) {
        copyGameStatusStructure(status, "Game over! It's a draw.", (char*)games[gameId].board, GAMEOVER_DRAW);
        games[gameId].activePlayers--;
        if (games[gameId].activePlayers <= 0) {
            games[gameId].state = gameEmpty;
            games[gameId].numPlayers = 0;
            initBoard(games[gameId].board);
        }
        pthread_cond_broadcast(&games[gameId].cond);
        pthread_mutex_unlock(&games[gameId].mutex);
        return SOAP_OK;
    }

    // Advance turn
    games[gameId].currentPlayer = calculateNextPlayer(games[gameId].currentPlayer);
    copyGameStatusStructure(status, "Move successful. Waiting for next turn...", (char*)games[gameId].board, TURN_WAIT);

    // Broadcast change to wake up waiting threads
    pthread_cond_broadcast(&games[gameId].cond);
    pthread_mutex_unlock(&games[gameId].mutex);

    return SOAP_OK;
}

void *processRequest(void *soap){

	pthread_detach(pthread_self());

	if (DEBUG_SERVER)
		printf ("Processing a new request...");

	soap_serve((struct soap*)soap);
	soap_destroy((struct soap*)soap);
	soap_end((struct soap*)soap);
	soap_done((struct soap*)soap);
	free(soap);

	return NULL;
}

int main(int argc, char **argv){ 

	struct soap soap;
	struct soap *tsoap;
	pthread_t tid;
	int port;
	SOAP_SOCKET m, s;

	if (argc != 2) {
        printf("Usage: %s port\n", argv[0]);
        exit(1);
    }

	// Init soap environment
	soap_init(&soap);

	// Configure timeouts
	soap.send_timeout = 60; // 60 seconds
	soap.recv_timeout = 60; // 60 seconds
	soap.accept_timeout = 3600; // server stops after 1 hour of inactivity
	soap.max_keep_alive = 100; // max keep-alive sequence

	initServerStructures(&soap);

	// Get listening port
	port = atoi(argv[1]);

	// Bind
	m = soap_bind(&soap, NULL, port, 100);

	if (!soap_valid_socket(m)){
		exit(1);
	}

	printf("Server is ON!\n");

	while (TRUE){

		// Accept a new connection
		s = soap_accept(&soap);

		// Socket is not valid :(
		if (!soap_valid_socket(s)){

			if (soap.errnum){
				soap_print_fault(&soap, stderr);
				exit(1);
			}

			fprintf(stderr, "Time out!\n");
			break;
		}

		// Copy the SOAP environment
		tsoap = soap_copy(&soap);

		if (!tsoap){
			printf ("SOAP copy error!\n");
			break;
		}

		// Create a new thread to process the request
		pthread_create(&tid, NULL, (void*(*)(void*))processRequest, (void*)tsoap);
	}

	// Detach SOAP environment
	soap_done(&soap);
	return 0;
}
