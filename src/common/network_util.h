#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

#include "log_handler.h"
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"{
#endif
  
/** Function to recieve data of previously unknown length from socket. 
 *  There is a buffer that resizes according to the size of recieved data.
 *  @param sock_fd  File descriptor for socket that is recieving data
 * 
 *  @return         buffer with recieved data - needs to be freed in the end
 */
char* sock_recv(int sock_fd);


#ifdef __cplusplus
}
#endif

#endif