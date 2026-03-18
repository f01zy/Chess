#ifndef CHECK_H_INCLUDED
#define CHECK_H_INCLUDED

#include "types.h"

bool is_attacked(struct Context *ctx, int x, int y);
bool is_protecting(struct Context *ctx, struct Move move);
bool is_check(struct Context *ctx);
bool is_checkmate(struct Context *ctx);
enum MoveType check_castling(struct Context *ctx, struct Move move);
enum MoveType check_move_validity(struct Context *ctx, struct Move move);

#endif
