#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "defines.h"
#include <stdbool.h>

enum PieceType { EMPTY, PAWN, KING, QUEEN, ROOK, BISHOP, KNIGHT };
enum Color { WHITE, BLACK };
enum MoveType { MOVE_NORMAL, MOVE_PAWN_DOUBLE, MOVE_EN_PASSANT, MOVE_CASTLING, MOVE_PROMOTION, MOVE_INVALID };
enum Scene { EXIT, CHOOSE_SERVER, LOBBY, SEARCHING, GAME };

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
  enum Color side;
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

struct Context {
  enum Color side;
  enum Color turn;
  struct Piece board[8][8];
  struct PlayedMove played_moves[MAX_MOVES];
  int played_moves_count;
  bool can_white_castle;
  bool can_black_castle;
};

#endif
