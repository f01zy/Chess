#ifndef VALIDATORS_H_INCLUDED
#define VALIDATORS_H_INCLUDED

#include "types.h"

enum MoveType validate_pawn(enum Color turn, struct Move move);
enum MoveType validate_king(struct Move move);
enum MoveType validate_queen(struct Move move);
enum MoveType validate_rook(struct Move move);
enum MoveType validate_bishop(struct Move move);
enum MoveType validate_knight(struct Move move);

#endif
