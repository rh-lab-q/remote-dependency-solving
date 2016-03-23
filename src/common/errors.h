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

#ifndef _ERRORS_H_
#define _ERRORS_H_

enum ERRORS
{
    OK = 0,
    SOCKET_ERROR,     //1
    NETWORKING_ERROR, //2
    INTERNAL_ERROR,   //3
    FILE_ERROR,       //5
    PARAMS_ERROR,     //6
    REPO_ERROR,       //7
    JSON_ERROR,       //8
    DOWNLOAD_ERROR,   //9
    MEMORY_ERROR,     //10
    ROOT_ERROR,       //11
    RPM_ORDER_ERROR,  //12
    INSTALL_ERROR,    //13
    UPDATE_ERROR,     //14
    ERASE_ERROR,      //15
    ACTION_ERROR,     //16
    EXIT              //17
};

#endif
