#include "xml_handler.hpp"
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
      std::string url;
      std::string name;
      short type = 0;
      void * val;
      
      LrYumRepoConf * conf = (LrYumRepoConf*)g_slist_nth_data(list, i);
      
      lr_yum_repoconf_getinfo(conf, &err, LR_YRC_ENABLED, &val);
      
      if(((long int)val)){
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_NAME, &val) != false){
	  name = (char*)val;
	}
	
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_MIRRORLIST, &val) != false){
	  url = (char*)val;
	  type = ssds_xml::url_type::SSDS_MIRRORLIST;
	}
	
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_METALINK, &val) != false){
	  url = (char*)val;
	  type = ssds_xml::url_type::SSDS_METALINK;
	}
	
	err = nullptr;
	//void ** val2;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_BASEURL, &val) != false){
	  url = ((char**)val)[0];
	  type = ssds_xml::url_type::SSDS_BASEURL;
	}
	//std::cout << "name: " << name << "\nurl: " << url << std::endl;
	
	
	LrUrlVars *list =  lr_urlvars_set(NULL, "releasever", "21"); //"3.18.6-200.fc21"
	list = lr_urlvars_set(list, "basearch", "x86_64");
	char *url_subst = lr_url_substitute(url.c_str(), list);
	
	switch(type){
	  case ssds_xml::url_type::SSDS_BASEURL: json.add_repo((char*)url_subst, (char*)name.c_str(), 1);
						 break;
	  case ssds_xml::url_type::SSDS_MIRRORLIST: json.add_repo((char*)url_subst, (char*)name.c_str(), 2);
						    break;
	  case ssds_xml::url_type::SSDS_METALINK: json.add_repo((char*)url_subst, (char*)name.c_str(), 3);
						  break;
	}
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