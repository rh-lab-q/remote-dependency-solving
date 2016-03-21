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

enum{
	YES,
	NO,
	DOWNLOAD,
	YES_NO,
	YES_NO_DOWNLOAD,
};

/**
 * Asking for new client ID from server.
 * @param socket    int socket handler
 * @param id        char** pointer to ID string
 * @param arch      char* type of computer architecture
 * @param release   char* version of installed system
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int get_new_id(int socket, char **id, char *arch, char *release);

/**
 * Send file to server.
 * @param socket    int communication socket handler
 * @param type      int type of file 
 * @param path      char* path to file
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int send_file(int socket, int type, char *path);

/**
 * Compare two files
 * @param fileOne   char* path to first file 
 * @param fileTwo   char* path to second file 
 * @return          Returns OK if files are identical, else error code.
 */
int compare_files(char *fileOne, char *fileTwo);

/**
 * Compare two files
 * @param source        char* path to source file 
 * @param destination   char* path to destination file 
 * @return              Returns OK, else error code.
 */
int copy_file(char *source, char *destination);

/**
 * Send type of operation, names of packages and addresses of repos to server. 
 * @param params    ParamOptsCl* structure of command line parsed parameters
 * @param arch	    char* type of computer architecture
 * @param release   char* version of installed system
 * @param socket    int socket handler
 * @param action    int type of requested operation
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int send_repo(ParamOptsCl* params, char *arch, char *release, int socket, int action);

/**
 * Parse answer from server of send_repo request.
 * @param socket    int socket handler
 * @param message   char** message fom delivered answer.
 * @return          Returns ANSWER_OK if and NULL in message, otherwise error code with message.
 */
int check_repo(int socket, char **message);

/**
 * Parses dependency solv answer and download and install/update/erase requested packages.
 * @param socket    int socket handler
 * @param action    int type of requested action
 * @return          Returns OK if the recieved json is valid, otherwise error code.
 */
int answer_process(int socket, int action);

/**
 * Download requested packages.
 * @param answer	user's answer 
 * @param install	list of packages to install
 * @param update	list of packages to update
 * @param erase		list of packages to erase  
 * @return          Returns OK, otherwise error code.
 */
int download(int answer, GSList *install, GSList *update, GSList *erase);

/**
 * Install/update/erase requested packages.
 * @param install	list of packages to install
 * @param update	list of packages to update
 * @param erase		list of packages to erase  
 * @return          Returns OK, otherwise error code.
 */
int rpm_process(GSList *install, GSList *update, GSList *erase);

/**
 * Asking user for his wish.
 * @param question  	char* string for print
 * @param possibilities int type (number) of offered possibilities
 * @return              Returns chosen possibility.
 */
int question(char* question, int possibilities);

#ifdef __cplusplus
}
#endif

#endif
