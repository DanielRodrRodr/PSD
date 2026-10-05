#include "game.h"

void initBoard (tBoard board){

	int i;

		for (i=0; i<(BOARD_WIDTH * BOARD_HEIGHT); i++)
			board[i] = (char) EMPTY_CELL;
}


void showError(const char *msg){
	perror(msg);
	exit(0);
}


void printBoard (tBoard board, char* message){

	int i, row, cell;

		// Show message
		printf("\n%s\n", message);

		// Print column numbers
		for (i=0; i<BOARD_WIDTH; i++)
			printf ("  %d ", i);

		printf ("\n");

		// Draw each row
		for (row = (BOARD_HEIGHT - 1); row>=0; row--){

			// Current cell in the board
			cell = row * BOARD_WIDTH;

			// Separator line
			for (i=0; i<BOARD_WIDTH;i++)
				printf ("|---");

			printf ("|\n");

			// Print a row of cells
			for (i=cell; i<(cell+BOARD_WIDTH); i++){
				printf("| %c ",board[i]);
			}

			// End of the row!
			printf ("|\n");
		}

		// Print the base
		printf ("|---");
		for (i=1; i<BOARD_WIDTH-1;i++)
			printf ("----");
		printf ("----|\n\n\n");

}


tMove checkMove (tBoard board, unsigned int column){

	tMove move;

		// Move is outside the board
		if (column>=BOARD_WIDTH)
			move = wrongColumn_move;

		// Column is full
		else if (board[(BOARD_WIDTH*(BOARD_HEIGHT-1)) + column] != EMPTY_CELL)
			move = fullColumn_move;

		// Correct move!
		else
			move = OK_move;

	return move;
}


tMove insertChip (tBoard board, tPlayer player, unsigned int column){

	char c;
	unsigned int cell;
	unsigned int found;
	tMove move;

		// Init...
		found = FALSE;
		cell = column;

		// Check move
		move = checkMove (board, column);

		if (move == OK_move){

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
			}

			// Insert the chip!
			board[cell] = c;
		}

	return move;
}


int checkUp (tBoard board, unsigned int cell, char c){

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


int checkRight (tBoard board, unsigned int cell, char c){

	int found, limit;

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


int checkUpLeft (tBoard board, unsigned int cell, char c){

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


int checkUpRight (tBoard board, unsigned int cell, char c){

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


int checkWinner (tBoard board, tPlayer player){

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


int isBoardFull (tBoard board){

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
