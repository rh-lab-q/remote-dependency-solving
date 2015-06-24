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

#include "log_handler.h"

const char* log_lvl_msg[4] = {"INFO", "MESSAGE", "WARNING", "ERROR", "DEBUG"};
int __verbose = 0;
int __debug = 0;

void ssds_log(int log_level, const char *message, ...)
{
  FILE *flog = fopen(LOG_FILE, "a");
  
  if(flog==NULL)
  {
    fprintf(stderr, "SSDS: ERROR: Unable to open log file\n");
    exit(EXIT_FAILURE);
  }
  
  if(log_level!=logDEBUG)
    fprintf(flog, "SSDS: %s: ", log_lvl_msg[log_level]);//beginning of every message
  
  va_list args;
  va_start(args, message);
  
  switch(log_level){
    case logINFO:
      if(__verbose==1)
      {
        fprintf(stderr, "SSDS: INFO: ");
        vfprintf(stderr, message, args);
      }
      break;
    case logMESSAGE:
      if(__verbose==1)
      {
        fprintf(stderr, "SSDS: MESSAGE: ");
        vfprintf(stderr, message, args);
      }
      break;
    case logWARNING:
      fprintf(stderr, "SSDS: WARNING: ");
      vfprintf(stderr, message, args);
      break;
    case logERROR:
      fprintf(stderr, "SSDS: ERROR: ");
      vfprintf(stderr, message, args);
      break;
    case logDEBUG:
      if(__debug==1)
      {
        fprintf(stderr, "SSDS: DEBUG: ");
        vfprintf(stderr, message, args);
      }
      break;
  }
  
  if(log_level!=logDEBUG)
  {
    va_start(args, message);
    vfprintf(flog, message, args);
  }
  
  fclose(flog);
  va_end(args);
}

void set_verbose()
{
  __verbose=1;
}

void unset_verbose()
{
  __verbose=0;
}

void set_debug()
{
  __debug=1;
}

void unset_debug()
{
  __debug=0;
}