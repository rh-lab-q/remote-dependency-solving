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

#ifndef _LOG_HANDLER_H
#define _LOG_HANDLER_H
  
#include "includes.h"
#ifdef __cplusplus
extern "C"{
#endif

/** Location of logging file */
#define LOG_FILE "/tmp/ssds.log"
  
/** 
 * Used for logging severence level
 */
enum LogClass{
  logINFO=0,        /**< INFO is for general info messages - prints only when __verbose == 1 */
  logMESSAGE,       /**< MESSAGE is for messages for user */
  logWARNING,       /**< WARNING is for describing unorthodox or undefined behaviour */
  logERROR,         /**< ERROR is for fatal situations */
  logDEBUG,         /**< DEBUG is only for debugging - prints only when __debug == 1*/
  logSSDS	    /**< SSDS is just for first and last message of server and client - printed everytime */
};        
  
/** Contains severence level message for ssds_log function */
extern const char* log_lvl_msg[6];

/**
 * Used on global scope to indicate if verbose option is on. 
 * If __verbose == 0 - ssds_log prints only WARNING and ERROR to stderr.
 * Set to 0 by default.
 */
extern int __verbose;

/** 
 * Used on global scope to indicate if debug option is on. 
 * If __debug==0 - ssds_log doesn't print anything with logDEBUG.
 * Set to 0 by default.
 */
extern int __debug;

/**
 * Used for logging. Works the same way as printf() but takes 
 * severity level as an argument. Prints given message into file 
 * and to stderr. By default doesn't print INFO, MESSAGE and DEBUG 
 * to stderr. Use set_verbose to turn on INFO and MESSAGE output to
 * stderr and set_debug to turn on DEBUG output to stderr.
 * The message supports formatting as printf with %d, %s, \n etc
 * @param log_level int
 * @param message   char*
 */
void ssds_log(int log_level, const char *message, ...);


/**
 * Sets __verbose to 1. This will allow INFO and MESSAGE to be printed
 * to stderr. 
 */
void set_verbose();

/**
 * Sets __verbose to 0.
 */
void unset_verbose();

/** 
 * Sets __debug to 1. This will allow DEBUG messages to be printed
 * to stderr and to log file.
 */
void set_debug();

/**
 * Sets __debug to 0.
 */
void unset_debug();

#ifdef __cplusplus
}
#endif

#endif /* _LOG_HANDLER_H */
