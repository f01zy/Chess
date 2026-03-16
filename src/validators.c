#include <stdbool.h>
#include <stdlib.h>

#include "globals.h"
#include "utility.h"
#include "validators.h"

bool validate_pawn(enum Color turn, struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  int dirY = ay == by ? 0 : ay > by ? -1 : 1;
  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  int max_advence = (ay == 1 || ay == 6) ? 2 : 1;

  if ((dirY == 1 && turn == WHITE) || (dirY == -1 && turn == BLACK)) {
    return false;
  }
  if (moveY > max_advence || moveY == 0 || moveX > 1) {
    return false;
  }

  const struct Piece *nextPiece = &board[ay + dirY][ax];
  if (moveY == 2 && (moveX == 1 || nextPiece->type != EMPTY)) {
    return false;
  }

  const struct Piece *victim = &board[by][bx];
  if ((moveX == 0 && victim->type != EMPTY) || (moveX == 1 && victim->type == EMPTY)) {
    return false;
  }

  return true;
};

bool validate_king(struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  if (abs(ax - bx) > 1 || abs(ay - by) > 1) {
    return false;
  }
  return true;
};

bool validate_queen(struct Move move) { return validate_rook(move) || validate_bishop(move); };

bool validate_rook(struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  int dirX = ax == bx ? 0 : ax > bx ? -1 : 1;
  int dirY = ay == by ? 0 : ay > by ? -1 : 1;
  if (abs(dirX) + abs(dirY) != 1) {
    return false;
  }

  int i = ay, j = ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == by && j == bx) {
      break;
    }
    if (board[i][j].type != EMPTY) {
      return false;
    }
  }

  return true;
};

bool validate_bishop(struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  int dirX = ax > bx ? -1 : 1;
  int dirY = ay > by ? -1 : 1;

  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  if (moveX != moveY) {
    return false;
  }

  int i = ay, j = ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == by && j == bx) {
      break;
    }
    if (board[i][j].type != EMPTY) {
      return false;
    }
  }

  return true;
};

bool validate_knight(struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  if (!(moveX == 1 && moveY == 2) && !(moveX == 2 && moveY == 1)) {
    return false;
  }
  return true;
};
