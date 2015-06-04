#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include "../common/network_util.h"

#define MAX_INPUT_LEN 10
#define BUFF_SIZE 50

int main()
{
  int socket_desc;
  const char * message = "Hello from client\n";
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  struct sockaddr_in server;
  server.sin_addr.s_addr=inet_addr("127.0.0.1");
  server.sin_family=AF_INET;
  server.sin_port=htons(2345);
  
  if(socket_desc==-1)
  {
    printf("Client encountered an error when creating socket for communication\n");
    return 1;
  }
  
  if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)
  {  
    printf("Connection error\n");
    return 1;
  }
  
  printf("Connection successful\n");
  
  write(socket_desc, message, strlen(message));
  
  char* buf=sock_recv(socket_desc);
  
  printf("%s", buf);
  
  free(buf);
  return 0;
}