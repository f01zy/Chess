#include "globals.h"

int curr_move = 0;
struct PlayedMove played_moves[MAX_MOVES] = {{EMPTY, false, false, false, 0, 0, 0, 0}};
struct Piece board[8][8] = {{EMPTY, WHITE}};
enum Color turn = WHITE;
