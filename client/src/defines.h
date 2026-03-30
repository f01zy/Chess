#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

// #define SERVER_IP        "193.58.121.55"
#define SERVER_IP        "localhost"
#define SERVER_PORT      "3000"
#define SERVER_URL       "ws://" SERVER_IP ":" SERVER_PORT

#define MAX_MENU_OPTIONS 10
#define DRAW_MOVES       12
#define MAX_MOVES        1000
#define MIN_WIDTH        20
#define MIN_HEIGHT       20
#define ESCAPE           27
#define ENTER            10
#define BACKSPACE        127

#define WHITE_ON_BLACK   0
#define WHITE_ON_RED     1
#define BLACK_ON_WHITE   2

#define WHITE_PAWN       L'\u2659'
#define WHITE_KING       L'\u2654'
#define WHITE_QUEEN      L'\u2655'
#define WHITE_ROOK       L'\u2656'
#define WHITE_BISHOP     L'\u2657'
#define WHITE_KNIGHT     L'\u2658'
#define BLACK_PAWN       L'\u265F'
#define BLACK_KING       L'\u265A'
#define BLACK_QUEEN      L'\u265B'
#define BLACK_ROOK       L'\u265C'
#define BLACK_BISHOP     L'\u265D'
#define BLACK_KNIGHT     L'\u265E'

#define WAIT_CONNECT     (1 << 0)
#define WAIT_DISCONNECT  (1 << 1)
#define WAIT_MOVE        (1 << 2)
#define WAIT_SEARCHING   (1 << 3)

#define MAX(A, B)        ((A) > (B) ? (A) : (B))
#define MIN(A, B)        ((A) < (B) ? (A) : (B))
#define CLAMP(A, B, C)   ((A) < (B) ? (B) : (A) > (C) ? (C) : (A))

#endif
