#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

#define MAX_INPUT_LEN 10
#define BUFF_SIZE 50

int main()
{
  int socket_desc;
  char* buffer=(char*)malloc(BUFF_SIZE*sizeof(char));
  char reply[MAX_INPUT_LEN];
  bzero(reply, MAX_INPUT_LEN);
  bzero(buffer, BUFF_SIZE);
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
  
  ssize_t ret = read(socket_desc, reply , MAX_INPUT_LEN);
  
  if(ret == -1)
  {
    printf("Recieving of data has failed\n");
    return 1;
  }
  
  if(ret==MAX_INPUT_LEN)
  {
    int read_count = 0;
    int buff_size = 1;
    memcpy(buffer, reply, MAX_INPUT_LEN);
    
    do
    {
      read_count++;
      bzero(reply, MAX_INPUT_LEN);
      
      if(read_count*MAX_INPUT_LEN >= buff_size*BUFF_SIZE)
        buffer=realloc(buffer, ++buff_size*BUFF_SIZE);
      
      ret = read(socket_desc, reply , MAX_INPUT_LEN);
      memcpy(buffer+read_count*MAX_INPUT_LEN, reply, MAX_INPUT_LEN);
      
      
    }while(!(ret<MAX_INPUT_LEN));
  }
  
  printf("%s", buffer);
  
  return 0;
}