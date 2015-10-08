GCC= gcc
FLAGS= -std=c11
INCLUDE=-I/usr/include/solv -I/usr/include/hawkey
LINK=-l hawkey -l solv

all: custom_solv

custom_solv:
	$(GCC) $(FLAGS) $(INCLUDE) $(LINK) custom_solv.c -o custom_solv

clean:
	rm custom_solv
