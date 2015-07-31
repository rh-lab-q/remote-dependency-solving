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

#include "mem_management.h"

Ssds_gc * global_gc = NULL;

void ssds_signal_handler(int signum)
{
	ssds_gc_cleanup();
	exit(signum);
}

void ssds_gc_cleanup()
{
	Ssds_gc_item * next = NULL;
	Ssds_gc_item * item = global_gc->top;
	while(item != NULL)
	{
		next = item->next;
		switch(item->type)
		{
			case SOCKET:
				close(item->data.sock_fd);
				break;
			case PTR:
				if(item->data.ptr != NULL)
				{
					free(item->data.ptr);
				}
				break;
			default:
				break;
		}

		free(item);
		item = next;
	}
	free(global_gc);
}

Ssds_gc_item * ssds_gc_search(Alloc_data data, int type)
{
	for (Ssds_gc_item * tmp = global_gc->top; tmp != NULL; tmp = tmp->next)
	{
		switch(type)
		{
			case SOCKET:
				if(tmp->data.sock_fd == data.sock_fd)
				{
					return tmp;
				}
				break;
			case PTR:
				if(tmp->data.ptr == data.ptr)
				{
					return tmp;
				}
				break;
			default:
				break;
		}
	}
	return NULL;
}

void ssds_gc_remove(Alloc_data data, int type)
{
	Ssds_gc_item * item = ssds_gc_search(data, type); 
	if(item != NULL)
	{
		if(item->next == NULL && item->prev == NULL) //only item in gc
		{
			global_gc->top = NULL;
		}
		else if(item->next == NULL && item->prev != NULL) //item on the end of gc
		{
			item->prev->next = NULL;
		}
		else if(item->next != NULL && item->prev == NULL) //item on the start of gc
		{
			item->next->prev = NULL;
			global_gc->top = item->next;
		}
		else if(item->next != NULL && item->prev != NULL) //item in between two other items
		{
			item->prev->next = item->next;
			item->next->prev = item->prev;
		}
		free(item);
	}
}

void ssds_gc_push(Alloc_data data, int type)
{
	if(ssds_gc_search(data, type) != NULL)
	{
		return;
	}
	Ssds_gc_item * new_item = (Ssds_gc_item *) malloc(sizeof(Ssds_gc_item));
	if(new_item == NULL)
	{
		fprintf(stderr, "Failed to allocate new item in ssds_gc_push.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	new_item->type = type;
	new_item->next = NULL;
	new_item->prev = NULL;
	switch(type)
	{
		case SOCKET:
			new_item->data.sock_fd = data.sock_fd;
			break;
		case PTR:
			new_item->data.ptr = data.ptr;
		default:
			break;

	}
	if(global_gc->top == NULL)
	{
		global_gc->top = new_item;
	}
	else
	{
		global_gc->top->prev = new_item;
		new_item->next = global_gc->top;
		global_gc->top = new_item;
	}
}

void ssds_gc_push_ptr(void * ptr)
{
	Alloc_data data;
	data.ptr = ptr;
	ssds_gc_push(data, PTR);
}

void ssds_gc_push_socket(int socket)
{
	Alloc_data data;
	data.sock_fd = socket;
	ssds_gc_push(data,SOCKET);
}

void ssds_free(void * ptr)
{
       ssds_gc_remove_ptr(ptr);
       free(ptr);
}

void ssds_close(int socket)
{
       ssds_gc_remove_socket(socket);
       close(socket);
}

void ssds_gc_remove_ptr(void * ptr)
{
	Alloc_data data;
	data.ptr = ptr;
	ssds_gc_remove(data, PTR);
}

void ssds_gc_remove_socket(int socket)
{
	Alloc_data data;
	data.sock_fd = socket;
	ssds_gc_remove(data,SOCKET);
}

void * ssds_realloc(void * old_ptr, int size)
{
	void * new_ptr = realloc(old_ptr, size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to reallocate pointer in ssds_realloc.\n");
		return NULL;
		//ssds_gc_cleanup();
		//exit(1);
	}
	if(new_ptr != old_ptr)
	{
		ssds_gc_remove_ptr(old_ptr);
		ssds_gc_push_ptr(new_ptr);
	}
	return new_ptr;
}

void * ssds_malloc(int size)
{
	void * new_ptr = malloc(size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to allocate pointer in ssds_malloc.\n");
		return NULL;
		//ssds_gc_cleanup();
		//exit(1);
	}
	ssds_gc_push_ptr(new_ptr);
	return new_ptr;
}

void * ssds_calloc(int n_items, int size)
{
	void * new_ptr = calloc(n_items, size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to allocate pointer in ssds_malloc.\n");
		return NULL;
		//ssds_gc_cleanup();
		//exit(1);
	}
	ssds_gc_push_ptr(new_ptr);
	return new_ptr;
}


void ssds_gc_init()
{
	global_gc = (Ssds_gc *) malloc(sizeof(Ssds_gc));
	if(global_gc == NULL)
	{
		ssds_log(logERROR, "Failed to allocate garbace collector.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	global_gc->top = NULL;
}

int ssds_socket(int domain, int type, int protocol)
{
	int new_socket = socket(domain, type, protocol);
	if(new_socket < 0)
	{
		ssds_log(logERROR, "Failed to open socket.\n");
		return -1;
		//ssds_gc_cleanup();
		//exit(1);
	}
	ssds_gc_push_socket(new_socket);
	return new_socket;
}

int ssds_accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
	int new_socket = accept(socket, address, address_len);
	if(new_socket < 0)
	{
		ssds_log(logERROR, "Failed to accept socket.\n");
		return -1;
		//ssds_gc_cleanup();
		//exit(1);
	}
	ssds_gc_push_socket(new_socket);
	return new_socket;
}
