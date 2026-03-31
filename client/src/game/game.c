#include <mongoose.h>
#include <ncurses.h>
#include <unistd.h>

#include "../defines.h"
#include "../engine/check.h"
#include "../engine/utility.h"
#include "../globals.h"
#include "../network/socket.h"
#include "../types.h"
#include "game.h"
#include "rendering.h"
#include "ui.h"

void searching() { send_status("searching", "Searching for an opponent...", WAIT_SEARCHING); }

void lobby() {
  char *options[MAX_MENU_OPTIONS] = {"Start", "Exit"};
  while (scene == LOBBY) {
    int option = menu(options, 2);
    if (option == 0) scene = SEARCHING;
    if (option == 1) scene = EXIT;
  }
}

void game() {
  while (scene == GAME) {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int x = cols / 2 - 8;
    int y = rows / 2 + 7;

    if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
      printw("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
      refresh();
      getch();
      continue;
    }

    if (ctx.turn != ctx.side) {
      render(&ctx);
      refresh();
      while (scene == GAME && ctx.turn != ctx.side) {
        mg_mgr_poll(&mgr, 100);
      }
      clear();
    }
    if (scene != GAME) break;

    if (is_checkmate(&ctx, ctx.side)) {
      send_status("disconnect_from_room", "Disconnecting...", WAIT_DISCONNECT);
      break;
    }

    render(&ctx);
    int ax, ay, bx, by;
    if (!get_coordinates(&ax, &ay, &bx, &by)) {
      send_status("disconnect_from_room", "Disconnecting...", WAIT_DISCONNECT);
      break;
    }

    struct Move move = {ax, ay, bx, by};
    if (!check_coordinates_validity(move)) {
      mvprintw(y, x, "The coordinates is invalid\n");
      refresh();
      getch();
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
      mvprintw(y, x, "The move is invalid\n");
      refresh();
      getch();
      continue;
    }

    if (is_protected(&ctx, move, move_type)) {
      mvprintw(y, x, "So you are in check\n");
      refresh();
      getch();
      continue;
    }

    if (move_type == MOVE_CASTLING || piece.type == KING || piece.type == ROOK) {
      ctx.turn == WHITE ? (ctx.can_white_castle = false) : (ctx.can_black_castle = false);
    }

    send_move(move, move_type);
  }
}
