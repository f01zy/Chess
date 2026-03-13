/*
 * TODO
 * 1. fix is_check method.
 */

#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MIN_WIDTH    14
#define MIN_HEIGHT   14
#define ESCAPE       27
#define WHITE_PAWN   L'\u2659'
#define WHITE_KING   L'\u2654'
#define WHITE_QUEEN  L'\u2655'
#define WHITE_ROOK   L'\u2656'
#define WHITE_BISHOP L'\u2657'
#define WHITE_KNIGHT L'\u2658'
#define BLACK_PAWN   L'\u265F'
#define BLACK_KING   L'\u265A'
#define BLACK_QUEEN  L'\u265B'
#define BLACK_ROOK   L'\u265C'
#define BLACK_BISHOP L'\u265D'
#define BLACK_KNIGHT L'\u265E'

enum PieceType { EMPTY, PAWN, KING, QUEEN, ROOK, BISHOP, KNIGHT };
enum Color { WHITE, BLACK };

struct Piece {
  enum PieceType type;
  enum Color color;
};

struct Piece board[8][8];
enum Color turn = WHITE;
int rows, cols;

void get_king_position(int *x, int *y);
void initialize_board();
void draw();
void initialize_colors();

bool is_attacked(int x, int y);
bool is_check();
bool is_checkmate();
bool check_move_validity(int ax, int ay, int bx, int by);

bool validate_pawn(int ax, int ay, int bx, int by);
bool validate_king(int ax, int ay, int bx, int by);
bool validate_queen(int ax, int ay, int bx, int by);
bool validate_rook(int ax, int ay, int bx, int by);
bool validate_bishop(int ax, int ay, int bx, int by);
bool validate_knight(int ax, int ay, int bx, int by);

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  raw();
  keypad(stdscr, TRUE);
  initialize_colors();
  initialize_board();

  getmaxyx(stdscr, rows, cols);
  if (cols < MIN_WIDTH || rows < MIN_HEIGHT) {
    endwin();
    printf("Your terminal too little. Minimum size is %dx%d\n", MIN_WIDTH, MIN_HEIGHT);
    exit(1);
  }

  do {
    draw();

    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + 6;

    if (is_checkmate()) {
      mvprintw(y++, x, "%s lose", turn == WHITE ? "White" : "Black");
      refresh();
      getch();
      break;
    }

    mvprintw(y++, x, "Coordinates: ");
    refresh();

    char buffer[5];
    echo();
    int count = getnstr(buffer, sizeof(buffer));
    noecho();

    if (count == ERR) {
      continue;
    }

    if (strcmp(buffer, "q") == 0) {
      break;
    }

    sscanf(buffer, "%c%d-%c%d", &fromX, &fromY, &toX, &toY);

    int ax = fromX - 'a';
    int ay = 8 - fromY;
    int bx = toX - 'a';
    int by = 8 - toY;

    if (!check_move_validity(ax, ay, bx, by)) {
      mvprintw(y++, x, "The move is incorrent\n");
      getch();
      continue;
    }

    board[by][bx] = board[ay][ax];
    board[ay][ax] = (struct Piece){EMPTY, WHITE};
    turn = turn == WHITE ? BLACK : WHITE;
  } while (TRUE);

  endwin();
}

void initialize_colors() {
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();

  // Default
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  // Check
  init_pair(1, COLOR_WHITE, COLOR_RED);
}

bool check_move_validity(int ax, int ay, int bx, int by) {
  if ((ay < 0 || ay > 7) || (by < 0 || by > 7) || (ax < 0 || ax > 7) || (bx < 0 || bx > 7)) {
    return false;
  }

  const struct Piece *piece = &board[ay][ax];
  const struct Piece *victim = &board[by][bx];
  if (piece->type == EMPTY || piece->color != turn || (victim->type != EMPTY && victim->color == turn)) {
    return false;
  }
  if (is_check() && piece->type != KING) {
    return false;
  }
  if (ax == bx && ay == by) {
    return false;
  }

  if (piece->type == PAWN) {
    return validate_pawn(ax, ay, bx, by);
  } else if (piece->type == KING) {
    return validate_king(ax, ay, bx, by) && !is_attacked(bx, by);
  } else if (piece->type == QUEEN) {
    return validate_queen(ax, ay, bx, by);
  } else if (piece->type == ROOK) {
    return validate_rook(ax, ay, bx, by);
  } else if (piece->type == BISHOP) {
    return validate_bishop(ax, ay, bx, by);
  } else if (piece->type == KNIGHT) {
    return validate_knight(ax, ay, bx, by);
  }
  return false;
};

