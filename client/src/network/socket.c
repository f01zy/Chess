#include <cJSON.h>
#include <mongoose.h>
#include <ncurses.h>

#include "../defines.h"
#include "../engine/utility.h"
#include "../globals.h"
#include "socket.h"

bool is_connected = false;
uint64_t pending  = 0;

static void handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
    cJSON *json              = cJSON_ParseWithLength(wm->data.buf, wm->data.len);
    char *type               = cJSON_GetObjectItem(json, "type")->valuestring;

    if (!strcmp(type, "opponent_found")) {
      enum Color side = cJSON_GetObjectItem(json, "side")->valueint;
      initialize_context(&ctx);
      ctx.side  = side;
      scene     = Game;
      pending  &= ~WAIT_SEARCHING;
    }

    if (!strcmp(type, "move")) {
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

    if (!strcmp(type, "connected")) { is_connected = true; }
    if (!strcmp(type, "disconnected")) {
      scene    = Lobby;
      pending &= ~WAIT_DISCONNECT;
    }

    cJSON_Delete(json);
  }
}

void initialize_mongoose() {
  mg_log_set(MG_LL_NONE);
  mg_mgr_init(&mgr);
  c = mg_ws_connect(&mgr, SERVER_URL, handler, NULL, NULL);
  while (!is_connected) {
    mg_mgr_poll(&mgr, 100);
  }
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
  while (pending &= WAIT_MOVE) {
    mg_mgr_poll(&mgr, 100);
  }
}

void send_status(char *status, uint64_t mask) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "type", status);

  char *data = cJSON_PrintUnformatted(json);
  mg_ws_send(c, data, strlen(data), WEBSOCKET_OP_TEXT);
  cJSON_Delete(json);

  pending |= mask;
  while (pending &= mask) {
    mg_mgr_poll(&mgr, 100);
    clear();
    printw("Pending...");
    refresh();
  }
}
