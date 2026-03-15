#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdbool.h>

enum PieceType { EMPTY, PAWN, KING, QUEEN, ROOK, BISHOP, KNIGHT };
enum Color { WHITE, BLACK };

struct Piece {
  enum PieceType type;
  enum Color color;
};

struct Move {
  int ax;
  int ay;
  int bx;
  int by;
};

struct PlayedMove {
  enum PieceType type;
  bool is_check;
  bool is_checkmate;
  bool is_take;
  int ax;
  int ay;
  int bx;
  int by;
};

#endif
