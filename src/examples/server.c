#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>

int main()
{
  int socket_desc, new_sock;
  char* client_ip;
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  if(socket_desc==-1)
  {
    printf("Server encountered an error when creating socket for communication\n");
    return 1;
  }
  
  struct sockaddr_in server, client;
  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(1234);
  
  if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) <0)
  {
    printf("Server wasn't able to bind with socket\n");
    return 1;
  }
  
  printf("Server ready. Waiting for incoming connections.\n");
  
  if(listen(socket_desc, 5)!=0)
  {
    printf("Listen failed on server\n");
    return 1;
  }
  
  const char* message = "Hello from server\n";
  int addr_len = sizeof(server);
  while(1)
  {
    if((new_sock=accept(socket_desc, (struct sockaddr *) &client, (socklen_t*)&addr_len))<0)
    {
      printf("Accept connection has failed\n");
      return 1;
    }
    
    client_ip=inet_ntoa(client.sin_addr);
    printf("Connection accepted from ip address %s\n", client_ip);
    
    write(new_sock, message, strlen(message));
  }
  
  return 0;
}