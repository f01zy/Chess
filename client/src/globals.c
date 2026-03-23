#include "types.h"
#include <mongoose.h>

enum Scene scene        = Lobby;
struct Context ctx      = {};
struct mg_mgr mgr       = {};
struct mg_connection *c = NULL;
