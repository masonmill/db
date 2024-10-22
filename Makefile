CC = gcc
CFLAGS = -Wall -Wextra -O2 -g

all: server client

server: server.c
	$(CC) $(CFLAGS) $< -o $@

client: client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f server client

.PHONY: all clean