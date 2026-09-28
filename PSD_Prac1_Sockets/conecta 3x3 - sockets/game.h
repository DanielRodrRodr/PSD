#include "gameTypes.h"

/**
 * Inits the board.
 *
 * @param board Board to be initialized.
 */
void initBoard (tBoard board);

/**
 * Shows an error message.
 *
 * @param msg Error message
 */
void showError(const char *msg);

/**
 * Prints a board in the screen
 *
 * @param board	Board of current game
 * @param message Message with info about the game
 */
void printBoard (tBoard board, char* message);

/**
 * Inserts a chip in the board.
 *
 * @param board Board of the current game.
 * @param player Current player.
 * @param column Column where the chip is inserted. *
 * @return Resulting move. OK_move if the chip can be inserted in the provided column,
 * wrongColumn_move if the provided column is not valid or fullColumn_move if the indicated
 * column is full.
 */
tMove insertChip (tBoard board, tPlayer player, unsigned int column);

/**
 * Checks if player is the winner.
 *
 * @param board Board of the current game.
 * @param player Current player. *
 * @return TRUE if player is the winner or FALSE in other case.
 */
int checkWinner (tBoard board, tPlayer player);

/**
 * Checks if the board is full.
 *
 * @param board Board of the current game.
 * @return TRUE if board is full or FALSE in other case.
 */
int isBoardFull (tBoard board);
