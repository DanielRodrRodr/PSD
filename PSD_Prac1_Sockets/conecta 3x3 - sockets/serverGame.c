#include "game.h"
#include <pthread.h>

#define MAX_MSG_LENGTH 256		// Maximum length of the message

/**
 * Sends a message to the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToPlayer (int socketClient, char* message){

	messageLength = send(socketClient, message, strlen(message), 0);

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
	messageLength = recv(socketClient, message, MAX_MSG_LENGTH-1, 0);
}

/**
 * Sends a code to the player.
 * @param socketClient Socket descriptor
 * @param code Code to be send
 */
void sendCodeToClient (int socketClient, unsigned int code){

	messageLength = send(socketClient, (char*)code, strlen(message), 0);

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
	messageLength = send(socketClient, (char*)board, strlen(message), 0);

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

	memset(message, 0, MAX_MSG_LENGTH);
	messageLength = recv(socketClient, message, MAX_MSG_LENGTH-1, 0);
	
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
	threadArgs thArg;					/** Socket descriptor for players */
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
	clientLength = sizeof(clientAddress);

	// Accept!
	socketPlayer1 = accept(socketfd, (struct sockaddr *) &clientAddress, &clientLength);
	
	// Check accept result
	if (socketPlayer1 < 0)
		showError("ERROR while accepting");

	// Accept!
	socketPlayer2 = accept(socketfd, (struct sockaddr *) &clientAddress, &clientLength);

	// Check accept result
	if (socketPlayer2 < 0)
		showError("ERROR while accepting");

	// Accept!
	socketPlayer3 = accept(socketfd, (struct sockaddr *) &clientAddress, &clientLength);

	// Check accept result
	if (socketPlayer3 < 0)
		showError("ERROR while accepting");	  

	// Init and read message
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
