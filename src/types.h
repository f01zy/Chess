#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdbool.h>

enum PieceType { EMPTY, PAWN, KING, QUEEN, ROOK, BISHOP, KNIGHT };
enum Color { WHITE, BLACK };
enum MoveType { MOVE_NORMAL, MOVE_PAWN_DOUBLE, MOVE_EN_PASSANT, MOVE_CASTLING, MOVE_PROMOTION, MOVE_INVALID };

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
  enum Color turn;
  enum PieceType piece_type;
  enum MoveType move_type;
  bool is_check;
  bool is_checkmate;
  bool is_take;
  bool is_castling;
  int ax;
  int ay;
  int bx;
  int by;
};

#endif
