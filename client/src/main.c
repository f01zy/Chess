#include <locale.h>
#include <ncurses.h>

#include "engine/utility.h"
#include "game/game.h"
#include "globals.h"
#include "network/socket.h"
#include "types.h"

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  initalize_colors();
  initialize_mongoose();

  while (scene != EXIT) {
    // clang-format off
    switch (scene) {
      case LOBBY:     lobby();     break;
      case SEARCHING: searching(); break;
      case GAME:      game();      break;
      case EXIT:                   break;
    }
    // clang-format on
  }

  mg_mgr_free(&mgr);
  endwin();
}
