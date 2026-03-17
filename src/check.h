#ifndef CHECK_H_INCLUDED
#define CHECK_H_INCLUDED

#include "types.h"

bool is_attacked(enum Color turn, int x, int y);
bool is_check(enum Color turn);
bool is_checkmate(enum Color turn);
enum MoveType check_castling(enum Color turn, struct Move move);
enum MoveType check_move_validity(enum Color turn, struct Move move);

#endif
