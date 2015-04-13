#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - hawkey example

PP=g++
CPPFLAGS= -std=c++11 -pedantic
INCLUDE=-I/usr/include/json-glib-1.0/json-glib/ -I/usr/include/json-glib-1.0/ -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/
LINK=-ljson-glib-1.0 -lglib-2.0 -lgobject-2.0

all: json_read json_create
	$(PP) $(CPPFLAGS) $(INCLUDE) $(LINK) json_ex.cpp json_read.o json_create.o -o json
json_read:
	$(PP) $(CPPFLAGS) $(INCLUDE) $(LINK) -c json_read.cpp -o json_read.o
json_create:
	$(PP) $(CPPFLAGS) $(INCLUDE) $(LINK) -c json_create.cpp -o json_create.o
clean:
	rm -r json *.o
