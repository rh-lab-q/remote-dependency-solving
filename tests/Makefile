# !!! all files have to be in one local testing folder for this temporary makefile
# make server, make client, make clean ("make" only builds server)

TARGETS = server #name of the program && target file 
OBJS = Server.o
CC = g++
CXXFLAGS = -O2 -g -Wall -Wextra -pedantic  -std=c++11  -I/usr/local/include -L/usr/local/lib -lboost_thread -lboost_system -fmessage-length=0

#basic build function with dependency OBJ
server: $(OBJS)
	$(CC)  $(CXXFLAGS) $(OBJS) -o $(TARGETS)

Server.o: Server.cpp
	$(CC) $(CXXFLAGS) -c Server.cpp

TAREGETC = client
OBJC =  client.o

client: $(OBJC)
	$(CC)  $(CXXFLAGS) $(OBJC) -o client

client.o: client.cpp
	$(CC) $(CXXFLAGS) -c client.cpp

clean:
	rm -f $(OBJS) $(TARGETS) $(OBJC) $(TARGETC)
