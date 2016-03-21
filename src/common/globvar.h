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
       SEND_SOLV,//2
       SEND_YUM_CONF, //3
       SOLV_MORE_FRAGMENT,//4
       SOLV_NO_MORE_FRAGMENT,//5
       GET_DEPENDENCY,//6
       GET_INSTALL,//7
       GET_UPDATE,//8
       GET_ERASE,//9
			 GET_UPDATE_ALL,//10
       ANSWER_OK,//11
       ANSWER_WARNING,//12
       ANSWER_ERROR,//13
			 ANSWER_NO_DEP //14 when dependencies are not ok
  };
	
	typedef enum {
	JS_ARR_INSTALL = 0,
	JS_ARR_UPGRADE,
	JS_ARR_ERASE,
	JS_ARR_OBSOLETE,
	JS_ARR_UNNEEDED
	}RdsJsArrayName;

	static const char* const JsArrayStr[] = {"install", "upgrade", "erase", "obsolete", "unneeded"};


#ifdef __cplusplus
}
#endif
#endif
