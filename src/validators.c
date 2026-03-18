#include <stdbool.h>
#include <stdlib.h>

#include "globals.h"
#include "types.h"
#include "validators.h"

enum MoveType validate_pawn(enum Color turn, struct Move move) {
  int dirY = move.ay == move.by ? 0 : move.ay > move.by ? -1 : 1;
  int moveX = abs(move.ax - move.bx);
  int moveY = abs(move.ay - move.by);
  int max_advence = (move.ay == 1 || move.ay == 6) ? 2 : 1;

  struct Piece nextPiece = board[move.ay + dirY][move.ax];
  struct Piece victim = board[move.by][move.bx];

  if ((dirY == 1 && turn == WHITE) || (dirY == -1 && turn == BLACK)) return MOVE_INVALID;
  if (moveY > max_advence || moveY == 0 || moveX > 1) return MOVE_INVALID;
  if (moveY == 2 && (moveX == 1 || nextPiece.type != EMPTY)) return MOVE_INVALID;

  if (moveX == 0 && victim.type != EMPTY) return MOVE_INVALID;
  if (moveX == 1 && victim.type == EMPTY) {
    if (!played_moves_count) return MOVE_INVALID;
    struct PlayedMove previous_move = played_moves[played_moves_count - 1];
    if (previous_move.move_type == MOVE_PAWN_DOUBLE && previous_move.bx == move.bx && previous_move.by == move.ay) return MOVE_EN_PASSANT;
    return MOVE_INVALID;
  }

  return moveY == 2 ? MOVE_PAWN_DOUBLE : MOVE_NORMAL;
};

enum MoveType validate_king(struct Move move) {
  if (abs(move.ax - move.bx) > 1 || abs(move.ay - move.by) > 1) return MOVE_INVALID;
  return MOVE_NORMAL;
};

enum MoveType validate_queen(struct Move move) {
  enum MoveType is_rook = validate_rook(move);
  enum MoveType is_bishop = validate_bishop(move);
  if (is_rook != MOVE_INVALID) return is_rook;
  if (is_bishop != MOVE_INVALID) return is_bishop;
  return MOVE_INVALID;
};

enum MoveType validate_rook(struct Move move) {
  int dirX = move.ax == move.bx ? 0 : move.ax > move.bx ? -1 : 1;
  int dirY = move.ay == move.by ? 0 : move.ay > move.by ? -1 : 1;
  if (abs(dirX) + abs(dirY) != 1) return MOVE_INVALID;

  int i = move.ay, j = move.ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == move.by && j == move.bx) break;
    if (board[i][j].type != EMPTY) return MOVE_INVALID;
  }

  return MOVE_NORMAL;
};

enum MoveType validate_bishop(struct Move move) {
  int dirX = move.ax > move.bx ? -1 : 1;
  int dirY = move.ay > move.by ? -1 : 1;

  int moveX = abs(move.ax - move.bx);
  int moveY = abs(move.ay - move.by);
  if (moveX != moveY) return MOVE_INVALID;

  int i = move.ay, j = move.ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == move.by && j == move.bx) break;
    if (board[i][j].type != EMPTY) return MOVE_INVALID;
  }

  return MOVE_NORMAL;
};

enum MoveType validate_knight(struct Move move) {
  int moveX = abs(move.ax - move.bx);
  int moveY = abs(move.ay - move.by);
  if (!(moveX == 1 && moveY == 2) && !(moveX == 2 && moveY == 1)) return MOVE_INVALID;
  return MOVE_NORMAL;
};
