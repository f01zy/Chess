#include "globals.h"

enum Scene scene        = CHOOSE_SERVER;
struct Context ctx      = {};
struct mg_mgr mgr       = {};
struct mg_connection *c = NULL;
