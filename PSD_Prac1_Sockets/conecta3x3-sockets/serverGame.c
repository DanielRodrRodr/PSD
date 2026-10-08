#include "game.h"
#include <pthread.h>

#define MAX_MSG_LENGTH 256		// Maximum length of the message

/**
 * Sends a message to the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToPlayer (int socketClient, char* message){

	unsigned int length = strlen(message);
    int msgLength = send(socketClient, &length, sizeof(length), 0);

    if (msgLength < 0)
        showError("ERROR while writing to socket");

    msgLength = send(socketClient, message, length, 0);
    if (msgLength < 0)
        showError("ERROR while writing to socket");
	
}

/**
 * Receives a message from the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be received
 */
void receiveMessageFromPlayer (int socketClient, char* message){

	unsigned int length = 0;
    int msgLength = recv(socketClient, &length, sizeof(length), 0);

    if (msgLength < 0)
        showError("ERROR while reading from socket");

    memset(message, 0, MAX_MSG_LENGTH);
    if (length > 0) {
        msgLength = recv(socketClient, message, length, 0);
        if (msgLength < 0)
            showError("ERROR while reading from socket");
        message[length] = '\0';
    }
}

/**
 * Sends a code to the player.
 * @param socketClient Socket descriptor
 * @param code Code to be send
 */
void sendCodeToClient (int socketClient, unsigned int code){

	int messageLength = send(socketClient, &code, sizeof(code), 0);

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
	int messageLength = send(socketClient, board, sizeof(tBoard), 0);

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

	unsigned int move = 0;
    int messageLength = recv(socketClient, &move, sizeof(move), 0);

    if (messageLength < 0)
        showError("ERROR while reading from socket");

    return move;
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

	if (player == player1) return player1socket;
    else if (player == player2) return player2socket;
    else if (player == player3) return player3socket;
    return -1;
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
    return player1;
}

