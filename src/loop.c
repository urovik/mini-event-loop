#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "loop.h"
#define MAX_EVENTS 64

typedef struct loop_entry{
    int fd;
    uint32_t events;
    event_cb_t callback;
    void* arg;
} loop_entry;


struct event_loop{
    int epfd;
    loop_entry entry[1024];
    int running;

};

static loop_entry* find_entry(event_loop_t *loop, int fd){
    for(int i = 0; i < 1024; i++){
        if(loop->entry[i].fd == fd){
            return &loop->entry[i];
        }
    }
    return NULL;
}

static struct loop_entry *find_free(event_loop_t *l) {
    for (int i = 0; i < 1024; i++) {
        if (l->entry[i].fd == 0) return &l->entry[i];
    }
    return NULL;
}

event_loop_t* loop_create(void){
    event_loop_t* loop = malloc(sizeof(event_loop_t));
    if (!loop){
        return NULL;
    }
    loop->epfd = epoll_create1(0);
    if (loop->epfd < 0){
        free(loop);
        return NULL;
    }
    loop->running = 0;
    return loop;
}

int loop_run(event_loop_t *loop){
    struct epoll_event events[MAX_EVENTS];
    loop->running = 1;

    while (loop->running){

        int n = epoll_wait(loop->epfd, events, MAX_EVENTS, -1);
        if (n < 0){
            if(errno = EINTR) continue;
            return -1;

        }

        if (n > 0){
            for(int i = 0; i < n;  i++){
                int fd = events[i].data.fd;
                uint32_t mask = events[i].events;

                loop_entry* e = find_entry(loop, fd);
                if (!e || !e->callback) continue;

                event_cb_t cb = e->callback;
                void *arg = e->arg;
                cb(loop, fd, mask, arg);
            }
        }
        
    }
    return 0;
}

void loop_destroy(event_loop_t *l) {
    if (!l) return;
    close(l->epfd);
    free(l);
}

void loop_stop(event_loop_t *l) {
    l->running = 0;
}
    




int loop_add(event_loop_t *loop, int fd, uint32_t events, event_cb_t cb, void *arg){
    struct epoll_event e = {0};
    e.data.fd = fd;
    e.events = events;

    if(epoll_ctl(loop->epfd, EPOLL_CTL_ADD, fd, &e) < 0){
        return -1;
    }

    loop_entry* entry = find_free(loop);
    if (!entry) {        
        epoll_ctl(loop->epfd, EPOLL_CTL_DEL, fd, NULL);
        return -1;

    entry->fd = fd;
    entry->events = events;
    entry->callback = cb;
    entry->arg = arg;
    }
    return 0;
}

int loop_mod(event_loop_t *loop, int fd, uint32_t events){
    struct epoll_event e = {0};
    e.data.fd = fd;
    e.events = events;
    
    if(epoll_ctl(loop->epfd,EPOLL_CTL_MOD, fd, &e) < 0){
        return -1;
    }
    loop_entry* entry = find_entry(loop, fd);
    if(entry == NULL){
        return -1;
    }
    entry->fd = fd;
    entry->events |= events;
    return 0;

}

int loop_del(event_loop_t *loop, int fd){
    if(epoll_ctl(loop->epfd,EPOLL_CTL_DEL, fd, NULL) < 0){
        return -1;
    }
    loop_entry* entry = find_entry(loop, fd);
    if(entry == NULL){
        return -1;
    }
    memset(entry, 0, sizeof(*entry));
    return 0;
}


