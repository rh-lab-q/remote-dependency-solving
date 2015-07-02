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

#ifndef _REPO_HANDLER_H
#define _REPO_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <sys/stat.h>
#include <unistd.h>

//LIBREPO
#include <librepo/repoconf.h>
#include <librepo/librepo.h>

//SSDS
#include "json_handler.h"
#include "log_handler.h"

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>

#ifdef __cplusplus
extern "C"{
#endif

/************************************************************/
/* Parse repo - getting information from local .repo files  */
/************************************************************/

/**
 * Structure for information about local repositories on client machine
 */
typedef struct SsdsLocalRepoInfo SsdsLocalRepoInfo;

struct SsdsLocalRepoInfo{
  LrYumRepoConfs* repoHandler; /**< List of repo structures */
};

/**
 * Returns newly allocated SsdsLocalRepoInfo
 * @return          SsdsLocalRepoInfo*
 */
SsdsLocalRepoInfo* ssds_repo_parse_init();

/**
 * Parses local .repo files. Function uses librepo class repoconf.h.
 * Information about repos are saved to SsdsLocalRepoInfo
 * @param repo      SsdsLocalRepoInfo
 */
void ssds_parse_default_repo(SsdsLocalRepoInfo* repo);

/**
 * Gets urls of local repos from SsdsLocalRepoInfo and inserts them
 * into SsdsJsonCreate structure so that they can be sent to server
 * @param repo      SsdsLocalRepoInfo*
 * @param json      SsdsJsonCreate*
 */
void ssds_get_repo_urls(SsdsLocalRepoInfo* repo, SsdsJsonCreate* json);

/*****************************************************************/
/* Repo metadata - information about metadata location on server */
/*****************************************************************/

/**
 * GSList of SsdsMetadataFilesLoc
 */
typedef struct SsdsRepoMetadataList SsdsRepoMetadataList;

/**
 * Structure is used by server to hold information about downloaded metadata
 */
typedef struct SsdsMetadataFilesLoc SsdsMetadataFilesLoc;

struct SsdsRepoMetadataList{
  GSList *files_locations; /**< GSList of SsdsMetadataFilesLoc */
};

struct SsdsMetadataFilesLoc{
  char* repomd;     /**< path to repomod.xml file */
  char* filelists;  /**< path to filelists.xml */
  char* primary;    /**< path to primary.xml */
};

/**
 * Returns newly allocated SsdsRepoMetadataList
 * @return SsdsRepoMetadataList*
 */
SsdsRepoMetadataList* ssds_repo_metadata_init();//

/**
 * Function tries to locate metadata about repos provided by json. 
 * If such metadata are not found on server, they are downloaded. 
 * Find local copy by using local_repo_metadata and download them by
 * using download_repo_metadata_by_url.
 * @param json      SsdsJsonRead*
 * @param info_list SsdsRepoInfoList*
 * @param meta_list SsdsRepoMetadataList*
 * @return
 */
int ssds_locate_repo_metadata(SsdsJsonRead* json, SsdsRepoInfoList* info_list, SsdsRepoMetadataList* meta_list);//

/**
 * Function tries to locate metadata for one repo provided in repo.
 * This works only for one repo - needs to be called in loop to find all.
 * @param repo      SsdsRepoInfo*
 * @param list      SsdsRepoMetadataList*
 * @return          1 if metadata were found locally, otherwise returns 0
 */
int local_repo_metadata(SsdsRepoInfo* repo, SsdsRepoMetadataList* list);//

/**
 * Function appends name of repo to full path to dir containing metadata on server
 * @param repo_name char*
 * @return          full path to repo as char*
 */
char* full_path_to_metadata(char* repo_name);//

/**
 * Function tries to download metadata for repo by using full url of repository
 * @param repo      SsdsRepoInfo*
 * @param list      SsdsRepoMetadataList*
 */
void download_repo_metadata_by_url(SsdsRepoInfo* repo, SsdsRepoMetadataList* list);

#ifdef __cplusplus
}
#endif

#endif /* _REPO_HANDLER_H */