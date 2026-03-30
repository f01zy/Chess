#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include "types.h"
#include <mongoose.h>

extern enum Scene scene;
extern struct Context ctx;
extern struct mg_mgr mgr;
extern struct mg_connection *c;

#endif
