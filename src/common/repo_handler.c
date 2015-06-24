#include "repo_handler.h"

SsdsLocalRepoInfo* ssds_repo_parse_init()
{ //this part uses librepo library to parse .repo files - repoconf module was created by TMlcoch
  SsdsLocalRepoInfo* new = (SsdsLocalRepoInfo*) malloc(sizeof(SsdsLocalRepoInfo));
  new->repoHandler = lr_yum_repoconfs_init(); 
  
  return new;
}

  
void ssds_parse_default_repo(SsdsLocalRepoInfo* repo)
{
  GError **err;
  gboolean ret = lr_yum_repoconfs_load_dir(repo->repoHandler, "/etc/yum.repos.d/", err);
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
      //std::cout << "name: " << name << "\nurl: " << url << std::endl;
      
      
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