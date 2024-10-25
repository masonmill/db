CC = g++
CFLAGS = -Wall -Wextra -O2 -g -std=c++17

all: server client

server: server.cpp
	$(CC) $(CFLAGS) $< -o $@

client: client.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f server client

.PHONY: all clean