#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - hawkey example

CC=gcc
CFLAGS= -std=c11 -pedantic -Wextra -Wall -O2

all: server client
server:
	$(CC) $(CFLAGS) server.c -o server
client: util.o log.o
	$(CC) $(CFLAGS) network_util.o client.c log_handler.o -o client
util.o: 
	$(CC) $(CFLAGS) -c ../common/network_util.c
log.o:
	$(CC) $(CFLAGS) -c ../common/log_handler.c
clean:
	rm -rf server client *.o
