#include <mongoose.h>
#include <ncurses.h>
#include <string.h>
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

void searching() {
  connect_to_server();
  while (scene == Searching) {
    clear();
    printw("Searching for opponent...");
    refresh();
    mg_mgr_poll(&mgr, 100);
  }
}

void lobby() {
  char *options[MAX_MENU_OPTIONS] = {"Start", "Exit"};
  while (scene == Lobby) {
    int option = menu(options, 2);
    if (option == 0) scene = Searching;
    if (option == 1) scene = Exit;
  }
}

void game() {
  initialize_context(&ctx);
  while (scene == Game) {
    mg_mgr_poll(&mgr, 100);
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
      mvprintw(y++, x, "%s lose\n", ctx.turn == WHITE ? "White" : "Black");
      refresh();
      getch();
      break;
    }

    char buffer[5];
    echo();
    int count = getnstr(buffer, sizeof(buffer));
    noecho();
    if (strcmp(buffer, "q") == 0) {
      disconnect_from_server();
      scene = Lobby;
      break;
    }
    sscanf(buffer, "%c%d-%c%d", &fromX, &fromY, &toX, &toY);

    int ax = fromX - 'a';
    int ay = 8 - fromY;
    int bx = toX - 'a';
    int by = 8 - toY;

    struct Move move = {ax, ay, bx, by};
    if (!check_coordinates_validity(move)) {
      mvprintw(y++, x, "The coordinates is invalid\n");
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
      mvprintw(y++, x, "The move is invalid\n");
      refresh();
      getch();
      continue;
    }

    execute_move(&ctx, move, move_type);
    if (is_check(&ctx, ctx.turn)) {
      undo_move(&ctx, move, move_type, piece, victim);
      mvprintw(y++, x, "So you are in check\n");
      refresh();
      getch();
      continue;
    }

    save_played_move(&ctx, move, move_type, piece, victim);
    change_turn(&ctx);
    send_move(buffer);
  }
}
