/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký, Walter Scherfel
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

#ifndef _MEM_MANAGEMENT_H
#define _MEM_MANAGEMENT_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "log_handler.h"


#ifdef __cplusplus
extern "C"{
#endif

enum ALLOC_TYPES
{
	SOCKET,
	PTR
};

typedef union al_dat
{
	int sock_fd;
	void * ptr;
}Alloc_data;

typedef struct mem_item
{
	Alloc_data data;
	int type;
	struct mem_item * next;
	struct mem_item * prev;
}Ssds_gc_item;

typedef struct s_gc
{
	Ssds_gc_item * top;
}Ssds_gc;

/**
 * Deletes everything in the garbage collector including garbage collector.
 */
void ssds_gc_cleanup();

/**
 * Searches for a pointer in the garbage collector
 * @param ptr       pointer that is searched
 * @return          1 if pointer is in garbage collector, 0 if its not
 */
Ssds_gc_item * ssds_gc_search(Alloc_data data, int type);

/**
 * Removes pointer from garbage collector
 * @param ptr       pointer that is removed
 */
void ssds_gc_remove(Alloc_data data, int type);

/**
 * Pushes pointer into garbage collector
 * @param ptr       pushed pointer
 */
void ssds_gc_push(Alloc_data data, int type);

/**
 * Realloc function that has the same interface as realloc but works
 * with garbage collector
 * @param old_ptr   old pointer
 * @param size      new size of pointer
 * @return          new reallocated pointer
 */
void * ssds_realloc(void * old_ptr, int size);

/**
 * Malloc function that has the same interface as realloc but works
 * with garbage collector
 * @param size      number of bytes to be allocated
 * @return          allocated pointer
 */
void * ssds_malloc(int size);

/**
 * Malloc function that has the same interface as realloc but works
 * with garbage collector
 * @param n_items   number of chunks
 * @param size      size of chunks
 * @return          allocated pointer
 */
void * ssds_calloc(int n_items, int size);

/**
 * Initialization function for garbage collector. Must be called before
 * user allocates something.
 */
void ssds_gc_init();

/**
 * Pushes pointer to garbage collector.
 * @param ptr       pushed pointer
 */
void ssds_gc_push_ptr(void * ptr);

/**
 * @Pushes socket to garbage collector.
 * @param socket    pushed socket
 */
void ssds_gc_push_socket(int socket);

/**
 * Removes pointer from garbage collector.
 * @param ptr       removed pointer
 */
void ssds_gc_remove_ptr(void * ptr);

/**
 * Removes socket from garbage collector.
 * @param socket    removed socket
 */
void ssds_gc_remove_socket(int socket);

/**
 * Creates a socket and pushes it into garbage collector.
 * @param domain    parameteor of socket
 * @param type      parameteor of socket
 * @param protocol  parameteor of socket
 * @return          new socket
 */
int ssds_socket(int domain, int type, int protocol);

/**
 * Accepts a socket and pushes it into garbage collector
 * @param socket        parameter of accept
 * @param address       parameter of accept
 * @param address_len   parameter of accept
 * @return              accepted socket
 */
int ssds_accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);


#ifdef __cplusplus
}
#endif

#endif //_MEM_MANAGEMENT_H
