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

 ssds_gc * global_gc = NULL;

 void ssds_gc_cleanup()
{
	ssds_gc_item * next = NULL;
	ssds_gc_item * item = global_gc->top;
	while(item != NULL)
	{
		next = item->next;
		if(item->ptr != NULL)
		{
			free(item->ptr);
		}
		free(item);
		item = next;
	}
	free(global_gc);
}

int ssds_gc_search(void * ptr)
{
	for (ssds_gc_item * tmp = global_gc->top; tmp != NULL; tmp = tmp->next)
	{
		if(tmp->ptr == ptr)
		{
			return 1;
		}
	}
	return 0;
}

void ssds_gc_remove(void * ptr)
{
	for (ssds_gc_item * item = global_gc->top; item != NULL; item = item->next)
	{
		if(item->ptr == ptr)
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
			return;
		}
	}
}

void ssds_gc_push(void * ptr)
{
	if(ssds_gc_search(ptr) == 1)
	{
		return;
	}
	ssds_gc_item * new_item = (ssds_gc_item *) malloc(sizeof(ssds_gc_item));
	if(new_item == NULL)
	{
		ssds_log(logERROR, "Failed to allocate new item in ssds_gc_push.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	new_item->ptr = ptr;
	new_item->next = NULL;
	new_item->prev = NULL;
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

void * ssds_realloc(void * old_ptr, int size)
{
	void * new_ptr = realloc(old_ptr, size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to reallocate pointer in ssds_realloc.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	if(new_ptr != old_ptr)
	{
		ssds_gc_remove(old_ptr);
		ssds_gc_push(new_ptr);
	}
	return new_ptr;
}

void * ssds_malloc(int size)
{
	void * new_ptr = malloc(size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to allocate pointer in ssds_malloc.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	ssds_gc_push(new_ptr);
	return new_ptr;
}

void * ssds_calloc(int n_items, int size)
{
	void * new_ptr = calloc(n_items, size);
	if(new_ptr == NULL)
	{
		ssds_log(logERROR, "Failed to allocate pointer in ssds_malloc.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	ssds_gc_push(new_ptr);
	return new_ptr;
}


void ssds_gc_init()
{
	global_gc = (ssds_gc *) malloc(sizeof(ssds_gc));
	if(global_gc == NULL)
	{
		ssds_log(logERROR, "Failed to allocate garbace collector.\n");
		ssds_gc_cleanup();
		exit(1);
	}
	global_gc->top = NULL;
}