void draw() {
  clear();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      struct Piece *piece = &board[i][j];
      wchar_t ch = ' ';
      int color = 0;

      if (piece->type == PAWN) {
        ch = piece->color == WHITE ? WHITE_PAWN : BLACK_PAWN;
      } else if (piece->type == KING) {
        ch = piece->color == WHITE ? WHITE_KING : BLACK_KING;
      } else if (piece->type == QUEEN) {
        ch = piece->color == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
      } else if (piece->type == ROOK) {
        ch = piece->color == WHITE ? WHITE_ROOK : BLACK_ROOK;
      } else if (piece->type == BISHOP) {
        ch = piece->color == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
      } else if (piece->type == KNIGHT) {
        ch = piece->color == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
      }

      if (is_check()) {
        color = 1;
      }

      int x = cols / 2 - 8 + j * 2;
      int y = rows / 2 - 4 + i;
      attron(COLOR_PAIR(color));
      mvprintw(y, x, "%lc", ch);
      attroff(COLOR_PAIR(color));
      i == 7 && mvprintw(y + 1, x, "%c", 'a' + j);
      j == 0 && mvprintw(y, x - 1, "%d", 8 - i);
    }
  }
  refresh();
}

void initialize_board() {
  for (int i = 0; i < 8; i++) {
    board[1][i] = (struct Piece){PAWN, BLACK};
    board[6][i] = (struct Piece){PAWN, WHITE};
  }

  board[0][0] = board[0][7] = (struct Piece){ROOK, BLACK};
  board[0][1] = board[0][6] = (struct Piece){KNIGHT, BLACK};
  board[0][2] = board[0][5] = (struct Piece){BISHOP, BLACK};
  board[0][3] = (struct Piece){QUEEN, BLACK};
  board[0][4] = (struct Piece){KING, BLACK};

  board[7][0] = board[7][7] = (struct Piece){ROOK, WHITE};
  board[7][1] = board[7][6] = (struct Piece){KNIGHT, WHITE};
  board[7][2] = board[7][5] = (struct Piece){BISHOP, WHITE};
  board[7][3] = (struct Piece){QUEEN, WHITE};
  board[7][4] = (struct Piece){KING, WHITE};
};

bool validate_pawn(int ax, int ay, int bx, int by) {
  int dirY = ay == by ? 0 : ay > by ? -1 : 1;
  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  int max_advence = (ay == 1 || ay == 6) ? 2 : 1;

  if ((dirY == 1 && turn == WHITE) || (dirY == -1 && turn == BLACK)) {
    return false;
  }
  if (moveY > max_advence || moveX > 1) {
    return false;
  }

  const struct Piece *nextPiece = &board[ay + dirY][ax];
  if (moveY == 2 && (moveX == 1 || nextPiece->type != EMPTY)) {
    return false;
  }

  const struct Piece *victim = &board[by][bx];
  if ((moveX == 0 && victim->type != EMPTY) || (moveX == 1 && victim->type == EMPTY)) {
    return false;
  }

  return true;
};

bool validate_king(int ax, int ay, int bx, int by) {
  const struct Piece *victim = &board[by][bx];
  if (victim->type != EMPTY && victim->color == turn) {
    return false;
  }
  if (abs(ax - bx) > 1 || abs(ay - by) > 1) {
    return false;
  }
  return true;
};

bool validate_queen(int ax, int ay, int bx, int by) { return validate_rook(ax, ay, bx, by) || validate_bishop(ax, ay, bx, by); };

bool validate_rook(int ax, int ay, int bx, int by) {
  int dirX = ax == bx ? 0 : ax > bx ? -1 : 1;
  int dirY = ay == by ? 0 : ay > by ? -1 : 1;
  if (abs(dirX) + abs(dirY) != 1) {
    return false;
  }

  int i = ay, j = ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == by && j == bx) {
      break;
    }
    if (board[i][j].type != EMPTY) {
      return false;
    }
  }

  return true;
};

bool validate_bishop(int ax, int ay, int bx, int by) {
  int dirX = ax > bx ? -1 : 1;
  int dirY = ay > by ? -1 : 1;

  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  if (moveX != moveY) {
    return false;
  }

  int i = ay, j = ax;
  while (1) {
    i += dirY;
    j += dirX;
    if (i == by && j == bx) {
      break;
    }
    if (board[i][j].type != EMPTY) {
      return false;
    }
  }

  return true;
};

bool validate_knight(int ax, int ay, int bx, int by) {
  int moveX = abs(ax - bx);
  int moveY = abs(ay - by);
  if (!(moveX == 1 && moveY == 2) && !(moveX == 2 && moveY == 1)) {
    return false;
  }
  return true;
};

void get_king_position(int *x, int *y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const struct Piece *piece = &board[i][j];
      if (piece->type == KING && piece->color == turn) {
        *y = i;
        *x = j;
        return;
      }
    }
  }
}

bool is_attacked(int x, int y) {
  if (x < 0 || x > 7 || y < 0 || y > 8) {
    return false;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const struct Piece *piece = &board[i][j];
      if ((i == y && j == x) || piece->color == turn) {
        continue;
      }
      if (check_move_validity(j, i, x, y)) {
        return true;
      }
    }
  }

  return false;
}

bool is_check() {
  int x, y;
  get_king_position(&x, &y);
  return is_attacked(x, y);
}

bool is_checkmate() {
  int x, y;
  get_king_position(&x, &y);
  return is_attacked(x - 1, y) || is_attacked(x + 1, y) || is_attacked(x, y - 1) || is_attacked(x, y + 1) || is_attacked(x - 1, y - 1) ||
         is_attacked(x + 1, y + 1) || is_attacked(x - 1, y + 1) || is_attacked(x + 1, y - 1);
}
