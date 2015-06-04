#include "network_util.h"


#define MAX_INPUT_LEN 2000
#define BUFF_SIZE 2000



char* sock_recv(int sock_fd)
{
  char* reply=(char*)malloc(MAX_INPUT_LEN*sizeof(char));
  memset(reply, 0, MAX_INPUT_LEN);
  ssize_t ret = read(sock_fd, reply , MAX_INPUT_LEN);
  
  if(ret==-1)
  {
    ssds_log("Unable to read data from socket.", logERROR);
    free(reply);
    return NULL;
  }
  
  if(ret==MAX_INPUT_LEN)
  {
    char* buffer=(char*)malloc(BUFF_SIZE*sizeof(char));
    memset(buffer, 0, BUFF_SIZE);
    
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
    
    free(reply);
    return buffer;
  }
  //no buffer is needed for very short messages
  return reply;
}
