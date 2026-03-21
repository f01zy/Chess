#include "socket.h"
#include "../defines.h"
#include "../globals.h"
#include "mongoose.h"

static void handler(struct mg_connection *c, int ev, void *ev_data) {}

void connect_to_server() {
  mg_mgr_init(&mgr);
  c = mg_ws_connect(&mgr, SERVER_URL, handler, NULL, NULL);
}

void disconnect_from_server() { mg_mgr_free(&mgr); }
