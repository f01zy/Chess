#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include "types.h"

void save_played_move(struct Move move);
void get_king_position(enum Color turn, int *x, int *y);
void initialize_board();
void initialize_colors();
void move_struct_to_number(const struct Move *move, int *ax, int *ay, int *bx, int *by);

#endif
