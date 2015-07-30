/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "network_util.h"

char* sock_recv(int sock_fd)
{
  char* reply=(char*)ssds_malloc(MAX_INPUT_LEN*sizeof(char));
  memset(reply, 0, MAX_INPUT_LEN);
  ssize_t ret = read(sock_fd, reply , MAX_INPUT_LEN);
  
  if(ret==-1)
  {
    ssds_log(logERROR, "Unable to read data from socket\n");
    ssds_free(reply);
    return NULL;
  }
  
  if(ret==MAX_INPUT_LEN)
  {
    char* buffer=(char*)ssds_malloc(BUFF_SIZE*sizeof(char));
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
    
    ssds_free(reply);
    return buffer;
  }
  //no buffer is needed for very short messages
  return reply;
}
