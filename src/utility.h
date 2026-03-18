#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "types.h"

void initialize_colors();
void initialize_context(struct Context *ctx);
void initialize_board(struct Context *ctx);
void change_turn(struct Context *ctx);
void get_king_position(struct Context *ctx, int *x, int *y);
void execute_move(struct Context *ctx, struct Move move, enum MoveType move_type);
void save_played_move(struct Context *ctx, struct Move move, enum MoveType move_type, struct Piece piece, struct Piece victim);

#endif
