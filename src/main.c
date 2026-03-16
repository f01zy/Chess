
/*
 * TODO
 * 1. взятие на проходе.
 * 2. рокировка.
 * 3. онлайн.
 */

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

  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
    endwin();
    printf("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
    exit(1);
  }

  do {
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

    // TODO: создать функцию перевода Color в строку.
    mvprintw(y++, x, "Coordinates (%s turn): ", turn == WHITE ? "white" : "black");
    refresh();
    char buffer[5];
    echo();
    int count = getnstr(buffer, sizeof(buffer));
    noecho();

    if (strcmp(buffer, "q") == 0) {
      break;
    }

    sscanf(buffer, "%c%d-%c%d", &fromX, &fromY, &toX, &toY);
    int ax = fromX - 'a';
    int ay = 8 - fromY;
    int bx = toX - 'a';
    int by = 8 - toY;

    struct Piece piece = board[ay][ax];
    struct Piece victim = board[by][bx];
    struct Move move = {ax, ay, bx, by};

    struct PlayedMove played_move;
    played_move.type = piece.type;
    played_move.turn = turn;
    played_move.ax = ax;
    played_move.ay = ay;
    played_move.bx = bx;
    played_move.by = by;

    if (!check_move_validity(turn, move)) {
      mvprintw(y++, x, "The move is incorrent\n");
      getch();
      continue;
    }

    // TODO: закинуть в отдельную функцию чтобы было понятнее.
    board[by][bx] = piece;
    board[ay][ax] = (struct Piece){EMPTY, WHITE};
    if (piece.color == victim.color) {
      // TODO: правильно переставить ладью и короля.
    }
    turn = turn == WHITE ? BLACK : WHITE;

    played_move.is_check = is_check(turn);
    played_move.is_checkmate = is_checkmate(turn);
    played_move.is_take = victim.type == EMPTY ? false : true;
    played_moves[played_moves_count++] = played_move;
  } while (TRUE);

  endwin();
}
