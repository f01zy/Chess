#include "globals.h"

enum Color turn = WHITE;
struct Piece board[8][8] = {{EMPTY, WHITE}};
struct PlayedMove played_moves[MAX_MOVES] = {{EMPTY, false, false, false, 0, 0, 0, 0}};
int played_moves_count = 0;
bool can_white_castle = true;
bool can_black_castle = true;
