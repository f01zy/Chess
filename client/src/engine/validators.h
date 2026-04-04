#ifndef VALIDATORS_H_INCLUDED
#define VALIDATORS_H_INCLUDED

#include "../types.h"

enum MoveType validate_pawn(struct Context *ctx, enum Color side, struct Move move);
enum MoveType validate_king(struct Context *ctx, struct Move move);
enum MoveType validate_queen(struct Context *ctx, struct Move move);
enum MoveType validate_rook(struct Context *ctx, struct Move move);
enum MoveType validate_bishop(struct Context *ctx, struct Move move);
enum MoveType validate_knight(struct Context *ctx, struct Move move);

#endif
