#include <mongoose.h>
#include <ncurses.h>
#include <stdio.h>

#include "../defines.h"
#include "../engine/utility.h"
#include "../globals.h"
#include "socket.h"

bool is_connected = false;

static void handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
    struct mg_str json       = mg_str(wm->data.buf);
    char *type               = mg_json_get_str(json, "$.type");
    if (!strcmp(type, "opponent_found")) {
      enum Color side = mg_json_get_long(json, "$.side", 0);
      initialize_context(&ctx);
      ctx.side = side;
      scene    = Game;
    }
    if (!strcmp(type, "move")) {
      is_waiting_for_move = false;
      change_turn(&ctx);
      enum Color side = mg_json_get_long(json, "$.side", 0);
      if (side == ctx.side) { return; }

      enum MoveType move_type = mg_json_get_long(json, "$.move_type", 0);
      long ax                 = mg_json_get_long(json, "$.ax", 0);
      long ay                 = mg_json_get_long(json, "$.ay", 0);
      long bx                 = mg_json_get_long(json, "$.bx", 0);
      long by                 = mg_json_get_long(json, "$.by", 0);
      struct Move move        = {ax, ay, bx, by};
      execute_move(&ctx, move, move_type);
    }
    if (!strcmp(type, "connected")) { is_connected = true; }
    if (!strcmp(type, "disconnected")) { scene = Lobby; }
  }
}

void initialize_mongoose() {
  mg_log_set(MG_LL_NONE);
  mg_mgr_init(&mgr);
  c = mg_ws_connect(&mgr, SERVER_URL, handler, NULL, NULL);
  while (!is_connected) {
    mg_mgr_poll(&mgr, 100);
    clear();
    printw("Connecting...");
    refresh();
  }
}

void send_move(struct Move move, enum MoveType move_type) {
  char buffer[256];
  snprintf(buffer, sizeof(buffer), "{\"type\":\"move\",\"side\":%d,\"move_type\":%d,\"ax\":%d,\"ay\":%d,\"bx\":%d,\"by\":%d}", ctx.side, move_type, move.ax,
           move.ay, move.bx, move.by);
  mg_ws_send(c, buffer, strlen(buffer), WEBSOCKET_OP_TEXT);

  is_waiting_for_move = true;
  while (is_waiting_for_move) {
    mg_mgr_poll(&mgr, 100);
  }
}

void send_status(char *status) {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "{\"type\":\"%s\"}", status);
  mg_ws_send(c, buffer, strlen(buffer), WEBSOCKET_OP_TEXT);
}
