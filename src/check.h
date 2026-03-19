#ifndef CHECK_H_INCLUDED
#define CHECK_H_INCLUDED

#include "types.h"

bool is_attacked(struct Context *ctx, enum Color side, int x, int y);
bool is_check(struct Context *ctx, enum Color side);
bool is_checkmate(struct Context *ctx, enum Color side);

bool check_coordinates_validity(struct Move move);
enum MoveType check_castling(struct Context *ctx, enum Color side, struct Move move);
enum MoveType check_move_validity(struct Context *ctx, enum Color side, struct Move move);

#endif
