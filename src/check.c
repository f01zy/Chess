#include "check.h"
#include "globals.h"
#include "types.h"
#include "utility.h"
#include "validators.h"
#include <ncurses.h>

bool is_attacked(enum Color turn, int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 7) { return false; }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      struct Move move = {j, i, x, y};
      enum Color opponent_turn = turn == WHITE ? BLACK : WHITE;

      if (piece.color == turn) { continue; }
      if (check_move_validity(opponent_turn, move) != MOVE_INVALID) {
        printw("from (%d, %d) to (%d, %d)", j, i, x, y);
        return true;
      }
    }
  }

  return false;
}

bool is_check(enum Color turn) {
  int x, y;
  get_king_position(turn, &x, &y);
  return is_attacked(turn, x, y);
}

bool is_checkmate(enum Color turn) {
  if (!is_check(turn)) { return false; }

  enum Color curr_turn = turn;
  enum Color opponent_turn = curr_turn == WHITE ? BLACK : WHITE;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      if (piece.type == EMPTY || piece.color != curr_turn) { continue; }
      for (int k = 0; k < 8; k++) {
        for (int l = 0; l < 8; l++) {
          struct Move move = {j, i, l, k};
          if (check_move_validity(curr_turn, move) == MOVE_INVALID) { continue; }
          struct Piece victim = board[k][l];

          board[k][l] = piece;
          board[i][j] = (struct Piece){EMPTY, WHITE};
          bool is_check_after_move = is_check(curr_turn);
          board[k][l] = victim;
          board[i][j] = piece;

          if (!is_check_after_move) { return false; }
        }
      }
    }
  }

  return true;
}

enum MoveType check_move_validity(enum Color turn, struct Move move) {
  if ((move.ay < 0 || move.ay > 7) || (move.by < 0 || move.by > 7) || (move.ax < 0 || move.ax > 7) || (move.bx < 0 || move.bx > 7)) { return MOVE_INVALID; }
  if (move.ax == move.bx && move.ay == move.by) { return MOVE_INVALID; }

  struct Piece piece = board[move.ay][move.ax];
  struct Piece victim = board[move.by][move.bx];

  if (piece.color != turn) { return MOVE_INVALID; }
  if (victim.color == turn && piece.type == KING && victim.type == ROOK) { return check_castling(turn, move); }
  if (victim.type != EMPTY && victim.color == turn) { return MOVE_INVALID; }

  // clang-format off
  switch (piece.type) {
    case KING:   return is_attacked(turn, move.bx, move.by) ? MOVE_INVALID : validate_king(move);
    case PAWN:   return validate_pawn  (turn, move);
    case QUEEN:  return validate_queen (move);
    case ROOK:   return validate_rook  (move);
    case BISHOP: return validate_bishop(move);
    case KNIGHT: return validate_knight(move);
    default:     return MOVE_INVALID;
  }
  // clang-format on

  return false;
};

enum MoveType check_castling(enum Color turn, struct Move move) {
  if (is_check(turn)) { return false; }

  if ((turn == WHITE && !can_white_castle) || (turn == BLACK && !can_black_castle)) { return MOVE_INVALID; }

  int dirX = move.ax > move.bx ? -1 : 1;
  for (int i = move.ax + dirX; i != move.bx; i += dirX) {
    struct Piece piece = board[move.ay][i];
    if (piece.type != EMPTY || is_attacked(turn, i, move.ay)) { return MOVE_INVALID; }
  }
  return MOVE_CASTLING;
}
