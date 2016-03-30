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

#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>

#include "util.h"
#include "log_handler.h"
#include "errors.h"

Rds_gc * global_gc = NULL;

int allocated = 0;
int num_items = 0;

void rds_signal_handler(int signum) {
    rds_log(logERROR, "End with signal number %d.\n", signum);
    rds_gc_cleanup();
    exit(signum);
}

void rds_gc_cleanup() {
    Rds_gc_item * next = NULL;
    Rds_gc_item * item = global_gc->top;
    
    while(item != NULL) {
        next = item->next;
        switch(item->type) {
            case SOCKET:
                close(item->data.sock_fd);
            break;
            
            case PTR:
                if(item->data.ptr != NULL)
                    free(item->data.ptr);
            break;
            
            default:
            break;
        }

        free(item);
        item = next;
    }
    
    free(global_gc);
    global_gc = NULL;
    rds_log(logDEBUG,"Number of items: %d Memory allocated: %d \n",num_items, allocated);
    allocated = 0;
    num_items = 0;
}

Rds_gc_item * rds_gc_search(Alloc_data * data, int type) {
    for (Rds_gc_item * tmp = global_gc->top; tmp != NULL; tmp = tmp->next) {
        switch(type) {
            case SOCKET:
                if(tmp->data.sock_fd == data->sock_fd)
                    return tmp;
            break;
            
            case PTR:
                if(tmp->data.ptr == data->ptr)
                    return tmp;
            break;
            
            default:
            break;
        }
    }
    
    return NULL;
}

void rds_gc_remove(Alloc_data * data, int type) {
    Rds_gc_item * item;
    
    item = rds_gc_search(data, type); 
    if(item != NULL) {
        if(item->next == NULL && item->prev == NULL) //only item in gc
            global_gc->top = NULL;
        else if(item->next == NULL && item->prev != NULL) //item on the end of gc
            item->prev->next = NULL;
        else if(item->next != NULL && item->prev == NULL) {//item on the start of gc
            item->next->prev = NULL;
            global_gc->top = item->next;
        }
        else if(item->next != NULL && item->prev != NULL) {//item in between two other items
            item->prev->next = item->next;
            item->next->prev = item->prev;
        }
        free(item);
    }
}

void rds_gc_push(Alloc_data * data, int type) {
    //if(rds_gc_search(data, type) != NULL)
    //{
    //	return;
    //}
    Rds_gc_item * new_item;

    new_item = (Rds_gc_item *) malloc(sizeof(Rds_gc_item));
    if(new_item == NULL) {
        fprintf(stderr, "Failed to allocate new item in rds_gc_push.\n");
        rds_gc_cleanup();
        exit(MEMORY_ERROR);
    }
    
    new_item->type = type;
    new_item->next = NULL;
    new_item->prev = NULL;
    switch(type) {
        case SOCKET:
            new_item->data.sock_fd = data->sock_fd;
        break;
        
        case PTR:
            new_item->data.ptr = data->ptr;
        default:
        break;

    }
    if(global_gc->top == NULL)
        global_gc->top = new_item;
    else {
        global_gc->top->prev = new_item;
        new_item->next = global_gc->top;
        global_gc->top = new_item;
    }
}

void rds_gc_push_ptr(void * ptr) {
    Alloc_data data = { .ptr = ptr};
    //data->ptr = ptr;
    rds_gc_push(&data, PTR);
}

void rds_gc_push_socket(int socket) {
    Alloc_data data = {.sock_fd = socket};
    //data->sock_fd = socket;
    rds_gc_push(&data,SOCKET);
}

void rds_free(void * ptr) {
    num_items -= 1;
    rds_gc_remove_ptr(ptr);
    free(ptr);
}

void rds_close(int socket) {
    num_items -= 1;
    rds_gc_remove_socket(socket);
    close(socket);
}

void rds_gc_remove_ptr(void * ptr) {
    Alloc_data data = {.ptr = ptr};
    //data->ptr = ptr;
    rds_gc_remove(&data, PTR);
}

void rds_gc_remove_socket(int socket) {
    Alloc_data data = {.sock_fd = socket};
    //data->sock_fd = socket;
    rds_gc_remove(&data,SOCKET);
}

void * rds_realloc(void * old_ptr, int size) {
    allocated += size;
    void * new_ptr;
    
    new_ptr = realloc(old_ptr, size);
    if(new_ptr == NULL) {
        rds_log(logERROR, "Failed to reallocate pointer in rds_realloc.\n");
        return NULL;
    }
    if(new_ptr != old_ptr) {
        rds_gc_remove_ptr(old_ptr);
        rds_gc_push_ptr(new_ptr);
    }
    return new_ptr;
}

void * rds_malloc(int size) {
    num_items += 1;
    allocated += size;
    void * new_ptr;
    
    new_ptr = malloc(size);
    if(new_ptr == NULL) {
        rds_log(logERROR, "Failed to allocate pointer in rds_malloc.\n");
        return NULL;
    }
    rds_gc_push_ptr(new_ptr);
    return new_ptr;
}

void * rds_calloc(int n_items, int size) {
    num_items += 1;
    allocated += n_items * size;
    void * new_ptr;
    
    new_ptr = calloc(n_items, size);
    if(new_ptr == NULL) {
        rds_log(logERROR, "Failed to allocate pointer in rds_malloc.\n");
        return NULL;
    }
    rds_gc_push_ptr(new_ptr);
    return new_ptr;
}


void rds_gc_init() {
    global_gc;

    global_gc = (Rds_gc *) malloc(sizeof(Rds_gc));
    if(global_gc == NULL) {
        rds_log(logERROR, "Failed to allocate garbace collector.\n");
        exit(MEMORY_ERROR);
    }
    global_gc->top = NULL;
}

int rds_socket(int domain, int type, int protocol) {
    num_items += 1;
    int new_socket;
    
    new_socket = socket(domain, type, protocol);
    if(new_socket < 0) {
        rds_log(logERROR, "Failed to open socket.\n");
        return -1;
    }
    rds_gc_push_socket(new_socket);
    return new_socket;
}

int rds_accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len) {
    num_items += 1;
    int new_socket;
    
    new_socket = accept(socket, address, address_len);
    if(new_socket < 0) {
        rds_log(logERROR, "Failed to accept socket.\n");
        return -1;
    }
    rds_gc_push_socket(new_socket);
    return new_socket;
}

Rds_gc * rds_gc_get_header() {
    return global_gc;
}
