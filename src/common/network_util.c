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
  char* reply = (char*)rds_malloc(MAX_INPUT_LEN*sizeof(char));
  memset(reply, 0, MAX_INPUT_LEN);
  ssize_t ret = rds_read(sock_fd, reply , MAX_INPUT_LEN);
  
  if(ret == -1)
  {
    rds_log(logERROR, "Unable to read data from socket\n");
    rds_free(reply);
    return NULL;
  }
  
  if(ret == MAX_INPUT_LEN)
  {
    char* buffer = (char*)rds_malloc(BUFF_SIZE*sizeof(char));
    memset(buffer, 0, BUFF_SIZE);
    
    int read_count = 0;
    int buff_size = 1;
    memcpy(buffer, reply, MAX_INPUT_LEN);
    
    do
    {
      read_count++;
      memset(reply, 0, MAX_INPUT_LEN);      
      
      if(read_count*MAX_INPUT_LEN >= buff_size*BUFF_SIZE)
        buffer = rds_realloc(buffer, ++buff_size*BUFF_SIZE);
      
      ret = rds_read(sock_fd, reply , MAX_INPUT_LEN);
      memcpy(buffer+read_count*MAX_INPUT_LEN, reply, MAX_INPUT_LEN);
      
      
    }while(!(ret < MAX_INPUT_LEN));
    
    rds_free(reply);
    return buffer;
  }
  //no buffer is needed for very short messages
  return reply;
}

ssize_t sock_solv_recv(int sock_fd, char **buffer)
{
  *buffer = (char*)rds_malloc(MAX_INPUT_LEN*sizeof(char));
  memset(*buffer, 0, MAX_INPUT_LEN);
  ssize_t retVal = rds_read(sock_fd, *buffer , MAX_INPUT_LEN);

  if(retVal == -1)
  {
    rds_log(logERROR, "Unable to read data from socket\n");
    rds_free(*buffer);
    return retVal;
  }

  if(retVal == MAX_INPUT_LEN)
  {
    char* buff = (char*)rds_malloc(BUFF_SIZE*sizeof(char));
    memset(buff, 0, BUFF_SIZE);

    int read_count = 0;
    int buff_size = 1;
    ssize_t ret;
    memcpy(buff, *buffer, MAX_INPUT_LEN);

    do
    {
      read_count++;
      memset(*buffer, 0, MAX_INPUT_LEN);

      if(read_count*MAX_INPUT_LEN >= buff_size*BUFF_SIZE)
        buff = rds_realloc(buff, ++buff_size*BUFF_SIZE);

      ret = rds_read(sock_fd, *buffer , MAX_INPUT_LEN);
      retVal += ret;
      memcpy(buff+read_count*MAX_INPUT_LEN, *buffer, MAX_INPUT_LEN);

    }while(!(ret < MAX_INPUT_LEN));

    rds_free(*buffer);
    *buffer = buff;
  }
  //no buffer is needed for very short messages
  return retVal;
}
 
 
void secure_write(int socket, char* message, ssize_t length)
{

	rds_write(socket, message, length);
}

int client_connect(int *socket, char *server_address, long int port)
{

  int connection_try = 1;

  *socket = rds_socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  rds_log(logDEBUG, "Set up socket descriptor.\n");

  rds_log(logDEBUG, "Setting up connection to server.\n");
  struct sockaddr_in server_comm;

  server_comm.sin_addr.s_addr = inet_addr(server_address);
  rds_log(logDEBUG, "Set server address.\n");

  server_comm.sin_family = AF_INET;
  rds_log(logDEBUG, "Set comunication protocol.\n");

  server_comm.sin_port = htons(port);
  rds_log(logDEBUG, "Set server port.\n");

  rds_log(logDEBUG, "Socket control.\n");
  if(*socket == -1)
  {
    rds_log(logERROR, "Client encountered an error when creating socket for communication.\n");
    return SOCKET_ERROR;
  }

  rds_log(logDEBUG, "Socket control - OK.\n");

  rds_log(logMESSAGE, "Trying to connect to server...(1 of 3)\n");
  while((connect(*socket, (struct sockaddr *)&server_comm, sizeof(server_comm)) < 0) && (connection_try < 3))
  {
     rds_log(logMESSAGE, "Unable to connect to comm. socket on server. Another attempt will be executed in 5 seconds.\n");
     sleep(5);
     rds_log(logMESSAGE, "Trying to connect to server...(%d of 3)\n", ++connection_try);
  }

  if(connection_try == 3){
    rds_log(logERROR, "Unable to connect comm. socket on server. Please, check out your network connection and try it again later.\n");
    return NETWORKING_ERROR;
  }

  rds_log(logMESSAGE, "Connection to server is established.\n");

  return OK;
}
