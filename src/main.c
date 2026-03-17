#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
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
  initialize_colors();
  initialize_board();

  do {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
      endwin();
      printf("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
      exit(1);
    }

    render_board();
    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + 6;

    if (is_checkmate(turn)) {
      mvprintw(y++, x, "%s lose", turn == WHITE ? "White" : "Black");
      refresh();
      getch();
      break;
    }

    // TODO: закинуть в отдельную функцию
    char *label = turn == WHITE ? "White" : "Black";
    int color = turn == WHITE ? 2 : 0;
    attron(COLOR_PAIR(color));
    mvprintw(y++, x, "%s turn", label);
    attroff(COLOR_PAIR(color));

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
    enum MoveType move_type = check_move_validity(turn, move);
    if (move_type == MOVE_INVALID) {
      mvprintw(y++, x, "The move is incorrent\n");
      refresh();
      getch();
      continue;
    }

    execute_move(move, move_type);
    save_played_move(move, move_type);
    turn = turn == WHITE ? BLACK : WHITE;
  } while (TRUE);

  endwin();
}
