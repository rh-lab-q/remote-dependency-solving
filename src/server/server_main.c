/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký
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

#include "server.h"


int main(int argc, char* argv[]) {
    /*******************************************************************/
    /* Setting up garbage collector and setting callback functions */
    /*******************************************************************/
    rds_gc_init();
    signal(SIGINT, rds_signal_handler);
    signal(SIGBUS, rds_signal_handler);
    signal(SIGSEGV, rds_signal_handler);
    signal(SIGTERM, rds_signal_handler);

    int status = OK, comm_desc, comm_addr_len;

    parse_params_srv(argc, argv);

    rds_log(logSSDS, "Server started.\n");
    rds_log(logDEBUG, "Params parsed.\n");

    status = server_init(&comm_desc, &comm_addr_len);
    if(status != OK) goto end;

    while(1) {
        status = server_accept_connection(comm_desc, comm_addr_len);
        if(status == EXIT) goto end;
    }
    
    end:
        rds_gc_cleanup();
        return status;
}
