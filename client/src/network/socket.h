#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include "../types.h"
#include <mongoose.h>

void initialize_mongoose();
void send_move(struct Move move, enum MoveType move_type);
void send_status(char *status);

#endif
