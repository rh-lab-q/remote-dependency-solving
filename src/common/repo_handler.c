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

#include "repo_handler.h"

SsdsLocalRepoInfo* ssds_repo_parse_init()
{ //this part uses librepo library to parse .repo files - repoconf module was created by TMlcoch
  SsdsLocalRepoInfo* new = (SsdsLocalRepoInfo*) malloc(sizeof(SsdsLocalRepoInfo));
  new->repoHandler = lr_yum_repoconfs_init(); 
  
  return new;
}

  
int ssds_parse_default_repo(SsdsLocalRepoInfo* repo)
{
  GError **err;
  gboolean ret = lr_yum_repoconfs_load_dir(repo->repoHandler, "/etc/yum.repos.d/", err);
   if(!ret){
	ssds_log(logERROR, "Unable to load default repo\n");
        return 0;
   }
   return 1; 
}

void ssds_get_repo_urls(SsdsLocalRepoInfo* repo, SsdsJsonCreate* json)
{
  GError* err = NULL;
  GSList* list = lr_yum_repoconfs_get_list(repo->repoHandler, &err);
  
  for(unsigned int i=0; i<g_slist_length(list); i++){
    char** url;
    char* name;
    short type = 0;
    void * val;
    
    LrYumRepoConf * conf = (LrYumRepoConf*)g_slist_nth_data(list, i);
    
    lr_yum_repoconf_getinfo(conf, &err, LR_YRC_ENABLED, &val);
    
    if((long int)val){
      err = NULL;
      if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_ID, &val) != 0){
        name = (char*)val;
      }
      
      err = NULL;
      if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_MIRRORLIST, &val) != 0){
        url[0] = (char*)val;
        type = SSDS_MIRRORLIST;
      }
      
      err = NULL;
      if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_METALINK, &val) != 0){
        url[0] = (char*)val;
        type = SSDS_METALINK;
      }
      
      err = NULL;
      //void ** val2;
      if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_BASEURL, &val) != 0){
        url = (char**)val; 
        type = SSDS_BASEURL;
      }
      
      LrUrlVars *list =  lr_urlvars_set(NULL, "releasever", "21"); //"3.18.6-200.fc21"
      list = lr_urlvars_set(list, "basearch", "x86_64");
      
      int k=0;
      while(url[k]!=NULL)
        k++;
      
      char **url_subst_list=(char**)malloc((k+1)*sizeof(char*));
      char* url_copy;
      char* url_subst;
      
      for(int j=0; j<k; j++)
      { 
        url_copy = strdup(url[j]);
        url_subst = lr_url_substitute(url_copy, list);
        url_subst_list[j]=strdup(url_subst);
        free(url_copy);
        free(url_subst);
      }
      url_subst_list[k]=NULL;
      
      ssds_js_add_repo(json,url_subst_list, name, type, k);
      
      for(int j=0; j<=k; j++)
        free(url_subst_list[j]);
      
      free(url_subst_list);
    }
  }
}
