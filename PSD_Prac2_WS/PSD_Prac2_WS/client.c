#include "soapH.h"
#include "conecta3x3.nsmap"
#include "game.h"

#define DEBUG_CLIENT 1

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


		// Init gSOAP environment
		soap_init(&soap);

		// Obtain server address
		serverURL = argv[1];

		
		// Check arguments
		if (argc !=2) {
			printf("Usage: %s http://server:port\n",argv[0]);
			exit(0);
		}

		
    
    
    
    
    

		// Clean the environment
		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);

  return 0;
}
