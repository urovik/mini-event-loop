#include <loop.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 64;

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

event_loop_t* loop_create(void){
    event_loop_t* loop = calloc(1,sizeof(*l));
    if (!loop){
        return NULL;
    }
    loop->epfd = epoll_create1(0);
    if (loop->epfd < 0){
        free(loop)
        return NULL;
    }
    loop->running = 0;
    return loop;
}

static loop_entry* find_entry(event_loop_t *loop, int fd){
    for(int = 0; i < 1024; i++){
        if(loop->entry[i].fd == fd){
            return &loop->entry[i];
        }
    }
    return NULL;
}

static struct loop_entry *find_free(event_loop_t *l) {
    for (int i = 0; i < 1024; i++) {
        if (l->entries[i].fd == 0) return &l->entries[i];
    }
    return NULL;
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
    return 0;
}

