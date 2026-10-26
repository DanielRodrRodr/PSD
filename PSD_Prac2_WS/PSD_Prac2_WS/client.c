#include "soapH.h"
#include "conecta3x3.nsmap"
#include "game.h"

#define DEBUG_CLIENT 1
#define STRING_LENGTH 256

/**
 * Reads a player move
 * @return A number between [0-6]
 */
unsigned int readMove (){

	xsd__string enteredMove;
	unsigned int move;
	unsigned int isRightMove;

		// Init...
		enteredMove = (xsd__string) malloc (STRING_LENGTH);
		memset (enteredMove, 0, STRING_LENGTH);
		isRightMove = FALSE;
		move = STRING_LENGTH;

		while (!isRightMove){

			printf ("Enter a move [0-6]:");

			// Read move
			fgets (enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Length of entered move is not correct
			if (strlen(enteredMove) != 1){
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
			}

			// Check if entered move is a number
			else if (isdigit(enteredMove[0])){

				// Convert move to an int
				move =  enteredMove[0] - '0';

				if (move > 6)
					printf ("Entered move is not correct. It must be a number between [0-6]\n");
				else
					isRightMove = TRUE;
			}

			// Entered move is not a number
			else
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
		}

	return move;
}


int main(int argc, char **argv){

	struct soap soap;					/** Soap struct */
	char *serverURL;					/** Server URL */
	unsigned int endOfGame;				/** Flag to control the end of the game */
	conecta3x3ns__tMessage playerName;	/** Player name */
	conecta3x3ns__tBlock gameStatus;	/** Game status */
	unsigned int playerMove;			/** Player move */
	int gameId;						/** Return code from server */
	char nameBuffer[STRING_LENGTH];       /** Buffer for player name input */

	// Check arguments
	if (argc != 2) {
		printf("Usage: %s http://server:port\n", argv[0]);
		exit(0);
	}

	// Obtain server address
	serverURL = argv[1];

	// Init gSOAP environment
	soap_init(&soap);

	// Obtain server address
	serverURL = argv[1];

	// Player name
    printf("Enter player name: ");
    if (fgets(nameBuffer, STRING_LENGTH - 1, stdin) != NULL) {
        nameBuffer[strcspn(nameBuffer, "\n")] = 0;
    }

    // Initialize playerName structure (tMessage)
    playerName.__size = strlen(nameBuffer);
    playerName.msg = (xsd__string) soap_malloc(&soap, STRING_LENGTH);
    strcpy(playerName.msg, nameBuffer);

    // 1. Register player
    if (soap_call_conecta3x3ns__register(&soap, serverURL, "", playerName, &gameId) != SOAP_OK) {
        soap_print_fault(&soap, stderr);
        soap_destroy(&soap);
        soap_end(&soap);
        soap_done(&soap);
        exit(1);
    }

    // Check registration response code
    if (gameId < 0) {
        if (gameId == ERROR_SERVER_FULL) {
            printf("Error: Server is full (no available games or slots).\n");
        } else if (gameId == ERROR_PLAYER_REPEATED) {
            printf("Error: Player name is already registered in this game.\n");
        } else {
            printf("Error registering player. Code: %d\n", gameId);
        }
        soap_destroy(&soap);
        soap_end(&soap);
        soap_done(&soap);
        exit(1);
    }

    printf("Player registered successfully! Assigned to Game ID: %d\n", gameId);

    // Initialize game status block structure
    soap_default_conecta3x3ns__tBlock(&soap, &gameStatus);

    // 2. Game
    endOfGame = FALSE;
    while (!endOfGame) {
        // Request game status
        if (soap_call_conecta3x3ns__getStatus(&soap, serverURL, "", gameId, playerName, &gameStatus) != SOAP_OK) {
            soap_print_fault(&soap, stderr);
            break;
        }

        // Check if player was not found
        if (gameStatus.code == ERROR_PLAYER_NOT_FOUND) {
            printf("Error: Player not found on server. Exiting.\n");
            break;
        }

        // Print board and status message using printBoard from utils.h
        printBoard(gameStatus.board, gameStatus.msgStruct.msg);

        // Check if the game is already over
        if (gameStatus.code == GAMEOVER_WIN || gameStatus.code == GAMEOVER_DRAW || gameStatus.code == GAMEOVER_LOSE) {
            endOfGame = TRUE;
            break;
        }

        // 3. While player's turn
        while (gameStatus.code == TURN_MOVE) {
            // Read valid move [0-6]
            playerMove = readMove();

            // Insert chip into the board
            if (soap_call_conecta3x3ns__insertChip(&soap, serverURL, "", gameId, playerName, playerMove, &gameStatus) != SOAP_OK) {
                soap_print_fault(&soap, stderr);
                break;
            }

            // Print board
            printBoard(gameStatus.board, gameStatus.msgStruct.msg);

            // Check if game end
            if (gameStatus.code == GAMEOVER_WIN || gameStatus.code == GAMEOVER_DRAW || gameStatus.code == GAMEOVER_LOSE) {
                endOfGame = TRUE;
                break;
            }

            // If turn is no longer ours
            if (gameStatus.code != TURN_MOVE) {
                break;
            }
        }
    }

    // Clean the gSOAP environment
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  return 0;
}
