#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
  int socket_desc;
  char reply[100];
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  struct sockaddr_in server;
  server.sin_addr.s_addr=inet_addr("127.0.0.1");
  server.sin_family=AF_INET;
  server.sin_port=htons(1234);
  
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
  
  if( recv(socket_desc, reply , 100 , 0) < 0)
  {
    printf("Recieving of data has failed\n");
    return 1;
  }
  
  printf("%s", reply);
  
  return 0;
}