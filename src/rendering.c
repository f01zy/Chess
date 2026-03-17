#include <ncurses.h>
#include <string.h>
#include <wchar.h>

#include "check.h"
#include "globals.h"
#include "rendering.h"
#include "types.h"

void render_played_moves() {
  int rows, cols;
  int start = MAX(played_moves_count - DRAW_MOVES, 0);
  getmaxyx(stdscr, rows, cols);

  for (int i = 0, j = start; j < played_moves_count && j < rows; i++, j++) {
    struct PlayedMove played_move = played_moves[j];
    char piece;
    char buffer[10];
    char ax = 'a' + played_move.ax;
    char bx = 'a' + played_move.bx;
    char by = '0' + 8 - played_move.by;
    int curr = 0;

    // clang-format off
    switch (played_move.type) {
      case PAWN:   piece = ' '; break;
      case KING:   piece = 'K'; break;
      case QUEEN:  piece = 'Q'; break;
      case ROOK:   piece = 'R'; break;
      case BISHOP: piece = 'B'; break;
      case KNIGHT: piece = 'N'; break;
      case EMPTY:  piece = ' '; break;
    }

    if (played_move.is_castling) {
      strcpy(buffer, "O-O-O\0");
    } else {
      if (piece != ' ') buffer[curr++] = piece;
      if (played_move.type == PAWN && played_move.is_take) buffer[curr++] = ax;
      if (played_move.is_take) buffer[curr++] = 'x';
      buffer[curr++] = bx;
      buffer[curr++] = by;
      if (played_move.is_checkmate) buffer[curr++] = '#';
      else if (played_move.is_check) buffer[curr++] = '+';
      buffer[curr] = '\0';
    }
    // clang-format on

    int x = 2;
    int y = i + 1;
    mvprintw(y, x, "%d. %s", j + 1, buffer);
  }
}

void render_board() {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  bool is_now_check = is_check(turn);
  bool is_now_checkmate = is_checkmate(turn);

  clear();
  render_played_moves();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece piece = board[i][j];
      wchar_t ch = ' ';
      int color = 0;

      if (piece.type == KING && piece.color == turn && is_now_check) {
        color = 1;
      }

      // clang-format off
      switch (piece.type) {
        case PAWN:   ch = piece.color == WHITE ? WHITE_PAWN   : BLACK_PAWN;   break;
        case KING:   ch = piece.color == WHITE ? WHITE_KING   : BLACK_KING;   break;
        case QUEEN:  ch = piece.color == WHITE ? WHITE_QUEEN  : BLACK_QUEEN;  break;
        case ROOK:   ch = piece.color == WHITE ? WHITE_ROOK   : BLACK_ROOK;   break;
        case BISHOP: ch = piece.color == WHITE ? WHITE_BISHOP : BLACK_BISHOP; break;
        case KNIGHT: ch = piece.color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT; break;
        case EMPTY:                                                            break;
      }
      // clang-format on

      int x = cols / 2 - 8 + j * 2;
      int y = rows / 2 - 4 + i;
      attron(COLOR_PAIR(color));
      mvprintw(y, x, "%lc", ch);
      attroff(COLOR_PAIR(color));
      i == 7 && mvprintw(y + 1, x, "%c", 'a' + j);
      j == 0 && mvprintw(y, x - 1, "%d", 8 - i);
    }
  }
  refresh();
}
