#include "game.h"


void initBoard (xsd__string board){

	int i;

		for (i=0; i<(BOARD_WIDTH * BOARD_HEIGHT); i++)
			board[i] = (char) EMPTY_CELL;
}


conecta3x3ns__tMove checkMove (xsd__string board, unsigned int column){

	conecta3x3ns__tMove move;

		// Column is full
		if (board[(BOARD_WIDTH*(BOARD_HEIGHT-1)) + column] != EMPTY_CELL)
			move = fullColumn_move;

		// Correct move!
		else
			move = OK_move;

	return move;
}


void insertChip (xsd__string board, conecta3x3ns__tPlayer player, unsigned int column){

	char c;
	unsigned int cell;
	unsigned int found;


		// Init...
		found = FALSE;
		cell = column;

		// Set chip type
		if (player == player1)
			c = PLAYER_1_CHIP;
		else if (player == player2)
			c = PLAYER_2_CHIP;
		else
			c = PLAYER_3_CHIP;


		// Get the right row
		while (!found){

			if (board[cell] == EMPTY_CELL)
				found = TRUE;
			else
				cell += BOARD_WIDTH;

			// re-Check!
			if (cell >= (BOARD_WIDTH * BOARD_HEIGHT))
				showError ("[insertChip] Checking out of bounds!!! :(\n");
		}

		// Insert the chip!
		board[cell] = c;
}


int checkUp (xsd__string board, unsigned int cell, char c){

	int found;

		// Init...
		found = FALSE;

		// Check second chip
		if ((cell+BOARD_WIDTH) < (BOARD_WIDTH * BOARD_HEIGHT))
			found = (board[cell+BOARD_WIDTH] == c);

		// Check third chip
		if (((cell+(2*BOARD_WIDTH)) < (BOARD_WIDTH * BOARD_HEIGHT)) && found)
			found = (board[cell+(2*BOARD_WIDTH)] == c);

	return found;
}


int checkRight (xsd__string board, unsigned int cell, char c){

	int found;
	int limit;

		// Init...
		found = FALSE;
		limit = ((cell/BOARD_WIDTH) +1) * BOARD_WIDTH;

		// Check second chip
		if ((cell+1) < limit)
			found = (board[cell+1] == c);

		// Check third chip
		if (((cell+2) < limit) && found)
			found = (board[cell+2] == c);

	return found;
}


int checkUpLeft (xsd__string board, unsigned int cell, char c){

	int found;

		// Init...
		found = FALSE;

		if (((cell%BOARD_WIDTH) >= 3) && (cell < (BOARD_WIDTH*3))){

			// Check second chip
			if ((cell+BOARD_WIDTH-1) < (BOARD_WIDTH * BOARD_HEIGHT))
				found = (board[cell+BOARD_WIDTH-1] == c);

			// Check third chip
			if (((cell+(2*BOARD_WIDTH)-2) < (BOARD_WIDTH * BOARD_HEIGHT)) && found)
				found = (board[cell+(2*BOARD_WIDTH)-2] == c);
		}

	return found;
}


int checkUpRight (xsd__string board, unsigned int cell, char c){

	int found;

		// Init...
		found = FALSE;

		if (((cell%BOARD_WIDTH) <= 3) && (cell < (BOARD_WIDTH*3))){

			// Check second chip
			if ((cell+BOARD_WIDTH+1) < (BOARD_WIDTH * BOARD_HEIGHT))
				found = (board[cell+BOARD_WIDTH+1] == c);

			// Check third chip
			if (((cell+(2*BOARD_WIDTH)+2) < (BOARD_WIDTH * BOARD_HEIGHT)) && found)
				found = (board[cell+(2*BOARD_WIDTH)+2] == c);
		}

	return found;
}


int checkWinner (xsd__string board, conecta3x3ns__tPlayer player){

	char c;
	unsigned int found, i;

		// Init...
		found = FALSE;
		i = 0;

		// Set chip type
		if (player == player1)
			c = PLAYER_1_CHIP;
		else if (player == player2)
			c = PLAYER_2_CHIP;
		else
			c = PLAYER_3_CHIP;

		// Check every cell in the board
		while ((!found) && (i<(BOARD_WIDTH * BOARD_HEIGHT))){

			// Is this cell occupied by current player?
			if (board[i] == c){

				// Check 4-line chips up
				found = checkUp (board, i, c);

				// Check 4-line chips right
				if (!found)
					found = checkRight (board, i, c);

				// Check 4-line chips upLeft
				if (!found)
					found = checkUpLeft (board, i, c);

				// Check 4-line chips upRight
				if (!found)
					found = checkUpRight (board, i, c);
			}

			// Check next cell
			if (!found)
				i++;
		}

	return found;
}


int isBoardFull (xsd__string board){

	int i;
	int emptyFound;

		// Init...
		i = BOARD_WIDTH*(BOARD_HEIGHT-1);
		emptyFound = FALSE;

		// Check last row
		while ((!emptyFound) && (i<(BOARD_WIDTH*BOARD_HEIGHT))){

			if (board[i] == EMPTY_CELL)
				emptyFound = TRUE;
			else
				i++;
		}

	return (!emptyFound);
}


void allocClearMessage (struct soap *soap, conecta3x3ns__tMessage* msg){    
    msg->msg = (xsd__string) soap_malloc (soap, STRING_LENGTH);
    memset(msg->msg, 0, STRING_LENGTH);
    msg->__size = STRING_LENGTH;
}


void allocClearBlock (struct soap *soap, conecta3x3ns__tBlock* block){
    block->code = -1;
    allocClearMessage (soap, &(block->msgStruct));
    block->board = (xsd__string) soap_malloc (soap, BOARD_WIDTH*BOARD_HEIGHT);
    memset (block->board, 0, BOARD_WIDTH*BOARD_HEIGHT);
    block->__size = BOARD_WIDTH*BOARD_HEIGHT;
}
