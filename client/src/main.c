#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>

#include "game/game.h"

void initialize_colors() {
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_WHITE, COLOR_RED);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
}

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  initialize_colors();
  game_menu();
  endwin();
}
