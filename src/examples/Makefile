#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - json_improved example

CC=gcc
CFLAGS= -std=c11 -pedantic -Wextra -Wall -O2

all: json_improved
json_improved:
	$(CC) $(CFLAGS) json_improved.c json_improved.h -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -ljson-glib-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -o json_improved
clean:
	rm -rf json_improved *.o