void *threadProcessing(void *threadArgs){
	tThreadArgs *args = (tThreadArgs *)threadArgs;
    int s1 = args->socketPlayer1;
    int s2 = args->socketPlayer2;
    int s3 = args->socketPlayer3;
    free(args);

    char name1[MAX_MSG_LENGTH], name2[MAX_MSG_LENGTH], name3[MAX_MSG_LENGTH];

    // Cambio de nombres
    receiveMessageFromPlayer(s1, name1);
    receiveMessageFromPlayer(s2, name2);
    receiveMessageFromPlayer(s3, name3);

    // Envia a cada jugador los nombres de los rivales
    sendMessageToPlayer(s1, name2);
    sendMessageToPlayer(s1, name3);

    sendMessageToPlayer(s2, name1);
    sendMessageToPlayer(s2, name3);

    sendMessageToPlayer(s3, name1);
    sendMessageToPlayer(s3, name2);

    // Juego
    tBoard board;
    initBoard(board);

    tPlayer currentPlayer = player1;
    int activeSocket = s1;
    tMove moveResult;

    // Info inicial
    sendCodeToClient(s1, TURN_MOVE);
    sendMessageToPlayer(s1, "Its your turn. You play with:o");
    sendBoardToClient(s1, board);

    sendCodeToClient(s2, TURN_WAIT);
    sendMessageToPlayer(s2, "Your rival is thinking... please, wait! You play with:x");
    sendBoardToClient(s2, board);

    sendCodeToClient(s3, TURN_WAIT);
    sendMessageToPlayer(s3, "Your rival is thinking... please, wait! You play with:-");
    sendBoardToClient(s3, board);

    int gameOver = 0;

    while (!gameOver) {
        activeSocket = getSocketPlayer(currentPlayer, s1, s2, s3);
        unsigned int column = receiveMoveFromPlayer(activeSocket);

        moveResult = insertChip(board, currentPlayer, column);

        if (moveResult != OK_move) {
            sendCodeToClient(activeSocket, TURN_MOVE);
            sendMessageToPlayer(activeSocket, "Invalid move! Column full or out of range. Try again:");
            sendBoardToClient(activeSocket, board);
            continue;
        }

        if (checkWinner(board, currentPlayer)) {
            gameOver = 1;
            if (currentPlayer == player1) {
                sendCodeToClient(s1, GAMEOVER_WIN);  sendMessageToPlayer(s1, "You WIN!");                sendBoardToClient(s1, board);
                sendCodeToClient(s2, GAMEOVER_LOSE); sendMessageToPlayer(s2, "GAMEOVER: Player 1 won."); sendBoardToClient(s2, board);
                sendCodeToClient(s3, GAMEOVER_LOSE); sendMessageToPlayer(s3, "GAMEOVER: Player 1 won."); sendBoardToClient(s3, board);
            } else if (currentPlayer == player2) {
                sendCodeToClient(s1, GAMEOVER_LOSE); sendMessageToPlayer(s1, "GAMEOVER: Player 2 won."); sendBoardToClient(s1, board);
                sendCodeToClient(s2, GAMEOVER_WIN);  sendMessageToPlayer(s2, "You WIN!");                sendBoardToClient(s2, board);
                sendCodeToClient(s3, GAMEOVER_LOSE); sendMessageToPlayer(s3, "GAMEOVER: Player 2 won."); sendBoardToClient(s3, board);
            } else {
                sendCodeToClient(s1, GAMEOVER_LOSE); sendMessageToPlayer(s1, "GAMEOVER: Player 3 won."); sendBoardToClient(s1, board);
                sendCodeToClient(s2, GAMEOVER_LOSE); sendMessageToPlayer(s2, "GAMEOVER: Player 3 won."); sendBoardToClient(s2, board);
                sendCodeToClient(s3, GAMEOVER_WIN);  sendMessageToPlayer(s3, "You WIN!");                sendBoardToClient(s3, board);
            }
        } else if (isBoardFull(board)) {
            gameOver = 1;
            sendCodeToClient(s1, GAMEOVER_DRAW); sendMessageToPlayer(s1, "GAMEOVER: It's a draw!"); sendBoardToClient(s1, board);
            sendCodeToClient(s2, GAMEOVER_DRAW); sendMessageToPlayer(s2, "GAMEOVER: It's a draw!"); sendBoardToClient(s2, board);
            sendCodeToClient(s3, GAMEOVER_DRAW); sendMessageToPlayer(s3, "GAMEOVER: It's a draw!"); sendBoardToClient(s3, board);
        } else {
            currentPlayer = getNextPlayer(currentPlayer);

            if (currentPlayer == player1) {
                sendCodeToClient(s1, TURN_MOVE); sendMessageToPlayer(s1, "Its your turn. You play with:o"); sendBoardToClient(s1, board);
                sendCodeToClient(s2, TURN_WAIT); sendMessageToPlayer(s2, "Your rival is thinking... please, wait! You play with:x"); sendBoardToClient(s2, board);
                sendCodeToClient(s3, TURN_WAIT); sendMessageToPlayer(s3, "Your rival is thinking... please, wait! You play with:-"); sendBoardToClient(s3, board);
            } else if (currentPlayer == player2) {
                sendCodeToClient(s1, TURN_WAIT); sendMessageToPlayer(s1, "Your rival is thinking... please, wait! You play with:o"); sendBoardToClient(s1, board);
                sendCodeToClient(s2, TURN_MOVE); sendMessageToPlayer(s2, "Its your turn. You play with:x"); sendBoardToClient(s2, board);
                sendCodeToClient(s3, TURN_WAIT); sendMessageToPlayer(s3, "Your rival is thinking... please, wait! You play with:-"); sendBoardToClient(s3, board);
            } else {
                sendCodeToClient(s1, TURN_WAIT); sendMessageToPlayer(s1, "Your rival is thinking... please, wait! You play with:o"); sendBoardToClient(s1, board);
                sendCodeToClient(s2, TURN_WAIT); sendMessageToPlayer(s2, "Your rival is thinking... please, wait! You play with:x"); sendBoardToClient(s2, board);
                sendCodeToClient(s3, TURN_MOVE); sendMessageToPlayer(s3, "Its your turn. You play with:-"); sendBoardToClient(s3, board);
            }
        }
    }

    close(s1);
    close(s2);
    close(s3);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	struct sockaddr_in serverAddress;	/** Server address structure */
	unsigned int port;					/** Listening port */
	struct sockaddr_in playerAddress;	/** Client address structure for player 1 */
	unsigned int clientLength;			/** Length of client structure */
	pthread_t threadID;					/** Thread ID */


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

	// Partidas
	while (1) {
        tThreadArgs *args = (tThreadArgs *) malloc(sizeof(tThreadArgs));

        clientLength = sizeof(playerAddress);
        args->socketPlayer1 = accept(socketfd, (struct sockaddr *) &playerAddress, &clientLength);
        if (args->socketPlayer1 < 0) showError("ERROR while accepting player 1");
        printf("Jugador 1 conectado\n");
        fflush(stdout);

        clientLength = sizeof(playerAddress);
        args->socketPlayer2 = accept(socketfd, (struct sockaddr *) &playerAddress, &clientLength);
        if (args->socketPlayer2 < 0) showError("ERROR while accepting player 2");
        printf("Jugador 2 conectado\n");
        fflush(stdout);

        clientLength = sizeof(playerAddress);
        args->socketPlayer3 = accept(socketfd, (struct sockaddr *) &playerAddress, &clientLength);
        if (args->socketPlayer3 < 0) showError("ERROR while accepting player 3");
        printf("Jugador 3 conectado\n");
        fflush(stdout);

        if (pthread_create(&threadID, NULL, threadProcessing, (void *)args) != 0) {
            showError("ERROR creating thread");
        }
        pthread_detach(threadID);
    }

	// Close sockets
	close(socketfd);

    return 0; 
}	
