#include "check.h"
#include "globals.h"
#include "utility.h"
#include "validators.h"

bool is_attacked(enum Color turn, int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 8) {
    return false;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const struct Piece *piece = &board[i][j];
      if (piece->color == turn) {
        continue;
      }
      if (check_move_validity(turn, (struct Move){j, i, x, y})) {
        return true;
      }
    }
  }

  return false;
}

bool is_check() {
  int x, y;
  get_king_position(&x, &y);
  return is_attacked(turn, x, y);
}

bool is_checkmate() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const struct Piece piece = board[i][j];
      if (piece.type == EMPTY || piece.type == KING || piece.color != turn) {
        continue;
      }
      enum Color opponentColor = turn == WHITE ? BLACK : WHITE;
      for (int k = 0; k < 8; k++) {
        for (int l = 0; l < 8; l++) {
          struct Move move = {j, i, l, k};
          // TODO: fix turn
          if (!check_turn(move, opponentColor) || !check_move_validity(opponentColor, move)) {
            continue;
          }
          const struct Piece victim = board[k][l];

          board[k][l] = piece;
          board[i][j] = (struct Piece){EMPTY, WHITE};
          bool is_right = !is_check();
          board[k][l] = victim;
          board[i][j] = piece;

          if (is_right) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool check_turn(struct Move move, enum Color turn) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  const struct Piece *piece = &board[ay][ax];
  const struct Piece *victim = &board[by][bx];
  if (piece->color != turn || (victim->type != EMPTY && victim->color == turn)) {
    return false;
  }
  return true;
}

bool check_move_validity(enum Color turn, struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  if ((ay < 0 || ay > 7) || (by < 0 || by > 7) || (ax < 0 || ax > 7) || (bx < 0 || bx > 7) || (ax == bx && ay == by)) {
    return false;
  }
  const struct Piece *piece = &board[ay][ax];

  // clang-format off
  switch (piece->type) {
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
