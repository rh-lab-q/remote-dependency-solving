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

#ifndef _PARAMS_H
#define _PARAMS_H


//GLIB
#include <glib.h>
#include <glib-2.0/glib.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-types.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "log_handler.h"
/**
 * Integer option for command in ParamOptsCl
 */
enum ParamsOptions { 
    PAR_NO_OPT = 0,   /**< 0 No option was provided */
    PAR_INSTALL,      /**< 1 Install option chosen */
    PAR_CHK_DEP,      /**< 2 Only check if dependencies are fulfilled */
    PAR_UPDATE,       /**< 3 Update option choosen */
    PAR_ERASE,        /**< 4 Erase option choosen */
    PAR_UPDATE_ALL		/**< 5 Update all packages */
}ParOpt;
 
/********************************/
/*  This part is used by client */
/********************************/

/**
 * Structure hold information parsed from parameters provided by user */
typedef struct ParamOptsCl ParamOptsCl;

struct ParamOptsCl {
    int command;      /**< ParOpt number of command */
    int pkg_count;    /**< Number of packages required by user */
    GSList* pkgs;     /**< Names of packages from command line */
};

/**
 * Parses command line on client side.
 * @param argc      int
 * @param argv      char**
 * @param params    ParamOptsCl*
 * @return          EXIT_SUCCES when all parameters are in order, EXIT_FAILURE otherwise
 */
int parse_params_cl(int argc, char* argv[], ParamOptsCl* params);

/**
 * Returns newly allocated ParamOptsCl
 * @return          ParamOptsCl*
 */
ParamOptsCl* init_params_cl();

void free_params_cl(ParamOptsCl* params);

/**
 * Prints help for client users
 */
void print_help_cl();

/********************************/
/*  This part is used by server */
/********************************/

void parse_params_srv(int argc,char* argv[]);

/**
 * Prints help for server usage
 */
void print_help_srv();

/**
 * Prints available commands for usage available on client
 */
void print_commands_cl();

#ifdef __cplusplus
}
#endif

#endif /* _PARAMS_H */
