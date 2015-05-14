#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - hawkey example

CC=gcc
CFLAGS= -std=c11 -pedantic -Wextra -Wall -O2

all: server client
server:
	$(CC) $(CFLAGS) server.c -o server
client:
	$(CC) $(CFLAGS) client.c -o client
clean:
	rm -rf server client
