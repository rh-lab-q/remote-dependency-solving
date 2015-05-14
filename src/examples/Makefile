CC=gcc
CFLAGS=-std=c11
INCLUDE=-I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/
LINKS=-lglib-2.0


all:
	$(CC) $(CFLAGS) $(INCLUDE) $(LINKS) log_example.c -o log
clean:
	rm -rf log
