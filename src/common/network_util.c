#include "network_util.h"


#define MAX_INPUT_LEN 200
#define BUFF_SIZE 1000



char* sock_recv(int sock_fd)
{
  char* buffer=(char*)malloc(BUFF_SIZE*sizeof(char));
  char reply[MAX_INPUT_LEN];
  
  memset(buffer, 0, BUFF_SIZE);
  memset(reply, 0, MAX_INPUT_LEN);
  
  ssize_t ret = read(sock_fd, reply , MAX_INPUT_LEN);
  
  if(ret==-1)
  {
    ssds_log("Unable to read data from socket.", logERROR);
    free(buffer);
    return NULL;
  }
  
  if(ret==MAX_INPUT_LEN)
  {
    int read_count = 0;
    int buff_size = 1;
    memcpy(buffer, reply, MAX_INPUT_LEN);
    
    do
    {
      read_count++;
      memset(reply, 0, MAX_INPUT_LEN);      
      
      if(read_count*MAX_INPUT_LEN >= buff_size*BUFF_SIZE)
        buffer=realloc(buffer, ++buff_size*BUFF_SIZE);
      
      ret = read(sock_fd, reply , MAX_INPUT_LEN);
      memcpy(buffer+read_count*MAX_INPUT_LEN, reply, MAX_INPUT_LEN);
      
      
    }while(!(ret<MAX_INPUT_LEN));
  }
  
  return buffer;
}
