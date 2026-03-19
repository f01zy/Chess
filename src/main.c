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
    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + 6;

    if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
      printw("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
      refresh();
      getch();
      continue;
    }

    render(&ctx);
    mvprintw(y++, x, "Coordinates: ");
    refresh();

    if (is_checkmate(&ctx, ctx.turn)) {
      mvprintw(y++, x, "%s lose", ctx.turn == WHITE ? "White" : "Black");
      refresh();
      getch();
      break;
    }

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
    if (!check_coordinates_validity(move)) {
      error_message(x, y++, "The coordinates is invalid");
      continue;
    }

    struct Piece piece  = ctx.board[ay][ax];
    struct Piece victim = ctx.board[by][bx];
    enum MoveType move_type;

    if (victim.color == ctx.turn && piece.type == KING && victim.type == ROOK) {
      move_type = check_castling(&ctx, ctx.turn, move);
    } else {
      move_type = check_move_validity(&ctx, ctx.turn, move);
    }

    if (move_type == MOVE_INVALID) {
      error_message(x, y++, "The move is invalid");
      continue;
    }

    execute_move(&ctx, move, move_type);
    if (is_check(&ctx, ctx.turn)) {
      undo_move(&ctx, move, move_type, piece, victim);
      error_message(x, y++, "So you are in check");
      continue;
    }

    save_played_move(&ctx, move, move_type, piece, victim);
    change_turn(&ctx);
  } while (TRUE);

  endwin();
}
