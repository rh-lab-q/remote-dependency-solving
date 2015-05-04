#include "repo_handler.hpp"

/* 
 * 
 * 
 * 
 */

namespace ssds_repo{
  parse_repo::parse_repo()
  {
    //this part uses librepo library to parse .repo files - repoconf module was created by TMlcoch
    repoHandler = lr_yum_repoconfs_init(); 
  }
  
  /*
   * Parses .repo files in default location /etc/yum.repos.d
   */
  void parse_repo::parse_default_repo()
  {
    GError **err;
    gboolean ret = lr_yum_repoconfs_load_dir(repoHandler, "/etc/yum.repos.d/", err);
  }
  
  /*
  * Parses .repo files, creates nodes in xml representing repo name and url (baseurl | metalink | mirrorlist)
  */
  void parse_repo::get_repo_url(ssds_json::json_create &json)
  {
    GError * err = nullptr;
    GSList * list = lr_yum_repoconfs_get_list(repoHandler, &err);
    
    for(unsigned int i=0; i<g_slist_length(list); i++){
      char** url;
      char* name;
      short type = 0;
      void * val;
      
      LrYumRepoConf * conf = (LrYumRepoConf*)g_slist_nth_data(list, i);
      
      lr_yum_repoconf_getinfo(conf, &err, LR_YRC_ENABLED, &val);
      
      if(((long int)val)){
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_ID, &val) != false){
	  name = (char*)val;
	}
	
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_MIRRORLIST, &val) != false){
	  url[0] = (char*)val;
	  type = ssds_json::url_type::SSDS_MIRRORLIST;
	}
	
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_METALINK, &val) != false){
	  url[0] = (char*)val;
	  type = ssds_json::url_type::SSDS_METALINK;
	}
	
	err = nullptr;
	//void ** val2;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_BASEURL, &val) != false){
	  url = (char**)val; 
	  type = ssds_json::url_type::SSDS_BASEURL;
	}
	//std::cout << "name: " << name << "\nurl: " << url << std::endl;
	
	
	LrUrlVars *list =  lr_urlvars_set(NULL, "releasever", "21"); //"3.18.6-200.fc21"
	list = lr_urlvars_set(list, "basearch", "x86_64");
          
	int k=0;
          while(url[k]!=nullptr)
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
          url_subst_list[k]=nullptr;
	
          json.add_repo(url_subst_list, name, type, k);
	
          for(int j=0; j<=k; j++)
            free(url_subst_list[j]);
          
          free(url_subst_list);
      }
    }
  }
 
  /*
   * repoHandler is C structure and needs to be properly freed
   */
  void parse_repo::free_resources()
  {
    lr_yum_repoconfs_free(this->repoHandler);
    repolist.erase(repolist.begin(), repolist.end());
  }


}