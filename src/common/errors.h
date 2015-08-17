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

#ifndef _ERRORS_H_
#define _ERRORS_H_

enum ERRORS
{
	OK = 0,
	SOCKET_ERROR,
	NETWORKING_ERROR,
	INTERNAL_ERROR,
	FILE_ERROR,
	PARAMS_ERROR,
	REPO_ERROR,
	JSON_ERROR,
	DOWNLOAD_ERROR,
	MEMORY_ERROR,
	ROOT_ERROR,
	RPM_ORDER_ERROR,
	ERASE_ERROR
};

#endif
