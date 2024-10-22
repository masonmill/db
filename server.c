#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Read and write
static void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        perror("read");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    ssize_t written = write(connfd, wbuf, strlen(wbuf));
    if (written < 0) {
        perror("write");
        return;
    }
}

int main(void)
{
    // Obtain a socket handle
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }

    // Configure the socket
    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("setsockopt");
        close(fd);
        exit(1);
    }

    // Bind to an address
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // INADDR_ANY
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        perror("bind");
        close(fd);
        exit(1);
    }

    // Listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        perror("listen");
        close(fd);
        exit(1);
    }

    // Accept connections
    while (1) {
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0) {
            perror("accept");
            continue; // Continue accepting other connections
        }

        do_something(connfd);

        if (close(connfd) < 0) {
            perror("close connection");
        }
    }

    return 0;
}