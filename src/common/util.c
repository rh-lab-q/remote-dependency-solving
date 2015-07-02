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

#include "util.h"

void ssds_resolve_dependency_file_path(char * ret_val)
{
    system("uname -r > version.tmp"); //printing system info to file

    FILE * f = fopen("version.tmp","r");
    int c, i = 0;
    char fedora_version[10]; //22 or 21 or any other
    char system_version[7]; //64bit(x86_64) or 32bit(i686)
    //char* path;

    while((c = fgetc(f)) != 'c'); //skipping to fedora version

    //reading fedora version
    while(isdigit(c = fgetc(f)))
    {
        fedora_version[i] = c;
        i++;
    }
    fedora_version[i] = '\0';
    i = 0;

    //reading system version
    while(isalnum(c = fgetc(f)) || (c == '_'))
    {
        system_version[i] = c;
        i++;
    }
    system_version[i] = '\0';

    //composing path to @System.solv file
    snprintf(ret_val, 100, "/var/cache/dnf/%s/%s/@System.solv",system_version, fedora_version);
    fclose(f);
    system("rm version.tmp"); //removing temporary file
}
