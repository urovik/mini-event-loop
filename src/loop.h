#ifndef LOOP_H
#define LOOP_H


#include <stdint.h>

typedef struct event_loop event_loop_t;

typedef void(*event_cb_t)(event_loop_t* loop, int fd, uint32_t events, void* arg);

event_loop_t* loop_create(void);

void          loop_destroy(event_loop_t *loop);

// добавить/удалить fd
int loop_add(event_loop_t *loop, int fd, uint32_t events, event_cb_t cb, void *arg);
int loop_mod(event_loop_t *loop, int fd, uint32_t events);
int loop_del(event_loop_t *loop, int fd);

// запустить цикл (блокирующий)
int loop_run(event_loop_t *loop);

// остановить
void loop_stop(event_loop_t *loop);

#endif