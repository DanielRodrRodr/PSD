#include "game.h"
#include <pthread.h>

#define MAX_MSG_LENGTH 256		// Maximum length of the message

/**
 * Sends a message to the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToPlayer (int socketClient, char* message){

	int messageLength = send(socketClient, message, strlen(message), 0);

	// Check bytes sent
	if (messageLength < 0)
		showError("ERROR while writing to socket");
	
}

/**
 * Receives a message from the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be received
 */
void receiveMessageFromPlayer (int socketClient, char* message){

	memset(message, 0, MAX_MSG_LENGTH);
	int messageLength = recv(socketClient, message, MAX_MSG_LENGTH-1, 0);
}

/**
 * Sends a code to the player.
 * @param socketClient Socket descriptor
 * @param code Code to be send
 */
void sendCodeToClient (int socketClient, unsigned int code){

	int messageLength = send(socketClient, (char*)code, strlen(code), 0);

	// Check bytes sent
	if (messageLength < 0)
		showError("ERROR while writing to socket");
	
}

/**
 * Sends a board to the player.
 * @param socketClient Socket descriptor
 * @param board Board of the game
 */
void sendBoardToClient (int socketClient, tBoard board){
	int messageLength = send(socketClient, (char*)board, strlen(board), 0);

	// Check bytes sent
	if (messageLength < 0)
		showError("ERROR while writing to socket");

	
}

/**
 * Receives a move from the player.
 * @param socketClient Socket descriptor
 * @return Move performed by the player
 */
unsigned int receiveMoveFromPlayer (int socketClient){

	char *message;
	int messageLength = recv(socketClient, message, MAX_MSG_LENGTH-1, 0);
	memset(messageLength, 0, MAX_MSG_LENGTH);
}

/**
 * Gets the socket of the current player
 *
 * @param player Current player
 * @param player1socket Socket that connects with player 1
 * @param player2socket Socket that connects with player 2
 * @param player3socket Socket that connects with player 3
 * @return Associated socket to player
 */
int getSocketPlayer (tPlayer player, int player1socket, int player2socket, int player3socket){

	
}

/**
 * Gets the next player to move.
 *
 * @param currentPlayer Current player
 * @return Next player
 */
tPlayer getNextPlayer (tPlayer currentPlayer){
	if(currentPlayer==player1) return player2;
	else if(currentPlayer==player2) return player3;
	else if(currentPlayer==player3) return player1;
}

void *threadProcessing(void *threadArgs){

	
}

int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	struct sockaddr_in serverAddress;	/** Server address structure */
	unsigned int port;					/** Listening port */
	struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	struct sockaddr_in player3Address;	/** Client address structure for player 3 */
	tThreadArgs thArg;					/** Socket descriptor for players */
	unsigned int clientLength;			/** Length of client structure */
	tThreadArgs *threadArgs; 			/** Thread parameters */
	pthread_t threadID;					/** Thread ID */
	char message[MAX_MSG_LENGTH];		/** Message */
	int messageLength;					/** Length of the message */


	// Check arguments
	if (argc != 2) {
		fprintf(stderr,"ERROR wrong number of arguments\n");
		fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
		exit(1);
	}

	// Create the socket
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check
	if (socketfd < 0)
	showError("ERROR while opening socket");

	// Init server structure
	memset(&serverAddress, 0, sizeof(serverAddress));

	// Get listening port
	port = atoi(argv[1]);

	// Fill server structure
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Bind
	if (bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		showError("ERROR while binding");

	// Listen
	listen(socketfd, 10);

	// Get length of client structure
	clientLength = sizeof(player1Address);

	// Accept!
	thArg.socketPlayer1 = accept(socketfd, (struct sockaddr *) &player1Address, &clientLength);
	
	// Check accept result
	if (thArg.socketPlayer1  < 0)
		showError("ERROR while accepting");

	// Accept!
	clientLength = sizeof(player2Address);
	thArg.socketPlayer2 = accept(socketfd, (struct sockaddr *) &player2Address, &clientLength);

	// Check accept result
	if (thArg.socketPlayer2 < 0)
		showError("ERROR while accepting");

	// Accept!
	clientLength = sizeof(player3Address);
	thArg.socketPlayer3 = accept(socketfd, (struct sockaddr *) &player3Address, &clientLength);

	// Check accept result
	if (thArg.socketPlayer3 < 0)
		showError("ERROR while accepting");	  

	// Init and read message

	tPlayer player=player1;
	tBoard board;
	initBoard(board);
	char* msg;
	while(!checkWinner(board,player)&&!isBoardFull(board)){
		if(player==player1){
			sendCodeToClient(thArg.socketPlayer1,TURN_MOVE);
			sendMessageToPlayer(thArg.socketPlayer1, "Its your turn. You play with:o");
			sendBoardToClient(thArg.socketPlayer1,board);

			sendCodeToClient(thArg.socketPlayer2,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer2, "Your rival is thinking... please, wait! You play with:x");
			sendBoardToClient(thArg.socketPlayer2,board);

			sendCodeToClient(thArg.socketPlayer3,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer3, "Your rival is thinking... please, wait! You play with: -");
			sendBoardToClient(thArg.socketPlayer3,board);

			receiveMessageFromPlayer(thArg.socketPlayer1, msg);
		}
		else if(player==player2){
			
			sendCodeToClient(thArg.socketPlayer1,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer1, "Your rival is thinking... please, wait! You play with:o");
			sendBoardToClient(thArg.socketPlayer1,board);

			sendCodeToClient(thArg.socketPlayer2,TURN_MOVE);
			sendMessageToPlayer(thArg.socketPlayer2, "Its your turn. You play with:x");
			sendBoardToClient(thArg.socketPlayer2,board);

			sendCodeToClient(thArg.socketPlayer3,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer3, "Your rival is thinking... please, wait! You play with:-");
			sendBoardToClient(thArg.socketPlayer3,board);

			receiveMessageFromPlayer(thArg.socketPlayer1, msg);
		}
		else if(player==player3){
			sendCodeToClient(thArg.socketPlayer1,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer1, "Your rival is thinking... please, wait! You play with:o");
			sendBoardToClient(thArg.socketPlayer1,board);

			sendCodeToClient(thArg.socketPlayer2,TURN_WAIT);
			sendMessageToPlayer(thArg.socketPlayer2, "Your rival is thinking... please, wait! You play with:x");
			sendBoardToClient(thArg.socketPlayer2,board);

			sendCodeToClient(thArg.socketPlayer3,TURN_MOVE);
			sendMessageToPlayer(thArg.socketPlayer3, "Its your turn. You play with:-");
			sendBoardToClient(thArg.socketPlayer3,board);

			receiveMessageFromPlayer(thArg.socketPlayer1, msg);
		}
		insertChip();
		player=getNextPlayer(player);
	}



	memset(message, 0, MAX_MSG_LENGTH);
	messageLength = recv(newsockfd, message, MAX_MSG_LENGTH-1, 0);

	// Check read bytes
	if (messageLength < 0)
		showError("ERROR while reading from socket");

	// Show message
	printf("Message: %d\n", messageLength);

	// Get the message length
	memset (messageLength, 0, MAX_MSG_LENGTH);
	messageLength = send(newsockfd, messageLength, strlen(messageLength), 0);

	// Check bytes sent
	if (messageLength < 0)
		showError("ERROR while writing to socket");

	// Close sockets
	close(newsockfd);
	close(socketfd);

    return 0; 
}	
