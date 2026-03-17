#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "types.h"

void get_king_position(enum Color turn, int *x, int *y);
void initialize_board();
void initialize_colors();
void execute_move(struct Move move, enum MoveType move_type);
void save_played_move(struct Move move, enum MoveType move_type);

#endif
