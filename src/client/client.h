/**
 * Server side dependency solving 
 * transfer of dependency solving from local machine to server when installing new packages
 * 
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký, Walter Scherfel, Šimon Matěj
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

#ifndef _CLIENT_H
#define _CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../common/includes.h"
#include <librepo/librepo.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmps.h>

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/params.h"
#include "../common/json_handler.h"
#include "../common/repo_handler.h"
#include "../common/util.h"
#include "../common/mem_management.h"
#include "../../cmake/Config/version_conf.h"
#include "../common/errors.h"
#include "../common/detect_missing_repos.h"
#include "../common/packages.h"

/**
 * Asking for new client ID from server.
 * @param socket    int socket handler
 * @param id        char** pointer to ID string
 * @param arch      char* type of computer architecture
 * @param release   char* version of installed system
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int ssds_get_new_id(int socket, char **id, char *arch, char *release);

/**
 * Send @System.solv to server.
 * @param comm_sock int communication socket handler
 * @param data_sock int data socket handler
 * @param path      char* path to @System.solv file
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int ssds_send_System_solv(int comm_sock, int data_sock, char *path);

/**
 * Send type of operation, names of packages and addresses of repos to server. 
 * @param params    ParamOptsCl* structure of command line parsed parameters
 * @param arch	    char* type of computer architecture
 * @param release   char* version of installed system
 * @param socket    int socket handler
 * @param action    int type of requested operation
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int ssds_send_repo(ParamOptsCl* params, char *arch, char *release, int socket, int action);

/**
 * Parse answer from server of ssds_send_repo request.
 * @param socket    int socket handler
 * @param message   char** message fom delivered answer.
 * @return          Returns ANSWER_OK if and NULL in message, otherwise error code with message.
 */
int ssds_check_repo(int socket, char **message);

/**
 * Parses dependency solv answer and download and install/update/erase requested packages.
 * @param socket    int socket handler
 * @param action    int type of requested action
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int ssds_answer_process(int socket, int action);

#ifdef __cplusplus
}
#endif

#endif
