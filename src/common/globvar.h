/**
 * Server side dependency solving 
 * transfer of dependency solving from local machine to server when installing new packages
 * 
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký, Walter Scherfel, Šimon Matěj
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

#ifndef _GLOBVAR_H
#define _GLOBVAR_H

#ifdef __cplusplus
extern "C"{
#endif

  /**********************************************************/
  /* Message codes used in json communication on both sides */
  /**********************************************************/

  enum{
       GENERATE_ID = 1,	
       SEND_SOLV,
       SOLV_MORE_FRAGMENT,
       SOLV_NO_MORE_FRAGMENT,
       GET_DEPENDENCY,
       GET_INSTALL,
       GET_UPDATE,
       GET_ERASE,
			 GET_UPDATE_ALL,
       ANSWER_OK,
       ANSWER_WARNING,
       ANSWER_ERROR
  };


#ifdef __cplusplus
}
#endif
#endif
