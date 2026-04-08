#include <cJSON.h>
#include <mongoose.h>
#include <ncurses.h>
#include <stdio.h>

#include "../defines.h"
#include "../engine/utility.h"
#include "../globals.h"
#include "socket.h"

uint64_t pending = 0;

static void handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
    cJSON *json              = cJSON_ParseWithLength(wm->data.buf, wm->data.len);
    char *type               = cJSON_GetObjectItem(json, "type")->valuestring;

    if (!strcmp(type, "opponent_found")) {
      enum Color side = cJSON_GetObjectItem(json, "side")->valueint;
      initialize_context(&ctx);
      ctx.side  = side;
      scene     = GAME;
      pending  &= ~WAIT_SEARCHING;
    }

    else if (!strcmp(type, "move")) {
      enum Color side         = cJSON_GetObjectItem(json, "side")->valueint;
      enum MoveType move_type = cJSON_GetObjectItem(json, "move_type")->valueint;
      int ax                  = cJSON_GetObjectItem(json, "ax")->valueint;
      int ay                  = cJSON_GetObjectItem(json, "ay")->valueint;
      int bx                  = cJSON_GetObjectItem(json, "bx")->valueint;
      int by                  = cJSON_GetObjectItem(json, "by")->valueint;
      struct Move move        = {ax, ay, bx, by};
      struct Piece piece      = ctx.board[ay][ax];
      struct Piece victim     = ctx.board[by][bx];

      execute_move(&ctx, move, move_type);
      save_played_move(&ctx, side, move, move_type, piece, victim);
      change_turn(&ctx);
      pending &= ~WAIT_MOVE;
    }

    else if (!strcmp(type, "connected")) {
      pending &= ~WAIT_CONNECT;
    }

    else if (!strcmp(type, "disconnected")) {
      scene    = LOBBY;
      pending &= ~WAIT_DISCONNECT;
    }

    cJSON_Delete(json);
  }
}

void initialize_mongoose(char *server, int port) {
  char buffer[SERVERS_BUFFER_MAX];
  snprintf(buffer, sizeof(buffer), "ws://%s:%d", server, port);
  mg_log_set(MG_LL_NONE);
  mg_mgr_init(&mgr);
  c        = mg_ws_connect(&mgr, buffer, handler, NULL, NULL);
  pending |= WAIT_CONNECT;
  waiting_message("Connecting...", WAIT_CONNECT);
}

void send_move(struct Move move, enum MoveType move_type) {
  cJSON *json = cJSON_CreateObject();

  cJSON_AddStringToObject(json, "type", "move");
  cJSON_AddNumberToObject(json, "side", ctx.side);
  cJSON_AddNumberToObject(json, "move_type", move_type);
  cJSON_AddNumberToObject(json, "ax", move.ax);
  cJSON_AddNumberToObject(json, "ay", move.ay);
  cJSON_AddNumberToObject(json, "bx", move.bx);
  cJSON_AddNumberToObject(json, "by", move.by);

  char *data = cJSON_PrintUnformatted(json);
  mg_ws_send(c, data, strlen(data), WEBSOCKET_OP_TEXT);
  cJSON_Delete(json);
  pending |= WAIT_MOVE;
  waiting_message("Pending move...", WAIT_MOVE);
}

void send_status(char *status, char *message, int mask) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "type", status);

  char *data = cJSON_PrintUnformatted(json);
  mg_ws_send(c, data, strlen(data), WEBSOCKET_OP_TEXT);
  cJSON_Delete(json);
  pending |= mask;
  waiting_message(message, mask);
}

void waiting_message(char *message, int mask) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  int x = cols / 2 - strlen(message) / 2;
  int y = rows / 2;

  attron(COLOR_PAIR(BLACK_ON_WHITE));
  while (pending & mask) {
    mg_mgr_poll(&mgr, 100);
    clear();
    mvprintw(y, x, "%s\n", message);
    refresh();
  }
  attroff(COLOR_PAIR(BLACK_ON_WHITE));
}
