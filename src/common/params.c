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

#include "params.h"
#include "includes.h"
#include "util.h"
#include "errors.h"



int parse_params_cl(int argc, char* argv[], ParamOptsCl* params) {
    if(argc==1) {
        rds_log(logERROR, "No command provided. You need to give some command.\n");
        print_commands_cl();
        return 2;
    }

    static int param_opt; //needs to be static or else make fails
    static struct option long_options[] = {
        {"install", no_argument, &param_opt, PAR_INSTALL},
        {"chkdep", no_argument, &param_opt, PAR_CHK_DEP},
        {"update", no_argument, &param_opt, PAR_UPDATE},
        {"remove", no_argument, &param_opt, PAR_ERASE},
        {"help", no_argument, 0, 'h'}
    };

    int c;
    int opt_index = 0;
    int seen = 0;
    extern int optind;
    opterr = 0;

    for(;;) {
        c = getopt_long(argc, argv, "vhd", long_options, &opt_index);
        switch(c) {
            case 0:
                params->command = long_options[opt_index].val;
                seen++;
            break;

            case 'h':
                print_help_cl();
                exit(EXIT_SUCCESS);
            break;

            case 'v':
                set_verbose();
            break;

            case 'd':
                set_debug();
            break;

            case '?':
                print_help_cl();
                return -1;
            break;
        }

        if(c == -1)
            break;
    }

    if(seen > 1) {
        rds_log(logMESSAGE, "Choose either install, update, erase or chkdep. The program will terminate now.\n");
        rds_log(logERROR, "Wrong parameter combination. Terminating.\n");
        return -1;
    }

    if(seen == 0) {
        rds_log(logERROR, "No command provided. The program will terminate now.\n");
        return 3;
    }

    if(optind < argc) {
        while(optind < argc) {
            params->pkgs = g_slist_append(params->pkgs, argv[optind++]);
            params->pkg_count++;
        }
    }
    return 0;
}

ParamOptsCl* init_params_cl() {
    ParamOptsCl* new;

    new = (ParamOptsCl*)malloc(sizeof(ParamOptsCl));
    new->pkg_count = 0;
    new->command = -1;
    new->pkgs = NULL;
    return new;
}


void free_params_cl(ParamOptsCl* params) {
    g_slist_free(params->pkgs); //only *char in the list so free will suffice
    free(params);
}


void parse_params_srv(int argc, char* argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "vhd")) != -1) {
        switch (opt) {
            case 'v':
                set_verbose();
            break;

            case 'h':
                print_help_srv();
                exit(EXIT_SUCCESS);
            break;

            case 'd':
                set_debug();
            break;

            default: /* '?' */
                print_help_srv();
                exit(PARAMS_ERROR);
            break;
        }
    }
}


void print_help_cl() {
    printf("Usage: ./rds-client <commands> [<packages> ...]\n\n"
        "List of Commands\n\n"
        "--install\t\tResolve dependencies and install packages\n"
        "--update\t\tResolve dependencies and update packages\n"
        "--remove\t\t\tRemove packages\n"
        "--chkdep\t\tOnly show required packages - do not install yet\n"
        "--help, -h\t\tDisplays help\n"
        "-v\t\t\tVerbose - turned off by default\n"
        "-d\t\t\tDebug - turned off by default\n"
    );
}

void print_help_srv() {
    printf("Usage: ./rds-server <commands>\n\n"
        "List of Commands\n\n"
        "-h\tDisplays help\n"
        "-v\tVerbose - turned off by default\n"
        "-d\tDebug - turned off by default\n"
    );
}

void print_commands_cl()
{
    printf("./rds-client <commands> [<packages> ...]\n\n"
            "List of main commands\n\n"
            "install\n"
            "remove\n"
            "update\n"
            "help\n");
}

