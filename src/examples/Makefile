#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - hawkey example

PP=g++
CPPFLAGS= -std=c++11
CC=gcc
CFLAGS= -std=c11
INCLUDE=/usr/include/
all: hawkey_ex

hawkey_ex: hawkey_ex_c.o hawkey_ex_cpp.o
	$(PP) $(CPPFLAGS) -I$(INCLUDE) hawkey_ex_c.o hawkey_ex_cpp.o -o hawkey_ex
hawkey_ex_c.o: hawkey_ex_c.o
	$(CC) $(CFLAGS) -I$(INCLUDE) -c hawkey_ex.c -o hawkey_ex_c.o
hawkey_ex_cpp.o: hawkey_ex_cpp.o
	$(PP) $(CPPFLAGS) -I$(INCLUDE) -c hawkey_ex.cpp -o hawkey_ex_cpp.o
clean:
	rm -rf *o hawkey_ex