#include <ctype.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../globals.h"
#include "../types.h"
#include "check.h"
#include "mongoose.h"
#include "utility.h"

void initalize_colors() {
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

void initialize_context(struct Context *ctx) {
  memset(ctx, 0, sizeof(struct Context));
  ctx->side               = WHITE;
  ctx->turn               = WHITE;
  ctx->played_moves_count = 0;
  ctx->can_white_castle   = true;
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

  if (move_type == MOVE_CASTLING) {
    int king_new_x                  = move.bx == 0 ? 2 : 6;
    int rook_new_x                  = move.bx == 0 ? 3 : 5;
    ctx->board[move.by][king_new_x] = piece;
    ctx->board[move.by][rook_new_x] = victim;
    ctx->board[move.by][move.bx]    = (struct Piece){EMPTY, WHITE};
    ctx->board[move.ay][move.ax]    = (struct Piece){EMPTY, WHITE};
  } else if (move_type == MOVE_EN_PASSANT) {
    ctx->board[move.by][move.bx] = piece;
    ctx->board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
    ctx->board[move.ay][move.bx] = (struct Piece){EMPTY, WHITE};
  } else if (move_type == MOVE_PROMOTION) {
    ctx->board[move.by][move.bx] = (struct Piece){QUEEN, ctx->turn};
    ctx->board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
  } else {
    ctx->board[move.by][move.bx] = piece;
    ctx->board[move.ay][move.ax] = (struct Piece){EMPTY, WHITE};
  }
}

void undo_move(struct Context *ctx, struct Move move, enum MoveType move_type, struct Piece piece, struct Piece victim) {
  enum Color opponent = ctx->turn == WHITE ? BLACK : WHITE;

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

void save_played_move(struct Context *ctx, enum Color side, struct Move move, enum MoveType move_type, struct Piece piece, struct Piece victim) {
  struct PlayedMove played_move;
  enum Color opponent                          = side == WHITE ? BLACK : WHITE;
  played_move.side                             = side;
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

bool get_move(struct Move *move) {
  int fromY, toY;
  char fromX, toX;
  char buffer[6]          = "";
  char *error             = "";
  int curr                = 0;
  bool is_first_rendering = true;
  nodelay(stdscr, TRUE);
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int x = cols / 2 - 8;
  int y = rows / 2 + 6;

  while (1) {
    mg_mgr_poll(&mgr, 0);
    if (scene != GAME) {
      nodelay(stdscr, FALSE);
      return false;
    }

    int ch = getch();
    if (ch == ERR && !is_first_rendering) continue;
    if (ch == KEY_BACKSPACE && curr > 0) buffer[--curr] = '\0';
    is_first_rendering = false;

    if (ch == ENTER) {
      int count        = sscanf(buffer, "%c%d-%c%d", &fromX, &fromY, &toX, &toY);
      struct Move temp = {fromX - 'a', 8 - fromY, toX - 'a', 8 - toY};

      if (count != 4) {
        error = "Format must be x1y1-x2y2";
      } else if (!check_coordinates_validity(temp)) {
        error = "Coordinates isn't valid";
      } else {
        nodelay(stdscr, FALSE);
        *move = temp;
        break;
      }

      memset(buffer, 0, sizeof(buffer));
      curr = 0;
    }

    if (isprint(ch) && curr < sizeof(buffer) - 1) {
      buffer[curr++] = ch;
      buffer[curr]   = '\0';
    }

    move(y, x);
    if (strlen(error)) {
      printw("%s\n", error);
      error = "";
    } else {
      printw("Coordinates: %s", buffer);
    }
    clrtoeol();
    refresh();
    usleep(10000);
  }

  return true;
}
