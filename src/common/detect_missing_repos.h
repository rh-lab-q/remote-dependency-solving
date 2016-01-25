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

#ifndef _DETECT_MISSING_REPOS_H
#define _DETECT_MISSING_REPOS_H

#include <dirent.h>
#include "../common/includes.h"
#include "../common/log_handler.h"
#include "../common/mem_management.h"
#include "../common/cfg_parsing.h"

#define REPO_LIST_FILE "../required_repos_list.txt"
#define LOCAL_REPOS_LOCATION "/etc/yum.repos.d/"
#define REPO_NAME_MIN_LENGTH 4

/*
 * returns 1 if everything is OK, -1 when error (for ex. missing file)
 * and number >=1 if that number of repos is missing
 */
int check_for_missing_repos(void);

#endif