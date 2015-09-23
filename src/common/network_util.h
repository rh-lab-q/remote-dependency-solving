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

#ifndef _NETWORK_UTIL_H
#define _NETWORK_UTIL_H

#include "log_handler.h"
#include "mem_management.h"
#include "includes.h"
#include "cfg_parsing.h"
#include "errors.h"
#
#ifdef __cplusplus
extern "C"{
#endif

/** Max length of one read call */
#define MAX_INPUT_LEN 2000

/** 
 * Initial length of buffer - buffer is resized 
 * when additional data is recieved
 */
#define BUFF_SIZE 2000
  
/** 
 * Function to recieve data of previously unknown length from socket. 
 * There is a buffer that resizes according to the size of recieved data.
 * @param sock_fd  File descriptor for socket that is recieving data
 * @return         buffer with recieved data - needs to be freed in the end
 */
char* sock_recv(int sock_fd);

/** 
 * Function to recieve data of previously unknown length from socket. 
 * There is a buffer that resizes according to the size of recieved data.
 * @param sock_fd  File descriptor for socket that is recieving data
 * @param buffer   buffer with recieved data - needs to be freed in the end
 * @return         count of readed bytes
 */
ssize_t sock_solv_recv(int sock_fd, char **buffer);

/** 
 * Function for secure transfer data to client/server . 
 * @param socket   communication socket
 * @param *message transfered message
 * @param length   length of message
 */
void secure_write(int socket, char* message, ssize_t length);

/** 
 * Function to connect client to server. 
 * @param  *socket  		communication socket
 * @param  *server_address	IP of server
 * @param  comm_port  		communication port
 * @return int         OK or error code
 */
int client_connect(int *socket, char *server_address, long int comm_port);

#ifdef __cplusplus
}
#endif

#endif /* _NETWORK_UTIL_H */
