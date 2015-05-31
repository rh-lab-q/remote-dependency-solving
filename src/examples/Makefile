CC=gcc
CFLAGS=-std=c11
INCLUDE=-I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/
LINKS=-lglib-2.0


all: handler example
	$(CC) $(CFLAGS) $(INCLUDE) $(LINKS) log_example.o log_handler.o -o log
example:
	$(CC) $(CFLAGS) $(INCLUDE) $(LINKS) -c log_example.c -o log_example.o
handler:
	$(CC) $(CFLAGS) $(INCLUDE) $(LINKS) -c log_handler.c -o log_handler.o
clean:
	rm -rf log *.o
