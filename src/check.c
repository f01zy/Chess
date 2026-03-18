#include "check.h"
#include "types.h"
#include "utility.h"
#include "validators.h"

bool is_attacked(struct Context *ctx, enum Color side, int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 7) return false;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece  = ctx->board[i][j];
      struct Move move    = {j, i, x, y};
      enum Color opponent = side == WHITE ? BLACK : WHITE;
      if (piece.color == side) continue;
      if (check_move_validity(ctx, opponent, move) != MOVE_INVALID) return true;
    }
  }

  return false;
}

// bool is_protecting(struct Context *ctx, enum Color side, struct Move move) {
//   struct Piece piece = ctx->board[move.ay][move.ax];
//   struct Piece victim = ctx->board[move.by][move.bx];
//   ctx->board[move.by][move.bx] = piece;
//   ctx->board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
//   bool is_protecting = is_check(ctx, side);
//   ctx->board[move.by][move.bx] = victim;
//   ctx->board[move.ay][move.ax] = piece;
//   return is_protecting;
// }

bool is_check(struct Context *ctx, enum Color side) {
  int x, y;
  get_king_position(ctx, side, &x, &y);
  return is_attacked(ctx, side, x, y);
}

bool is_checkmate(struct Context *ctx, enum Color side) {
  if (!is_check(ctx, side)) return false;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = ctx->board[i][j];
      if (piece.type == EMPTY || piece.color != side) continue;
      for (int k = 0; k < 8; k++) {
        for (int l = 0; l < 8; l++) {
          struct Move move = {j, i, l, k};
          if (check_move_validity(ctx, side, move) == MOVE_INVALID) continue;
          struct Piece victim = ctx->board[k][l];

          ctx->board[k][l]         = piece;
          ctx->board[i][j]         = (struct Piece){EMPTY, WHITE};
          bool is_check_after_move = is_check(ctx, side);
          ctx->board[k][l]         = victim;
          ctx->board[i][j]         = piece;

          if (!is_check_after_move) return false;
        }
      }
    }
  }

  return true;
}

bool check_coordinated_validity(struct Move move) {
  if ((move.ay < 0 || move.ay > 7) || (move.by < 0 || move.by > 7) || (move.ax < 0 || move.ax > 7) || (move.bx < 0 || move.bx > 7)) return false;
  if (move.ax == move.bx && move.ay == move.by) return false;
  return true;
}

enum MoveType check_move_validity(struct Context *ctx, enum Color side, struct Move move) {
  // TODO: везде, где используется структура Move сделать доп проверку через check_coordinated_validity
  struct Piece piece  = ctx->board[move.ay][move.ax];
  struct Piece victim = ctx->board[move.by][move.bx];

  if (piece.color != side) return MOVE_INVALID;
  if (victim.type != EMPTY && victim.color == side) return MOVE_INVALID;

  // clang-format off
  switch (piece.type) {
    case KING:   return validate_king  (ctx, move);
    case PAWN:   return validate_pawn  (ctx, move);
    case QUEEN:  return validate_queen (ctx, move);
    case ROOK:   return validate_rook  (ctx, move);
    case BISHOP: return validate_bishop(ctx, move);
    case KNIGHT: return validate_knight(ctx, move);
    default:     return MOVE_INVALID;
  }
  // clang-format on

  return false;
};

enum MoveType check_castling(struct Context *ctx, enum Color side, struct Move move) {
  if ((side == WHITE && !ctx->can_white_castle) || (side == BLACK && !ctx->can_black_castle)) return MOVE_INVALID;
  int dirX = move.ax > move.bx ? -1 : 1;
  for (int i = move.ax; i != move.bx; i += dirX) {
    struct Piece piece = ctx->board[move.ay][i];
    if ((piece.type != EMPTY && piece.type != KING) || is_attacked(ctx, side, i, move.ay)) return MOVE_INVALID;
  }
  return MOVE_CASTLING;
}
