GCC=gcc
CFLAGS=-std=c11

all: lr_bug

lr_bug:
	$(GCC) $(CFLAGS) lr_handle_free_issue.c -o lr_bug -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -L/usr/include/librepo/ -lrepo -lgobject-2.0 -lglib-2.0

clean:
	rm lr_bug