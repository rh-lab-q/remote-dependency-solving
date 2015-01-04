#soubor: Makefile
# Michal Ruprich, xrupri00, RedHat, SSDS - hawkey example

PP=g++
CPPFLAGS= -std=c++11
CC=gcc
CFLAGS= -std=c11
INCLUDE=-I/usr/include/solv -I/usr/include/hawkey
all: hawkey_ex

hawkey_ex: hawkey_ex_c.o
	$(PP) $(CPPFLAGS) $(INCLUDE) -l hawkey hawkey_ex.cpp hawkey_ex_c.o -o hawkey_ex
hawkey_ex_c.o: hawkey_ex_c.o
	$(CC) $(CFLAGS) $(INCLUDE) -l hawkey -c hawkey_ex.c -o hawkey_ex_c.o
clean:
	rm -rf *o hawkey_ex