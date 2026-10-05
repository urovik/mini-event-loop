#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>



int main(void){


    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0){
        perror("failed create listen fd");
        return 1;
    }

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

    printf("listen on 127.0.0.1:6376\n");

    while (1) {
        struct sockaddr_in addr_client;
        socklen_t len_client = sizeof(addr_client);
        int client_fd = accept(listen_fd, (struct sockaddr*) &addr_client, &len_client);
        if (client_fd < 0){
            perror("failed create client_fd");
            close(listen_fd);
            return 1;
        }
        printf("client connected\n");
    
        char buf[1024];
        ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
        if(n == -1){
            perror("read");
        } 
        if(n == 0){
            printf("client closed\n");
            close(client_fd);
        }
        if(n > 0){
            buf[n] = '\0';
            //printf("received: %s", buf);
            write(client_fd, buf, n);
        }
    }
        printf("close");
        close(listen_fd);
        return 0;
}