#include <ncurses.h>
#include <stdlib.h>

#include "check.h"
#include "globals.h"
#include "types.h"
#include "utility.h"

void get_king_position(enum Color turn, int *x, int *y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      if (piece.type == KING && piece.color == turn) {
        *y = i;
        *x = j;
        return;
      }
    }
  }
}

void initialize_colors() {
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_WHITE, COLOR_RED);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
}

void initialize_board() {
  for (int i = 0; i < 8; i++) {
    board[1][i] = (struct Piece){PAWN, BLACK};
    board[6][i] = (struct Piece){PAWN, WHITE};
  }
  board[0][0] = board[0][7] = (struct Piece){ROOK, BLACK};
  board[0][1] = board[0][6] = (struct Piece){KNIGHT, BLACK};
  board[0][2] = board[0][5] = (struct Piece){BISHOP, BLACK};
  board[0][3] = (struct Piece){QUEEN, BLACK};
  board[0][4] = (struct Piece){KING, BLACK};
  board[7][0] = board[7][7] = (struct Piece){ROOK, WHITE};
  board[7][1] = board[7][6] = (struct Piece){KNIGHT, WHITE};
  board[7][2] = board[7][5] = (struct Piece){BISHOP, WHITE};
  board[7][3] = (struct Piece){QUEEN, WHITE};
  board[7][4] = (struct Piece){KING, WHITE};
}

void execute_move(struct Move move, enum MoveType move_type) {
  struct Piece piece = board[move.ay][move.ax];
  struct Piece victim = board[move.by][move.bx];

  if (move_type == MOVE_CASTLING || piece.type == KING || piece.type == ROOK) { turn == WHITE ? (can_white_castle = false) : (can_black_castle = false); }

  if (move_type == MOVE_CASTLING) {
    int king_new_x = move.bx == 0 ? 2 : 6;
    int rook_new_x = move.bx == 0 ? 3 : 5;
    board[move.by][king_new_x] = piece;
    board[move.by][rook_new_x] = victim;
    board[move.by][move.bx] = (struct Piece){EMPTY, WHITE};
    board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
  } else if (move_type == MOVE_EN_PASSANT) {
    int dirY = turn == WHITE ? -1 : 1;
    int eaten_pawn_y = move.ay - dirY;
    board[move.by][move.bx] = piece;
    board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
    board[eaten_pawn_y][move.bx] = (struct Piece){EMPTY, WHITE};
  } else if (move_type == MOVE_NORMAL) {
    board[move.by][move.bx] = piece;
    board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
  }
}

void save_played_move(struct Move move, enum MoveType move_type) {
  struct Piece piece = board[move.ay][move.ax];
  struct Piece victim = board[move.by][move.bx];
  enum Color opponent = turn == WHITE ? BLACK : WHITE;

  bool is_now_check = is_check(opponent);
  bool is_now_checkmate = is_checkmate(opponent);
  bool is_take = victim.type == EMPTY ? false : true;
  bool is_castling = move_type == MOVE_CASTLING;

  struct PlayedMove played_move;
  played_move.turn = turn;
  played_move.piece_type = piece.type;
  played_move.move_type = move_type;
  played_move.is_check = is_now_check;
  played_move.is_checkmate = is_now_checkmate;
  played_move.is_take = is_take;
  played_move.is_castling = is_castling;
  played_move.ax = move.ax;
  played_move.ay = move.ay;
  played_move.bx = move.bx;
  played_move.by = move.by;
  played_moves[played_moves_count++] = played_move;
}
