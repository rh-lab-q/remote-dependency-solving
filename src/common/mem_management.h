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
#include "log_handler.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef struct mem_item
{
	void *ptr;
	struct mem_item * next;
	struct mem_item * prev;
}ssds_gc_item;

typedef struct s_gc
{
	ssds_gc_item * top;
}ssds_gc;

/**
 * Deletes everything in the garbage collector including garbage collector.
 */
void ssds_gc_cleanup();

/**
 * Searches for a pointer in the garbage collector
 * @param ptr - pointer that is searched
 * @return 1 if pointer is in garbage collector, 0 if its not
 */
int ssds_gc_search(void * ptr);

/**
 * Removes pointer from garbage collector
 * @param ptr - pointer that is removed
 */
void ssds_gc_remove(void * ptr);

/**
 * Pushes pointer into garbage collector
 * @param ptr - pushed pointer
 */
void ssds_gc_push(void * ptr);

/**
 * Realloc function that has the same interface as realloc but works
 * with garbage collector
 * @param old_ptr - old pointer
 * @param size - new size of pointer
 * @return new reallocated pointer
 */
void * ssds_realloc(void * old_ptr, int size);

/**
 * Malloc function that has the same interface as realloc but works
 * with garbage collector
 * @param size - number of bytes to be allocated
 * @return allocated pointer
 */
void * ssds_malloc(int size);

/**
 * Malloc function that has the same interface as realloc but works
 * with garbage collector
 * @param n_items - number of chunks
 * @param size - size of chunks
 * @return allocated pointer
 */
void * ssds_calloc(int n_items, int size);

/**
 * Initialization function for garbage collector. Must be called before
 * user allocates something.
 */
void ssds_gc_init();


#ifdef __cplusplus
}
#endif

#endif //_MEM_MANAGEMENT_H
