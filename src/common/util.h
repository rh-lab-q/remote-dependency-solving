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

#ifndef _UTIL_H
#define _UTIL_H

#include <librepo/librepo.h>

#ifdef __cplusplus
extern "C"{
#endif

#define DEBUG

/**
 * Resolves path for @System.solv file depending on system version
 * @brief resolve_dependency_file_path
 * @param ret_val return value of function
 * @param arch    return value of function
 * @param release return value of function
 */
int resolve_dependency_file_path(char* ret_val, char** arch, char** release);

/**
 * Callback function for downloading packages
 * @param data		void*
 * @param total 	double
 * @param downloaded	double
 * @return		0
 */
int progress_callback(void *data, double total, double downloaded);

/**
 * Callback function called after package download
 * @param data		void*
 * @param status	LrTransferStatus
 * @param msg		const char*
 * @return 		status
 */
int end_callback(void *data, LrTransferStatus status, const char *msg);

/**
 * Malloc with allocation control. If the malloc fails, the program calls abort.
 * @param len   Number of bytes to be allocated
 * @return      New pointer to the allocated memory
 */
void * rds_malloc();

/**
 * Realloc with allocation control. If the malloc fails, the program calls abort.
 * @param ptr   Pointer to memory segment for reallocation.
 * @param len   Number of bytes to be allocated.
 * @return      New pointer to the allocated memory.
 */
void * rds_realloc(void* ptr, size_t len);

/**
 * Prints error and aborts
 */
void rds_out_of_memory();

/**
 * Function called when termination signals are recieved.
 * @param signum    int
 */
void rds_signal_handler(int signum);


#ifdef __cplusplus
}
#endif

#endif
