#include <fcntl.h>
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

void choose_server() {
  FILE *file = fopen("servers.txt", "r");
  if (!file) {
    endwin();
    printf("File servers.txt not found\n");
    exit(1);
  }

  char *servers[MAX_SERVERS];
  char buffer[SERVERS_BUFFER_MAX];
  int i = 0;
  while (i < MAX_MENU_OPTIONS && fgets(buffer, sizeof(buffer), file)) {
    buffer[strcspn(buffer, "\n")] = '\0';
    servers[i]                    = malloc(strlen(buffer) + 1);
    if (servers[i] != NULL) {
      strcpy(servers[i], buffer);
      i++;
    }
  }

  int id = menu(servers, i);
  initialize_mongoose(servers[id], 3000);
  scene = LOBBY;
}

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

    if (is_checkmate(&ctx, ctx.turn)) {
      send_status("disconnect_from_room", "Disconnecting...", WAIT_DISCONNECT);
      break;
    }

    if (scene != GAME) break;
    render(&ctx);
    struct Move move;
    if (!get_move(&move)) {
      send_status("disconnect_from_room", "Disconnecting...", WAIT_DISCONNECT);
      break;
    }

    struct Piece piece  = ctx.board[move.ay][move.ax];
    struct Piece victim = ctx.board[move.by][move.bx];
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

    if (is_protected(&ctx, ctx.turn, move, move_type)) {
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
