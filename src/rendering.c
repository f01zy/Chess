#include <ncurses.h>
#include <wchar.h>

#include "check.h"
#include "globals.h"
#include "rendering.h"

void render() {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  bool is_now_check = is_check();
  bool is_now_checkmate = is_now_check ? is_checkmate() : false;

  clear();
  for (int i = MAX(curr_move - DRAW_MOVES, 0); i <= curr_move; i++) {
    int x = cols / 2 - 8 + i * 2;
    int y = rows / 2 - 6;
    const struct PlayedMove *played_move = &played_moves[i];
    // TODO: played_move drawing
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece *piece = &board[i][j];
      wchar_t ch = ' ';
      int color = 0;

      if (piece->type == KING && piece->color == turn) {
        color = is_now_checkmate ? 2 : is_now_check ? 1 : 0;
      }

      // clang-format off
      switch (piece->type) {
        case PAWN:   ch = piece->color == WHITE ? WHITE_PAWN   : BLACK_PAWN;   break;
        case KING:   ch = piece->color == WHITE ? WHITE_KING   : BLACK_KING;   break;
        case QUEEN:  ch = piece->color == WHITE ? WHITE_QUEEN  : BLACK_QUEEN;  break;
        case ROOK:   ch = piece->color == WHITE ? WHITE_ROOK   : BLACK_ROOK;   break;
        case BISHOP: ch = piece->color == WHITE ? WHITE_BISHOP : BLACK_BISHOP; break;
        case KNIGHT: ch = piece->color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT; break;
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
