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

const char* log_lvl_msg[7] = {"INFO", "MESSAGE", "WARNING", "ERROR", "DEBUG", "SSDS", "QUESTION"};
int __verbose = 0;
int __debug = 0;

void rds_log(int log_level, const char *message, ...)
{
  FILE *flog = fopen(LOG_FILE, "a");
  time_t current_time = time(NULL);
  struct tm *ts = localtime(&current_time);
  
  if(flog == NULL)
  {
    fprintf(stderr, "[%d/%d/%d %02d:%02d:%02d SSDS ERROR]: Unable to open log file.\n",
            ts->tm_mday, ts->tm_mon+1, ts->tm_year-100, 
            ts->tm_hour, ts->tm_min, ts->tm_sec);
    exit(EXIT_FAILURE);
  }
  
  if(log_level != logDEBUG)
    fprintf(flog, "[%d/%d/%d %02d:%02d:%02d %s]: ", 
            ts->tm_mday, ts->tm_mon+1, ts->tm_year-100,
            ts->tm_hour, ts->tm_min, ts->tm_sec,
            log_lvl_msg[log_level]);//beginning of every message
  
  va_list args;
  va_start(args, message);
  
  switch(log_level){
    case logSSDS:
      fprintf(stdout, "\033[92m[%d/%d/%d %02d:%02d:%02d SSDS]\033[0m: ",
                ts->tm_mday, ts->tm_mon+1, ts->tm_year-100,
                ts->tm_hour, ts->tm_min, ts->tm_sec);
      vfprintf(stdout, message, args);
      break;

    case logQUESTION:
      fprintf(stdout, "\033[1;92m[QUESTION]\033[0m: ");
      vfprintf(stdout, message, args);
      break;

    case logINFO:
      if(__verbose == 1)
      {
        fprintf(stdout, "\033[96m[%d/%d/%d %02d:%02d:%02d INFO]\033[0m: ",
                ts->tm_mday, ts->tm_mon+1, ts->tm_year-100,
                ts->tm_hour, ts->tm_min, ts->tm_sec);
        vfprintf(stdout, message, args);
      }
      break;

    case logMESSAGE:
      fprintf(stdout, "\033[94m[MESSAGE]\033[0m: ");
      vfprintf(stdout, message, args);
      break;

    case logWARNING:
      fprintf(stderr, "\033[93m[%02d:%02d:%02d WARNING]\033[0m: ",
              ts->tm_hour, ts->tm_min, ts->tm_sec);
      vfprintf(stderr, message, args);
      break;

    case logERROR:
      fprintf(stderr, "\033[91m[%02d:%02d:%02d ERROR]\033[0m: ",
              ts->tm_hour, ts->tm_min, ts->tm_sec);
      vfprintf(stderr, message, args);
      break;
    case logDEBUG:
      if(__debug == 1)
      {
        fprintf(stderr, "\033[95m[%02d:%02d:%02d DEBUG]\033[0m: ",
                ts->tm_hour, ts->tm_min, ts->tm_sec);
        vfprintf(stderr, message, args);
      }
      break;
  }
  
  if(log_level != logDEBUG)
  {
    va_start(args, message);
    vfprintf(flog, message, args);
  }
  fclose(flog);
  va_end(args);
}

void set_verbose()
{
  __verbose = 1;
}

void unset_verbose()
{
  __verbose = 0;
}

void set_debug()
{
  __debug = 1;
}

void unset_debug()
{
  __debug = 0;
}
