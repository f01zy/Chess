
/*
 * TODO
 * 1. взятие на проходе.
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
    struct PlayedMove played_move = {piece.type, turn, false, false, false, false, ax, ay, bx, by};

    if (!check_move_validity(turn, move)) {
      mvprintw(y++, x, "The move is incorrent\n");
      refresh();
      getch();
      continue;
    }

    // TODO: закинуть в отдельную функцию чтобы было понятнее.
    bool is_castling = piece.color == victim.color && piece.type == KING && victim.type == ROOK;

    if (is_castling || piece.type == KING || piece.type == ROOK) {
      turn == WHITE ? (can_white_castle = false) : (can_black_castle = false);
    }

    if (is_castling) {
      int king_new_x = bx == 0 ? 2 : 6;
      int rook_new_x = bx == 0 ? 3 : 5;
      board[by][king_new_x] = piece;
      board[by][rook_new_x] = victim;
      board[by][bx] = (struct Piece){EMPTY, WHITE};
      board[ay][ax] = (struct Piece){EMPTY, WHITE};
    } else {
      board[by][bx] = piece;
      board[ay][ax] = (struct Piece){EMPTY, WHITE};
    }
    turn = turn == WHITE ? BLACK : WHITE;

    played_move.is_check = is_check(turn);
    played_move.is_checkmate = is_checkmate(turn);
    played_move.is_take = victim.type == EMPTY ? false : true;
    played_move.is_castling = is_castling;
    played_moves[played_moves_count++] = played_move;
  } while (TRUE);

  endwin();
}
