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
typedef struct SsdsLocalRepoInfo SsdsLocalRepoInfo;

struct SsdsLocalRepoInfo{
  LrYumRepoConfs* repoHandler;
  LrYumRepoConf* singleRepo;
};

SsdsLocalRepoInfo* ssds_repo_parse_init();
void ssds_parse_default_repo(SsdsLocalRepoInfo* repo);
void ssds_get_repo_urls(SsdsLocalRepoInfo* repo, SsdsJsonCreate* json);

/*****************************************************************/
/* Repo metadata - information about metadata location on server */
/*****************************************************************/
typedef struct SsdsRepoMetadataList SsdsRepoMetadataList;
typedef struct SsdsMetadataFilesLoc SsdsMetadataFilesLoc;

struct SsdsRepoMetadataList{
  GSList *files_locations; //list of SsdsMetadataFilesLoc
//   GSList *urls;
};

struct SsdsMetadataFilesLoc{
  char* repomd;
  char* filelists;
  char* primary;
};

SsdsRepoMetadataList* ssds_repo_metadata_init();//
int ssds_locate_repo_metadata(SsdsJsonRead* json, SsdsRepoInfoList* info_list, SsdsRepoMetadataList* meta_list);//
int local_repo_metadata(SsdsRepoInfo* repo, SsdsRepoMetadataList* list);//
char* full_path_to_metadata(char* repo_name);//
void download_repo_metadata_by_url(SsdsRepoInfo* repo, SsdsRepoMetadataList* list);

#ifdef __cplusplus
}
#endif

#endif