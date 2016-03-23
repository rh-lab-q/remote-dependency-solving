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

#include "includes.h"

//LIBREPO
#include <librepo/repoconf.h>
#include <librepo/librepo.h>

//SSDS
#include "json_handler.h"

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
typedef struct LocalRepoInfo LocalRepoInfo;

struct LocalRepoInfo {
    LrYumRepoConfs* repoHandler; /**< List of repo structures */
};

/**
 * Returns newly allocated LocalRepoInfo
 * @return          LocalRepoInfo*
 */
LocalRepoInfo* repo_parse_init();

/**
 * Parses local .repo files. Function uses librepo class repoconf.h.
 * Information about repos are saved to LocalRepoInfo
 * @param repo      LocalRepoInfo
 * @return          int
 */
int parse_default_repo(LocalRepoInfo* repo);

/**
 * Gets urls of local repos from LocalRepoInfo and inserts them
 * into JsonCreate structure so that they can be sent to server
 * @param repo      LocalRepoInfo*
 * @param json      JsonCreate*
 * @param arch      char*
 * @param release   char*
 */
void get_repo_urls(LocalRepoInfo* repo, JsonCreate* json, char* arch, char* release);

/*****************************************************************/
/* Repo metadata - information about metadata location on server */
/*****************************************************************/

/**
 * GSList of MetadataFilesLoc
 */
typedef struct RepoMetadataList RepoMetadataList;

/**
 * Structure is used by server to hold information about downloaded metadata
 */
typedef struct MetadataFilesLoc MetadataFilesLoc;

struct RepoMetadataList {
    GSList *files_locations; /**< GSList of MetadataFilesLoc */
};

struct MetadataFilesLoc {
    char* repomd;     /**< path to repomod.xml file */
    char* filelists;  /**< path to filelists.xml */
    char* primary;    /**< path to primary.xml */
    char* repo_name;  /**< name of repo for later use */
};

/**
 * Returns newly allocated RepoMetadataList
 * @return RepoMetadataList*
 */
RepoMetadataList* repo_metadata_init();//

/**
 * Function tries to locate metadata about repos provided by json. 
 * If such metadata are not found on server, they are downloaded. 
 * Find local copy by using local_repo_metadata and download them by
 * using download_repo_metadata_by_url.
 * @param json      JsonRead*
 * @param info_list RepoInfoList*
 * @param meta_list RepoMetadataList*
 * @return
 */
int locate_repo_metadata(/*JsonRead* json, */RepoInfoList* info_list, RepoMetadataList* meta_list);//

/**
 * Function tries to locate metadata for one repo provided in repo.
 * This works only for one repo - needs to be called in loop to find all.
 * @param repo      RepoInfo*
 * @param list      RepoMetadataList*
 * @return          1 if metadata were found locally, otherwise returns 0
 */
int local_repo_metadata(RepoInfo* repo, RepoMetadataList* list);//

/**
 * Function appends name of repo to full path to dir containing metadata on server
 * @param repo_name char*
 * @return          full path to repo as char*
 */
char* full_path_to_metadata(char* repo_name);//

/**
 * Callback function for downloading metadata
 * @param data	void*
 * @param total double
 * @param now	double
 * @return      0
 */
int metadata_progress(G_GNUC_UNUSED void *data, double total, double now);

/**
 * Function tries to download metadata for repo by using full url of repository
 * @param repo      RepoInfo*
 * @param list      RepoMetadataList*
 */
void download_repo_metadata_by_url(RepoInfo* repo, RepoMetadataList* list);

/**
 * Redeclaration of function strdup
 * @param s	    const char*
 * @return          char *
 */
char *strdup(const char *s);
#ifdef __cplusplus
}
#endif

#endif /* _REPO_HANDLER_H */
