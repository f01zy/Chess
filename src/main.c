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
    getmaxyx(stdscr, rows, cols);
    if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
      printw("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
      refresh();
      getch();
      continue;
    }

    char *color_label = ctx.turn == WHITE ? "White" : "Black";
    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + 6;

    render_board(&ctx);
    int color = ctx.turn == WHITE ? 2 : 0;
    attron(COLOR_PAIR(color));
    mvprintw(y++, x, "%s turn", color_label);
    attroff(COLOR_PAIR(color));

    if (is_checkmate(&ctx, ctx.turn)) {
      mvprintw(y++, x, "%s lose", color_label);
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
    enum MoveType move_type = check_move_validity(&ctx, ctx.turn, move);
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
