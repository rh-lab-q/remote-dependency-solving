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
    struct utsname *machine = (struct utsname *)malloc(sizeof(struct utsname));

    if(machine == NULL){
	ssds_log(logERROR, "Not enough memory on heap.\n");
        return;
    }
    if(uname(machine) < 0){
	ssds_log(logERROR, "Unable to found system type and computer architecture.\n");
	free(machine);
        return;
    }

    char *end = strrchr(machine->release, '.');

    if(end == NULL){
	ssds_log(logERROR, "Internal error - unable to find dot in release string.\n");
	free(machine);
	return;
    }

    char *i = end-1;

    while(i > machine->release && isdigit(i[0]))
	i--;

    int length = end - i;
    char *fedora_version = (char *)malloc(length*sizeof(char));

    if(fedora_version == NULL){
	ssds_log(logERROR, "Not enough memory on heap.\n");
	free(machine);
	return;
    }

    i++; length--;

    strncpy(fedora_version, i, length); //22 or 21 or any other
    fedora_version[length] = '\0';

    //composing path to @System.solv file
    snprintf(ret_val, 100, "/var/cache/dnf/%s/%s/@System.solv",machine->machine,fedora_version);
    free(machine);
    free(fedora_version);
}

int progress_callback(void *data, double total, double downloaded){
        if(total > 0){
                printf("\r%.40s\t%.0f%%",(char *)data, (downloaded/total)*100);
                fflush(stdout);
        }
        return 0;
}

int end_callback(void *data, LrTransferStatus status, const char *msg){
        if(status == LR_TRANSFER_SUCCESSFUL){
                printf("\r%.40s\t%s\n",(char *)data,"100% - Downloaded.");
        }else{
                printf("\r%.40s\t%s\n",(char *)data,msg);
        }
        return status;
}

