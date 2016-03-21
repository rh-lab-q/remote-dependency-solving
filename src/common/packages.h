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

#ifndef _PACKAGES_H
#define _PACKAGES_H

#ifdef __cplusplus
extern "C"{
#endif

#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmdb.h>

// Paths to download folders
static const char *DOWNLOAD_TARGET = "/var/cache/ssds/packages/";
 
// Types of supported operation
enum{
        RDS_INSTALL = 0,
        RDS_UPDATE
};

/**
 * Add package to RPM transaction with requested operation.
 * @param ts        rpmts RPM transaction
 * @param pkg       char* path to package
 * @param action    int type of operation
 * @return          RPM transaction set return value
 */
int add_to_transaction (rpmts ts, char *pkg, int action);

/**
 * Add package to RPM transaction for erase. 
 * @param ts        rpmts RPM transaction
 * @param pkg       char* name of package to erase
 * @return          RPM transaction set return value
 */
int add_to_erase (rpmts ts, char *pkg);

#ifdef __cplusplus
}
#endif

#endif

