#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>

int main()
{
  int socket_desc, new_sock;
  char* client_ip;
  char client_msg[1000];
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  if(socket_desc==-1)
  {
    printf("Server encountered an error when creating socket for communication\n");
    return 1;
  }
  
  struct sockaddr_in server, client;
  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(2345);
  
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
  
  const char* message = "Look, just because I don't be givin' no man a foot massage don't make it right for Marsellus to throw Antwone into a glass motherfuckin' house, fuckin' up the way the nigger talks. Motherfucker do that shit to me, he better paralyze my ass, 'cause I'll kill the motherfucker, know what I'm sayin'?\n";
  printf("msg len: %d\n", strlen(message));
  int addr_len = sizeof(server);
  while(1)
  {
    if((new_sock=accept(socket_desc, (struct sockaddr *) &client, (socklen_t*)&addr_len))<0)
    {
      printf("Accept connection has failed\n");
      return 1;
    }
    
    if( recv(new_sock, client_msg , 1000 , 0) < 0)
    {
      printf("Recieving of data has failed\n");
      return 1;
    }
    
    client_ip=inet_ntoa(client.sin_addr);
    printf("Connection accepted from ip address %s\n", client_ip);
    printf("%s\n", client_msg);
    
    write(new_sock, message, strlen(message));
  }
  
  return 0;
}