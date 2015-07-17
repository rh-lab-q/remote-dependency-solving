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

#include "solving.h"

// HySack* ssds_solve_init()
// {
//   HySack sack = hy_sack_create(NULL, NULL, NULL,HY_MAKE_CACHE_DIR);
//     
//   if(hy_sack_load_system_repo(sack, NULL, HY_BUILD_CACHE))
//     return NULL;
//   
//   HySack* ret;
//   ret = &sack;
//   return ret;
// }

void ssds_fill_sack(HySack* sack, SsdsRepoMetadataList* list)
{
 // for(ssds_repo::metadata_files_location* loc : metadata.files_locations)
  guint i;
  for(i=0; i<g_slist_length(list->files_locations); i++)
  {
    HyRepo repo = hy_repo_create("solving");
    SsdsMetadataFilesLoc* file = (SsdsMetadataFilesLoc*)g_slist_nth_data(list->files_locations, i);
    ssds_log(logDEBUG, "repomd: %s\n filelists: %s\nprimary: %s\n", file->repomd, file->filelists, file->primary);
    
    int path_len = strlen(file->repomd)+strlen("/repodata/repomd.xml");
    char* repomd_path = (char*)malloc((path_len+1)*sizeof(char));
    memset(repomd_path, '\0', path_len+1);
    strcpy(repomd_path, file->repomd);
    strcat(repomd_path, "/repodata/repomd.xml");
    
    hy_repo_set_string(repo, HY_REPO_MD_FN, repomd_path);
    hy_repo_set_string(repo, HY_REPO_PRIMARY_FN, file->primary);
    hy_repo_set_string(repo, HY_REPO_FILELISTS_FN, file->filelists);
    
    hy_sack_load_yum_repo(*sack, repo, 0);
    ssds_log(logDEBUG, "One repo loaded to sack\n");
  }
}

void ssds_dep_query(const char* request, SsdsJsonCreate* answer, HySack* sack)
{
  HyQuery query = hy_query_create(*sack);
  hy_query_filter(query, HY_PKG_NAME, HY_SUBSTR, request);
  hy_query_filter(query, HY_PKG_REPONAME, HY_NEQ, HY_SYSTEM_REPO_NAME);
  hy_query_filter_latest_per_arch(query, 1);
    
  /* Getting list of packages from the query */
  HyPackageList plist = hy_packagelist_create();
  plist = hy_query_run(query);
    
  //HyPackage test;
  ssds_log(logMESSAGE,"No. of packages found by query: %d\n", hy_packagelist_count(plist));
  
  /*int i;
  for(i=0; i<hy_packagelist_count(plist);i++)
  {
    test = hy_packagelist_get(plist, i);
//       std::cout<< "baliky jsou na: " << hy_package_get_url(test) << ":"<< hy_package_get_location(test)<< std::endl;
  }*/
    
  HyPackage pkg;
  pkg = hy_packagelist_get(plist, 0);
    
  /* GOAL */
  HyGoal goal = hy_goal_create(*sack);
  hy_goal_install(goal, pkg);
  if(hy_goal_run(goal)==0)
    ssds_log(logMESSAGE, "Dependencies are ok.\n");
    
//     answer.install_pkgs_init();
  ssds_js_pkgs_insert(answer, &goal, request);
//     answer.install_pkgs_insert(&goal, request);
}


void ssds_dep_answer(SsdsJsonRead *client_data, SsdsJsonCreate* answer, HySack* sack)
{
  ssds_log(logDEBUG,"Dependencies answer.\n");
  SsdsPkgInfo* pkgs = ssds_read_pkginfo_init();
  ssds_read_get_packages(pkgs, client_data);
  
  ssds_log(logDEBUG,"Answer after get_packages\n");
  int i;
  for(i=0; i<pkgs->length; i++)
  {
    ssds_log(logDEBUG,"Answer in for loop.\n");
    ssds_dep_query(pkgs->packages[i], answer, sack);
//     query(pkgs->packages[i], answer);
  }
}
