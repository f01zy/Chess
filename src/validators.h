#ifndef VALIDATORS_H_INCLUDED
#define VALIDATORS_H_INCLUDED

#include "types.h"

bool validate_pawn(enum Color turn, struct Move move);
bool validate_king(struct Move move);
bool validate_queen(struct Move move);
bool validate_rook(struct Move move);
bool validate_bishop(struct Move move);
bool validate_knight(struct Move move);

#endif
