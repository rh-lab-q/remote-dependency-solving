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

//#ifndef _CFG_PARSING_H
//#define _CFG_PARSING_H

#include "includes.h"
#include "log_handler.h"
#include "mem_management.h"


#ifdef __CASE_CLIENT
#define CFG_FILE "../etc/rds-client.conf"
#else
#define CFG_FILE "../etc/rds-server.conf"
#endif

int read_cfg(char **ret_client_id, char **address, long int *comm_port);
int write_to_cfg(char *name, char *value);
int read_srv_cfg(long int* comm_port);
int write_to_cfg(char *name, char *value);
char* file_read_value(FILE* file, int max_length);
void compare(FILE* file, char* str, int len, char *state, char *act_c);
