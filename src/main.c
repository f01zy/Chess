#include <locale.h>
#include <ncurses.h>
#include <string.h>

#include "check.h"
#include "defines.h"
#include "globals.h"
#include "rendering.h"
#include "types.h"
#include "utility.h"

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  initialize_context(&ctx);
  initialize_colors();

  do {
    clear();
    int rows, cols;
    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + 6;
    getmaxyx(stdscr, rows, cols);

    if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
      printw("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
      refresh();
      getch();
      continue;
    }

    render(&ctx);
    if (is_checkmate(&ctx, ctx.turn)) {
      mvprintw(y++, x, "%s lose", ctx.turn == WHITE ? "White" : "Black");
      refresh();
      getch();
      break;
    }

    mvprintw(y++, x, "Coordinates: ");
    refresh();
    char buffer[5];
    echo();
    int count = getnstr(buffer, sizeof(buffer));
    noecho();
    if (strcmp(buffer, "q") == 0) { break; }
    sscanf(buffer, "%c%d-%c%d", &fromX, &fromY, &toX, &toY);

    int ax = fromX - 'a';
    int ay = 8 - fromY;
    int bx = toX - 'a';
    int by = 8 - toY;

    struct Move move = {ax, ay, bx, by};
    if (!check_coordinated_validity(move)) {
      mvprintw(y++, x, "The coordinates is invalid\n");
      refresh();
      getch();
      continue;
    }

    // TODO: закончить и добавить is_protecting
    struct Piece piece      = ctx.board[ay][ax];
    struct Piece victim     = ctx.board[by][bx];
    bool is_castling        = victim.color == ctx.turn && piece.type == KING && victim.type == ROOK;
    enum MoveType move_type = is_castling ? check_castling(&ctx, ctx.turn, move) : check_move_validity(&ctx, ctx.turn, move);

    if (move_type == MOVE_INVALID) {
      mvprintw(y++, x, "The move is incorrent\n");
      refresh();
      getch();
      continue;
    }

    execute_move(&ctx, move, move_type);
    change_turn(&ctx);
  } while (TRUE);

  endwin();
}
