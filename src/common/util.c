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
#include <syslog.h>

#include "util.h"
#include "log_handler.h"


int resolve_dependency_file_path(char * ret_val, char** arch, char** release) {
    #ifdef DEBUG
        printf("resolve_dependency_file_path\n");
    #endif
    *release = NULL;
    struct utsname *machine;

    machine = (struct utsname *)rds_malloc(sizeof(struct utsname));

    if(uname(machine) < 0){
        syslog("Unable to retreive system info");
        fprintf(stderr, "ERROR: Unable to retreive system information.\n");
        free(machine);
        return 1;
    }

    char *end = strrchr(machine->release, '.');

    if(end == NULL) {
        syslog(LOG_ERR, "strrchr failed");
        fprintf(stderr, "ERROR: Unable to find dot in release string.\n");
        free(machine);
        return 1;
    }

    char *i = end-1;

    while(i > machine->release && isdigit(i[0]))
        i--;
    
    int length = end - i;
    char *fedora_release = (char *)rds_malloc(length*sizeof(char));

    i++; length--;

    strncpy(fedora_release, i, length); //22 or 21 or any other
    fedora_release[length] = '\0';

    //composing path to @System.solv file
    snprintf(ret_val, 100, "/var/cache/dnf/x86_64/21/@System.solv");
    *arch = machine->machine;
    *release = fedora_release;
    
    return 0;
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
    if (!m && len) rds_out_of_memory();
    return m;
}

void rds_out_of_memory(){
    syslog(LOG_ERR, "RDS out of memory");
    fprintf(stderr, "RDSolving out of memory\n");
    abort();
    exit(1);
}

void rds_signal_handler(int signum) {
    syslog("Ended with signal %d", signum);
    fprintf(stderr, "RDS ended with signal number %d.\n", signum);
    exit(signum);
}

