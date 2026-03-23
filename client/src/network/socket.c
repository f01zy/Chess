#include "socket.h"
#include "../defines.h"
#include "../globals.h"
#include "mongoose.h"
#include <ncurses.h>
#include <string.h>

bool is_done = false;

static void handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
    struct mg_str json       = mg_str(wm->data.buf);
    char *type               = mg_json_get_str(json, "$.type");

    if (!strcmp(type, "opponent_found")) {
      scene = Game;
    } else if (!strcmp(type, "disconnected")) {
      scene = Lobby;
    }
  }
}

void connect_to_server() {
  mg_mgr_init(&mgr);
  c = mg_ws_connect(&mgr, SERVER_URL, handler, &is_done, NULL);
}

void disconnect_from_server() {
  c->is_closing = true;
  mg_mgr_free(&mgr);
  c = NULL;
  mg_mgr_poll(&mgr, 100);
}
void send_move(const char *move) { mg_ws_send(c, move, strlen(move), WEBSOCKET_OP_TEXT); }
