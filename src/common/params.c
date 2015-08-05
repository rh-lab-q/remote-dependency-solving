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

int parse_params_cl(int argc, char* argv[], ParamOptsCl* params)
{
  if(argc==1)
  {
    ssds_log(logERROR, "No command provided. The program will terminate now.\n");
    return 2;
  }
  
  static int param_opt; //needs to be static or else make fails
  static struct option long_options[] =
  {
    {"install", no_argument, &param_opt, PAR_INSTALL},
    {"chkdep", no_argument, &param_opt, PAR_CHK_DEP},
    {"help", no_argument, 0, 'h'}
  };
  
  int c;
  int opt_index = 0;
  int seen = 0;
  extern int optind;
  
  while(1)
  {
    c = getopt_long(argc, argv, "vhd", long_options, &opt_index);
    switch(c)
    {
      case 0:
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
        exit(EXIT_FAILURE);
        break;
    }
    
    if(c == -1)
      break;
  }

  if(seen > 1)
  {
    ssds_log(logMESSAGE, "Choose either install or chkdep. The program will terminate now.\n");
    ssds_log(logERROR, "Wrong parameter combination. Terminating.\n");
    return -1;
  }
  
  if(seen == 0)
  {
    ssds_log(logERROR, "No command provided. The program will terminate now.\n");
    return 3;
  }
  if(optind < argc)
  {
    while(optind < argc)
    {
      params->pkgs = g_slist_append(params->pkgs, argv[optind++]);
      params->pkg_count++;
    }
  }
  return 0;
}

ParamOptsCl* init_params_cl()
{
  ParamOptsCl* new = (ParamOptsCl*)ssds_malloc(sizeof(ParamOptsCl));
  new->pkg_count = 0;
  new->command = -1;
  new->pkgs = NULL;
  return new;
}


void free_params_cl(ParamOptsCl* params)
{
  g_slist_free_full(params->pkgs, (GDestroyNotify) free); //only *char in the list so free will suffice
  ssds_free(params);
}


void parse_params_srv(int argc, char* argv[])
{
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
        exit(EXIT_FAILURE);
    }
  }
}


void print_help_cl()
{
  printf("Usage: ./ssds-client <commands> [<packages> ...]\n\n"
         "List of Commands\n\n"
         "--install\t\tResolve dependencies and install packages\n"
         "--checkdep\t\tOnly show required packages - do not install yet\n"
         "--help, -h\t\tDisplays help\n"
         "-v\t\t\tVerbose - turned off by default\n"
         "-d\t\t\tDebug - turned off by default\n"
  );
}

void print_help_srv()
{
  printf("Usage: ./ssds-server <commands>\n\n"
         "List of Commands\n\n"
         "-h\tDisplays help\n"
         "-v\tVerbose - turned off by default\n"
         "-d\tDebug - turned off by default\n"
  );
}
