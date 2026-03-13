#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>

#define WIDTH        8
#define HEIGHT       8
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

struct Piece board[HEIGHT][WIDTH];
enum Color turn = WHITE;
int rows, cols;

bool check_move_validity(int ax, int ay, int bx, int by);
void initialize_board();
void draw();

// TODO
void validate_pawn(int ax, int ay, int bx, int by);
void validate_king(int ax, int ay, int bx, int by);
void validate_queen(int ax, int ay, int bx, int by);
void validate_rook(int ax, int ay, int bx, int by);
void validate_bishop(int ax, int ay, int bx, int by);
void validate_knight(int ax, int ay, int bx, int by);

int main() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  raw();
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, rows, cols);
  initialize_board();

  do {
    draw();

    char fromX, toX;
    int fromY, toY;
    int x = cols / 2 - 8;
    int y = rows / 2 + HEIGHT / 2 + 2;
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
    int ay = HEIGHT - fromY;
    int bx = toX - 'a';
    int by = HEIGHT - toY;

    if (!check_move_validity(ax, ay, bx, by)) {
      mvprintw(y++, x, "The move is incorrent\n");
      getch();
      continue;
    }

    board[by][bx] = board[ay][ax];
    board[ay][ax] = (struct Piece){EMPTY, WHITE};
  } while (TRUE);

  endwin();
}

bool check_move_validity(int ax, int ay, int bx, int by) {
  if ((ay < 0 || ay >= HEIGHT) || (by < 0 || by >= HEIGHT) ||
      (ax < 0 || ax >= WIDTH) || (bx < 0 || bx >= WIDTH)) {
    return false;
  }
  return true;
};

void draw() {
  clear();
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      struct Piece *piece = &board[i][j];
      wchar_t ch = ' ';

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

      int x = cols / 2 - WIDTH + j * 2;
      int y = rows / 2 - HEIGHT / 2 + i;
      mvprintw(y, x, "%lc", ch);
      i == HEIGHT - 1 && mvprintw(y + 1, x, "%c", 'a' + j);
      j == 0 && mvprintw(y, x - 1, "%d", WIDTH - i);
    }
  }
  refresh();
}

void initialize_board() {
  for (int i = 0; i < WIDTH; i++) {
    board[1][i] = (struct Piece){PAWN, BLACK};
    board[HEIGHT - 2][i] = (struct Piece){PAWN, WHITE};
  }

  board[0][0] = board[0][7] = (struct Piece){ROOK, BLACK};
  board[0][1] = board[0][6] = (struct Piece){KNIGHT, BLACK};
  board[0][2] = board[0][5] = (struct Piece){BISHOP, BLACK};
  board[0][3] = (struct Piece){QUEEN, BLACK};
  board[0][4] = (struct Piece){KING, BLACK};

  board[HEIGHT - 1][0] = board[HEIGHT - 1][7] = (struct Piece){ROOK, WHITE};
  board[HEIGHT - 1][1] = board[HEIGHT - 1][6] = (struct Piece){KNIGHT, WHITE};
  board[HEIGHT - 1][2] = board[HEIGHT - 1][5] = (struct Piece){BISHOP, WHITE};
  board[HEIGHT - 1][3] = (struct Piece){QUEEN, WHITE};
  board[HEIGHT - 1][4] = (struct Piece){KING, WHITE};
};
