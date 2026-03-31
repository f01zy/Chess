#include "globals.h"

enum Scene scene        = LOBBY;
struct Context ctx      = {};
struct mg_mgr mgr       = {};
struct mg_connection *c = NULL;
