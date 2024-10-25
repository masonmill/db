#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Maximum size for a single message
const size_t k_max_msg = 4096;

// Log a message to stderr
void msg(const char *msg) { fprintf(stderr, "%s\n", msg); }

// Print error message with errno and abort program
void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

// Read exactly n bytes from fd into buf, retrying on partial reads
int32_t read_full(int fd, char *buf, size_t n)
{
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

// Write exactly n bytes from buf to fd, retrying on partial writes
int32_t write_all(int fd, const char *buf, size_t n)
{
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

#endif /* UTIL_H */