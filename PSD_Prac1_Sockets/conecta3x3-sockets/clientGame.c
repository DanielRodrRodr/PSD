#include "game.h"

/**
 * Sends a message to the server. This includes the length of the message and the message itself
 * @param socketServer Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToServer (int socketServer, char* message){
	unsigned int length = strlen(message);
    int msgLength = send(socketServer, &length, sizeof(length), 0);

    if (msgLength < 0)
        showError("ERROR while writing to the socket");

    msgLength = send(socketServer, message, length, 0);
    if (msgLength < 0)
        showError("ERROR while writing to the socket");
}

/**
 * Receives a message from the server. This includes the length of the message and the message itself
 * @param socketServer Socket descriptor
 * @param message Message to be received
 */
void receiveMessageFromServer (int socketServer, char* message){

	unsigned int length = 0;
    int msgLength = recv(socketServer, &length, sizeof(length), 0);

    if (msgLength < 0)
        showError("ERROR while reading from the socket");

    memset(message, 0, MAX_MSG_LENGTH);
    if (length > 0) {
        msgLength = recv(socketServer, message, length, 0);
        if (msgLength < 0)
            showError("ERROR while reading from the socket");
        message[length] = '\0';
    }
}


/**
 * Receives a board from the server.
 * @param socketServer Socket descriptor
 * @param board Board of the game
 */
void receiveBoard (int socketServer, tBoard board){

	int msgLength = recv(socketServer, &board, sizeof(tBoard), 0);
	if (msgLength < 0)
		showError("ERROR while reading from the socket");
}


/**
 * Receives a code from the server.
 * @param socketServer Socket descriptor
 * @return Code
 */
unsigned int receiveCode (int socketServer){

	unsigned int code = 0;
    int msgLength = recv(socketServer, &code, sizeof(code), 0);

    if (msgLength < 0)
        showError("ERROR while reading from the socket");

    return code;
}

/**
 * Reads a move entered by the player
 * @return A number between [0-BOARD_WIDTH] that represents the column where the chip is going to be inserted
 */
unsigned int readMove (){

	int i, isValid, column;
	tString enteredMove;

		// Init...
		column = 0;

		// While player does not enter a correct move...
		do{

			// Init...
			bzero (enteredMove, STRING_LENGTH);
			isValid = TRUE;

			// Show input message
			printf ("Enter a move [0-%d]:", BOARD_WIDTH-1);

			// Read move
			fgets(enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Check if each character is a digit
			for (i=0; i<strlen(enteredMove) && isValid; i++){

				if (!isdigit(enteredMove[i]))
					isValid = FALSE;
			}

			// Entered move is not a number
			if (!isValid){
				printf ("Entered move is not correct. It must be a number between [0-%d]\n", BOARD_WIDTH-1);
			}

			// Entered move is a number...
			else{

				// Conver entered text to an int
				column = atoi (enteredMove);
			}

		}while (!isValid);

	return ((unsigned int) column);
}

/**
 * Sends a move to the server.
 * @param socketServer Socket descriptor
 * @param move A number between [0-6] that represents the column where the chip is going to be inserted
 */
void sendMoveToServer (int socketServer, unsigned int move){
	int msgLength = send(socketServer, &move, sizeof(move), 0);
    if (msgLength < 0)
        showError("ERROR while writing to socket");
}


int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;	/** Server address structure */
	char* serverIP;						/** Server IP */
    tString playerName;                    /** Name of the player */
	char rival1[MAX_MSG_LENGTH], rival2[MAX_MSG_LENGTH];

	// Check arguments!
	if (argc != 3){
		fprintf(stderr,"ERROR wrong number of arguments\n");
		fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
		exit(0);
	}

	// Get the server address
	serverIP = argv[1];

	// Get the port
	port = atoi(argv[2]);

	// Create socket
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check if the socket has been successfully created
	if (socketfd < 0)
		showError("ERROR while opening socket");
	

	// Fill server address structure
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Connect with server
	if (connect(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        showError("ERROR while connecting");

	// Init player's name
	do{
		memset(playerName, 0, STRING_LENGTH);
		printf ("Enter player name:");
		fgets(playerName, STRING_LENGTH-1, stdin);

		// Remove '\n'
		playerName[strlen(playerName)-1] = 0;

	}while (strlen(playerName) <= 2);

	// Main loop
	sendMessageToServer(socketfd, playerName);
	receiveMessageFromServer(socketfd, rival1);
    receiveMessageFromServer(socketfd, rival2);
	
	unsigned int code;
    char message[MAX_MSG_LENGTH];
    tBoard board;
	int gameOver = 0;

    while (!gameOver) {
        code = receiveCode(socketfd);
        receiveMessageFromServer(socketfd, message);
        receiveBoard(socketfd, board);

        // Imprimir tablero y mensaje de estado
        printBoard(board, message);

        if (code == TURN_MOVE) {
            unsigned int move = readMove();
            sendMoveToServer(socketfd, move);
        } else if (code == GAMEOVER_WIN || code == GAMEOVER_LOSE || code == GAMEOVER_DRAW) {
            gameOver = 1;
        }
    }


	// Close socket
	close (socketfd);

    return 0;
}
