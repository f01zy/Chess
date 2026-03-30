#include "ui.h"

#include <ncurses.h>
#include <string.h>
#include <unistd.h>

// TODO: решить проблему с мерцанием
int menu(char *options[MAX_MENU_OPTIONS], int size) {
  int option = 0, c = 0;
  do {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (c == KEY_DOWN) option++;
    if (c == KEY_UP) option--;
    option = CLAMP(option, 0, size - 1);

    int max_length = 0;
    for (int i = 0; i < size; i++) {
      size_t length = strlen(options[i]);
      max_length    = MAX(max_length, length);
    }

    for (int i = 0; i < size; i++) {
      int x     = cols / 2 - max_length / 2;
      int y     = rows / 2 - size / 2 + i;
      int color = i == option ? BLACK_ON_WHITE : WHITE_ON_BLACK;
      attron(COLOR_PAIR(color));
      mvprintw(y, x, "%s\n", options[i]);
      attroff(COLOR_PAIR(color));
    }
    refresh();
    usleep(10000);
  } while ((c = getch()) != ENTER);

  return option;
}
