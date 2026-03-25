#include "types.h"
#include <mongoose.h>

bool is_waiting_for_move = false;
enum Scene scene         = Lobby;
struct Context ctx       = {};
struct mg_mgr mgr        = {};
struct mg_connection *c  = NULL;
