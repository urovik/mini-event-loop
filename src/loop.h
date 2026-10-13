#ifndef LOOP_H
#define LOOP_H


#include <stdint.h>

typedef struct event_loop event_loop_t;

typedef void(*event_cb_t)(event_loop_t* loop, int fd, uint32_t events, void* arg)

#endif