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

#ifndef _SOLVING_H
#define _SOLVING_H

#ifdef __cplusplus
extern "C"{
#endif

//SSDS
#include "repo_handler.h"
#include "json_handler.h"
#include "log_handler.h"
#include "mem_management.h"

//GLIB
#include "includes.h"

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>

//LIBREPO
#include <librepo/yum.h>
#include <librepo/handle.h>


// HySack* ssds_solve_init();
  
/**
 * Fills sack object with information from all repositories provided by client
 * @param sack      HySack*
 * @param list      SsdsRepoMetadataList*
 */
void ssds_fill_sack(HySack* sack, SsdsRepoMetadataList* list);

/**
 * Function connects input with output and dependency solving. 
 * Takes information from client and passes them for dependency solving to
 * ssds_dep_query. Ssds_dep_query adds each result to SsdsJsonCreate.
 * @param client_data         SsdsJsonRead*
 * @param answer              SsdsJsonCreate*
 * @param sack                HySack*
 */
//void ssds_dep_answer(SsdsJsonRead *client_data, SsdsJsonCreate* answer, HySack* sack);

/**
 * Solves dependency for one package. Needs to be called in loop for all packages.
 * Used by ssds_dep_answer.
 * @param request   const char*
 * @param answer    SsdsJsonCreate*
 * @param sack      HySack*
 */
int ssds_dep_query(const char** request, SsdsJsonCreate* answer, HySack* sack, int operation, int pkg_count);


#ifdef __cplusplus
}
#endif

#endif /* _SOLVING_H */
