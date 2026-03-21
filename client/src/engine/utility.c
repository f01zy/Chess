#include <ncurses.h>
#include <string.h>

#include "../types.h"
#include "check.h"
#include "utility.h"

void initialize_context(struct Context *ctx) {
  memset(ctx, 0, sizeof(struct Context));
  ctx->turn               = WHITE;
  ctx->played_moves_count = 0;
  ctx->can_white_castle   = true;
  ctx->can_black_castle   = true;
  initialize_board(ctx);
}

void get_king_position(struct Context *ctx, enum Color side, int *x, int *y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = ctx->board[i][j];
      if (piece.type == KING && piece.color == side) {
        *y = i;
        *x = j;
        return;
      }
    }
  }
}

void change_turn(struct Context *ctx) { ctx->turn = ctx->turn == WHITE ? BLACK : WHITE; }

void initialize_board(struct Context *ctx) {
  for (int i = 0; i < 8; i++) {
    ctx->board[1][i] = (struct Piece){PAWN, BLACK};
    ctx->board[6][i] = (struct Piece){PAWN, WHITE};
  }
  ctx->board[0][0] = ctx->board[0][7] = (struct Piece){ROOK, BLACK};
  ctx->board[0][1] = ctx->board[0][6] = (struct Piece){KNIGHT, BLACK};
  ctx->board[0][2] = ctx->board[0][5] = (struct Piece){BISHOP, BLACK};
  ctx->board[0][3]                    = (struct Piece){QUEEN, BLACK};
  ctx->board[0][4]                    = (struct Piece){KING, BLACK};
  ctx->board[7][0] = ctx->board[7][7] = (struct Piece){ROOK, WHITE};
  ctx->board[7][1] = ctx->board[7][6] = (struct Piece){KNIGHT, WHITE};
  ctx->board[7][2] = ctx->board[7][5] = (struct Piece){BISHOP, WHITE};
  ctx->board[7][3]                    = (struct Piece){QUEEN, WHITE};
  ctx->board[7][4]                    = (struct Piece){KING, WHITE};
}

void execute_move(struct Context *ctx, struct Move move, enum MoveType move_type) {
  struct Piece piece  = ctx->board[move.ay][move.ax];
  struct Piece victim = ctx->board[move.by][move.bx];

  if (move_type == MOVE_CASTLING || piece.type == KING || piece.type == ROOK) {
    ctx->turn == WHITE ? (ctx->can_white_castle = false) : (ctx->can_black_castle = false);
  }

  if (move_type == MOVE_CASTLING) {
    int king_new_x                  = move.bx == 0 ? 2 : 6;
    int rook_new_x                  = move.bx == 0 ? 3 : 5;
    ctx->board[move.by][king_new_x] = piece;
    ctx->board[move.by][rook_new_x] = victim;
    ctx->board[move.by][move.bx]    = (struct Piece){EMPTY, WHITE};
    ctx->board[move.ay][move.ax]    = (struct Piece){EMPTY, WHITE};
  } else {
    ctx->board[move.by][move.bx] = piece;
    ctx->board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
    if (move_type == MOVE_EN_PASSANT) ctx->board[move.ay][move.bx] = (struct Piece){EMPTY, WHITE};
  }
}

void undo_move(struct Context *ctx, struct Move move, enum MoveType move_type, struct Piece piece, struct Piece victim) {
  enum Color opponent = ctx->turn == WHITE ? BLACK : WHITE;
  if (move_type == MOVE_CASTLING || piece.type == KING || piece.type == ROOK) {
    ctx->turn == WHITE ? (ctx->can_white_castle = true) : (ctx->can_black_castle = true);
  }

  ctx->board[move.ay][move.ax] = piece;
  ctx->board[move.by][move.bx] = victim;

  if (move_type == MOVE_CASTLING) {
    int king_x                  = move.bx == 0 ? 2 : 6;
    int rook_x                  = move.bx == 0 ? 3 : 5;
    ctx->board[move.by][king_x] = (struct Piece){EMPTY, WHITE};
    ctx->board[move.by][rook_x] = (struct Piece){EMPTY, WHITE};
  } else if (move_type == MOVE_EN_PASSANT) {
    ctx->board[move.ay][move.bx] = (struct Piece){PAWN, opponent};
  }
}

void save_played_move(struct Context *ctx, struct Move move, enum MoveType move_type, struct Piece piece, struct Piece victim) {
  enum Color opponent = ctx->turn == WHITE ? BLACK : WHITE;
  struct PlayedMove played_move;
  played_move.turn                             = ctx->turn;
  played_move.piece_type                       = piece.type;
  played_move.move_type                        = move_type;
  played_move.is_check                         = is_check(ctx, opponent);
  played_move.is_checkmate                     = is_checkmate(ctx, opponent);
  played_move.is_take                          = victim.type != EMPTY;
  played_move.is_castling                      = move_type == MOVE_CASTLING;
  played_move.ax                               = move.ax;
  played_move.ay                               = move.ay;
  played_move.bx                               = move.bx;
  played_move.by                               = move.by;
  ctx->played_moves[ctx->played_moves_count++] = played_move;
}
