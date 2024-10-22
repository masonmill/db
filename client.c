#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    // Make a connection
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        perror("connect");
        close(fd);
        exit(1);
    }

    char msg[] = "hello";
    ssize_t written = write(fd, msg, strlen(msg));
    if (written < 0) {
        perror("write");
        close(fd);
        exit(1);
    }
    if (written < strlen(msg)) {
        fprintf(stderr, "Partial write occurred\n");
        close(fd);
        exit(1);
    }

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        perror("read");
        close(fd);
        exit(1);
    }
    printf("server says: %s\n", rbuf);

    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }

    return 0;
}