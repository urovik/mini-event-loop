#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>



int main(void){


    int epfd = epoll_create1(0);
    if (epfd < 0){
        perror("epoll");
        return 1;
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd < 0){
        perror("failed create listen fd");
        return 1;
    }
    int one = 1;
    setsockopt(listen_fd,SOL_SOCKET, SO_REUSEADDR, &one,sizeof(one));


    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(6376);

    if(bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0){
        perror("error bind listend fd");
        close(listen_fd);
        return 1;
    }

    if(listen(listen_fd, 128) < 0){
        perror("listen for listen_fd");
        close(listen_fd);
        return 1;
    }

    struct  epoll_event e = {0};
    e.events = EPOLLIN;
    e.data.fd = listen_fd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &e) < 0){
        perror("epoll_ctl");
        return 1;
    }
    

    printf("listen on 127.0.0.1:6376\n");

    struct epoll_event events[64];
    while (1) {
        int n = epoll_wait(epfd, events, 64, -1);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait"); break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                // пришёл новый клиент
                int cfd = accept(listen_fd, NULL, NULL);
                if (cfd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                    perror("accept"); continue;
                }
                printf("client fd=%d connected\n", cfd);

                // регистрируем клиента
                struct epoll_event cev = {0};
                cev.events = EPOLLIN;
                cev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &cev);
            }
            else{
                char buf[1024];
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                if(n == -1){
                    perror("read");
                } 
                if(n == 0){
                    printf("client closed\n");
                    close(fd);
                }
                if(n > 0){
                    buf[n] = '\0';
                    //printf("received: %s", buf);
                    write(fd, buf, n);
                }
            }
    
        }
    }
        printf("close");
        close(listen_fd);
        close(epfd);
        return 0;
    
}