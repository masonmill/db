#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include "util.h"

const size_t k_max_msg = 4096;

static int32_t one_request(int connfd)
{
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read()");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // Assume little endian
    if (len > k_max_msg) {
        msg("message too long");
        return -1;
    }

    // Request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read()");
        return err;
    }

    // Do something
    rbuf[4 + len] = '\0';
    printf("client says: %s\n", &rbuf[4]);

    // Reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(connfd, wbuf, 4 + len);
}

int main(void)
{
    // Obtain a socket handle
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    // Configure the socket
    int val = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        die("setsockopt()");
    }

    // Bind to an address
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // INADDR_ANY
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        close(fd);
        die("bind()");
    }

    // Listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        close(fd);
        die("listen()");
    }

    // Accept connections
    while (1) {
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0) {
            msg("accept()");
            continue; // Continue accepting other connections
        }

        // Only serves one client connection at a time
        while (1) {
            int32_t err = one_request(connfd);
            if (err) {
                break;
            }
        }
        close(connfd);
    }

    return 0;
}