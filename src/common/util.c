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
#include <sys/utsname.h>

#include "util.h"
#include "log_handler.h"


void resolve_dependency_file_path(char * ret_val, char** arch, char** release) {
    *release = NULL;
    struct utsname *machine;

    rds_log(logDEBUG, "Got info about machine.\n");
    
    machine = (struct utsname *)malloc(sizeof(struct utsname));
    if(machine == NULL){
        rds_log(logERROR, "Not enough memory on heap.\n");
        return;
    }

    rds_log(logDEBUG, "Getting info about release and arch.\n");

    if(uname(machine) < 0){
        rds_log(logERROR, "Unable to found system type and computer architecture.\n");
        free(machine);
        return;
    }

    rds_log(logDEBUG, "Getting last dot in release string.\n");
    char *end = strrchr(machine->release, '.');

    if(end == NULL) {
        rds_log(logERROR, "Internal error - unable to find dot in release string.\n");
        free(machine);
        return;
    }

    char *i = end-1;
    rds_log(logDEBUG, "Search release number.\n");

    while(i > machine->release && isdigit(i[0]))
        i--;
    
    rds_log(logDEBUG, "Getting release string length.\n");

    int length = end - i;
    rds_log(logDEBUG, "Allocating memory for release string.\n");
    char *fedora_release = (char *)malloc(length*sizeof(char));

    if(fedora_release == NULL) {
        rds_log(logERROR, "Not enough memory on heap.\n");
        free(machine);
        return;
    }
   
    rds_log(logDEBUG, "Memory allocated.\n");

    i++; length--;

    rds_log(logDEBUG, "Copying release number into new string.\n");
    strncpy(fedora_release, i, length); //22 or 21 or any other
    fedora_release[length] = '\0';

    rds_log(logDEBUG, "Getting path string.\n");
    //composing path to @System.solv file
    snprintf(ret_val, 100, "/var/cache/dnf/@System.solv");
    rds_log(logDEBUG, "Getting arch string.\n");
    *arch = machine->machine;
    *release = fedora_release;
}

int progress_callback(void *data, double total, double downloaded){
    if(total > 0) {
        printf("\r%.40s\t%.0f%%",(char *)data, (downloaded/total)*100);
        fflush(stdout);
    }
    return 0;
}

int end_callback(void *data, LrTransferStatus status, const char *msg){
    if(status == LR_TRANSFER_SUCCESSFUL)
        printf("\r%.40s\t%s\n",(char *)data,"100% - Downloaded.");
    else
        printf("\r%.40s\t%s\n",(char *)data,msg);
    
    return status;
}

void * rds_malloc(size_t len){
    void *m = malloc(len);
    if (!m) lr_out_of_memory();
    return m;
}

void* rds_realloc(void* ptr, size_t len){
    void *m = realloc(ptr, len);
    if (!m && len) lr_out_of_memory();
    return m;
}

void rds_out_of_memory(){
    fprintf(stderr, "Remote Dependency Solving out of memory\n");
    abort();
    exit(1);
}

void rds_signal_handler(int signum) {
    rds_log(logERROR, "End with signal number %d.\n", signum);
    exit(signum);
}

