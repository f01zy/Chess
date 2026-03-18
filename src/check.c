#include "check.h"
#include "types.h"
#include "utility.h"
#include "validators.h"

bool is_attacked(struct Context *ctx, int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 7) return false;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = ctx->board[i][j];
      struct Move move = {j, i, x, y};
      if (piece.color == ctx->turn) continue;

      change_turn(ctx);
      // TODO: тут падает код на validate_pawn. ошибка не в валидаторе. без этой хуйни все работает норм.
      if (check_move_validity(ctx, move) != MOVE_INVALID) return true;
      change_turn(ctx);
    }
  }

  return false;
}

bool is_check(struct Context *ctx) {
  int x, y;
  get_king_position(ctx, &x, &y);
  return is_attacked(ctx, x, y);
}

bool is_checkmate(struct Context *ctx) {
  if (!is_check(ctx)) return false;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = ctx->board[i][j];
      if (piece.type == EMPTY || piece.color != ctx->turn) continue;
      for (int k = 0; k < 8; k++) {
        for (int l = 0; l < 8; l++) {
          struct Move move = {j, i, l, k};
          if (check_move_validity(ctx, move) == MOVE_INVALID) continue;
          struct Piece victim = ctx->board[k][l];

          ctx->board[k][l] = piece;
          ctx->board[i][j] = (struct Piece){EMPTY, WHITE};
          bool is_check_after_move = is_check(ctx);
          ctx->board[k][l] = victim;
          ctx->board[i][j] = piece;

          if (!is_check_after_move) return false;
        }
      }
    }
  }

  return true;
}

enum MoveType check_move_validity(struct Context *ctx, struct Move move) {
  if ((move.ay < 0 || move.ay > 7) || (move.by < 0 || move.by > 7) || (move.ax < 0 || move.ax > 7) || (move.bx < 0 || move.bx > 7)) return MOVE_INVALID;
  if (move.ax == move.bx && move.ay == move.by) return MOVE_INVALID;

  struct Piece piece = ctx->board[move.ay][move.ax];
  struct Piece victim = ctx->board[move.by][move.bx];

  if (piece.color != ctx->turn) return MOVE_INVALID;
  if (victim.color == ctx->turn && piece.type == KING && victim.type == ROOK) return check_castling(ctx, move);
  if (victim.type != EMPTY && victim.color == ctx->turn) return MOVE_INVALID;

  // clang-format off
  switch (piece.type) {
    case KING:   return is_check       (ctx) ? MOVE_INVALID : validate_king(ctx, move);
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

enum MoveType check_castling(struct Context *ctx, struct Move move) {
  if (is_check(ctx)) return MOVE_INVALID;
  if ((ctx->turn == WHITE && !ctx->can_white_castle) || (ctx->turn == BLACK && !ctx->can_black_castle)) return MOVE_INVALID;

  int dirX = move.ax > move.bx ? -1 : 1;
  for (int i = move.ax + dirX; i != move.bx; i += dirX) {
    struct Piece piece = ctx->board[move.ay][i];
    if (piece.type != EMPTY || is_attacked(ctx, i, move.ay)) return MOVE_INVALID;
  }
  return MOVE_CASTLING;
}
