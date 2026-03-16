#include "check.h"
#include "globals.h"
#include "types.h"
#include "utility.h"
#include "validators.h"

#include <ncurses.h>

bool is_attacked(enum Color turn, int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 7) {
    return false;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      struct Move move = {j, i, x, y};
      enum Color opponent_turn = turn == WHITE ? BLACK : WHITE;

      if (piece.color == turn) {
        continue;
      }

      if (check_move_validity(opponent_turn, move)) {
        printw("(%d, %d) atacked (%d, %d)\n", j, i, x, y);
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
  if (!is_check(turn)) {
    return false;
  }

  enum Color curr_turn = turn;
  enum Color opponent_turn = curr_turn == WHITE ? BLACK : WHITE;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      if (piece.type == EMPTY || piece.color != curr_turn) {
        continue;
      }
      for (int k = 0; k < 8; k++) {
        for (int l = 0; l < 8; l++) {
          struct Move move = {j, i, l, k};
          if (!check_move_validity(curr_turn, move)) {
            continue;
          }
          struct Piece victim = board[k][l];

          board[k][l] = piece;
          board[i][j] = (struct Piece){EMPTY, WHITE};
          bool is_check_after_move = is_check(curr_turn);
          board[k][l] = victim;
          board[i][j] = piece;

          if (!is_check_after_move) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool check_move_validity(enum Color turn, struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);

  if ((ay < 0 || ay > 7) || (by < 0 || by > 7) || (ax < 0 || ax > 7) || (bx < 0 || bx > 7) || (ax == bx && ay == by)) {
    return false;
  }
  struct Piece piece = board[ay][ax];
  struct Piece victim = board[by][bx];

  if (piece.color != turn) {
    return false;
  }

  if (victim.color == turn && piece.type == KING && victim.type == ROOK) {
    return check_castling(turn, move);
  }

  if (victim.type != EMPTY && victim.color == turn) {
    return false;
  }

  // clang-format off
  switch (piece.type) {
    case PAWN:   return validate_pawn  (turn, move);
    case KING:   return validate_king  (move) && !is_attacked(turn, bx, by);
    case QUEEN:  return validate_queen (move);
    case ROOK:   return validate_rook  (move);
    case BISHOP: return validate_bishop(move);
    case KNIGHT: return validate_knight(move);
    default:     return false;
  }
  // clang-format on

  return false;
};

// TODO: щас работает только формат король-ладья, надо сделать и обратный.
bool check_castling(enum Color turn, struct Move move) {
  if (is_check(turn)) {
    return false;
  }

  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);

  for (int i = 0; i < played_moves_count; i++) {
    struct PlayedMove played_move = played_moves[i];
    bool is_king = played_move.type == KING;
    bool is_rook = played_move.type == ROOK && played_move.ax == move.ax && played_move.ay == move.ay;
    if (played_move.turn == turn && (is_king || is_rook)) {
      return false;
    }
  }

  int dirX = ax > bx ? -1 : 1;
  for (int i = ax + dirX; i != bx; i += dirX) {
    struct Piece piece = board[ay][i];
    if (piece.type != EMPTY || is_attacked(turn, i, ay)) {
      printw("(%d, %d) atacked\n", i, ay);
      return false;
    }
  }
  return true;
}
