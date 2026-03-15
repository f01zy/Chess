#include <ncurses.h>
#include <stdlib.h>

#include "check.h"
#include "globals.h"
#include "types.h"
#include "utility.h"

void get_king_position(enum Color turn, int *x, int *y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const struct Piece *piece = &board[i][j];
      if (piece->type == KING && piece->color == turn) {
        *y = i;
        *x = j;
        return;
      }
    }
  }
}

void move_struct_to_number(const struct Move *move, int *ax, int *ay, int *bx, int *by) {
  *ax = move->ax;
  *ay = move->ay;
  *bx = move->bx;
  *by = move->by;
}

void initialize_colors() {
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_WHITE, COLOR_YELLOW);
  init_pair(2, COLOR_WHITE, COLOR_RED);
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

void save_played_move(struct Move move) {
  int ax, ay, bx, by;
  move_struct_to_number(&move, &ax, &ay, &bx, &by);
  const struct Piece *piece = &board[ay][ax];
  const struct Piece *victim = &board[by][bx];
  bool is_now_check = is_check(turn);
  bool is_now_checkmate = is_now_check ? is_checkmate() : false;

  struct PlayedMove played_move = {piece->type, is_now_check, is_now_checkmate, false, ax, ay, bx, by};
  played_moves[curr_move++] = played_move;
}
