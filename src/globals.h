#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "types.h"

#define MAX(A, B)    ((A) > (B) ? (A) : (B))
#define DRAW_MOVES   4
#define MAX_MOVES    1024
#define MIN_WIDTH    20
#define MIN_HEIGHT   20
#define ESCAPE       27
#define WHITE_PAWN   L'\u2659'
#define WHITE_KING   L'\u2654'
#define WHITE_QUEEN  L'\u2655'
#define WHITE_ROOK   L'\u2656'
#define WHITE_BISHOP L'\u2657'
#define WHITE_KNIGHT L'\u2658'
#define BLACK_PAWN   L'\u265F'
#define BLACK_KING   L'\u265A'
#define BLACK_QUEEN  L'\u265B'
#define BLACK_ROOK   L'\u265C'
#define BLACK_BISHOP L'\u265D'
#define BLACK_KNIGHT L'\u265E'

extern int curr_move;
extern struct PlayedMove played_moves[MAX_MOVES];
extern struct Piece board[8][8];
extern enum Color turn;

#endif
