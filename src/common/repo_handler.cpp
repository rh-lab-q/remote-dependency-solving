#include "xml_handler.hpp"
#include "repo_handler.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include <boost/program_options.hpp>


//LIBREPO
#include <librepo/repoconf.h>

/*
 * TODO - c++ neco co parsuje .repo soubory v yum?? zkusit jestli neni neco pro c++
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
  void parse_repo::get_repo_url(ssds_xml::create_xml& xml)
  {
    //object representing xml that will be sent to the server
    if(xml.find_node_by_path((xmlChar* )"//data/repolist") == nullptr){		//items will be added into this node
      xml.add_child(xml.dataNodePtr, (xmlChar*) "repolist", (xmlChar*) "");	//if it is not there I create it
      xml.currNodePtr = xml.addedNodePtr;//addedNodePtr might be needed later co I use currNodePtr instead
    }
    
    
    GError * err = nullptr;
    GSList * list = lr_yum_repoconfs_get_list(repoHandler, &err);
    
    for(unsigned int i=0; i<g_slist_length(list); i++){
      std::string url;
      std::string name;
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
	}
	
	err = nullptr;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_METALINK, &val) != false){
	  url = (char*)val;
	}
	
	err = nullptr;
	//void ** val2;
	if(lr_yum_repoconf_getinfo(conf, &err, LR_YRC_BASEURL, &val) != false){
	  url = ((char**)val)[0];
	}
	//std::cout << "name: " << name << "\nurl: " << url << std::endl;
	
	xmlChar* doc_str = xmlEncodeEntitiesReentrant(xml.document, (xmlChar*) url.c_str());
	
	xml.add_child(xml.currNodePtr, (xmlChar*) "repo", doc_str);
	xmlFree(doc_str);

	xml.add_attr((xmlChar*) "name", (xmlChar*) name.c_str());
      }
    }
    
    /*
    while(nextPtr != nullptr){
      
      std::cout << "while list" << std::endl;
      std::string url;
      std::string name;
      */
      
#if 0
      //std::cout << "repo name: " << ((LrYumRepoConf*)nextPtr->data)->name << std::endl;
      //std::cout << std::endl;
      void *ptr = nullptr;
      //std::cout << "repo debug: " << ((LrYumRepoConf*)nextPtr->data)->name << std::endl;
      lr_yum_repoconf_getinfo()
      gboolean ret = lr_yum_repoconf_getinfo((LrYumRepoConf*)nextPtr->data, &err, LR_YRC_NAME, &ptr);
      
      if(((LrYumRepoConf*)nextPtr->data)->enabled == true){//chyba chyba chyba
	std::cout << "repo enabled" << std::endl;
	name = ((LrYumRepoConf*)nextPtr->data)->name;
	
	if(((LrYumRepoConf*)nextPtr->data)->baseurl != nullptr)
	  url = ((LrYumRepoConf*)nextPtr->data)->baseurl[0];
	else if(((LrYumRepoConf*)nextPtr->data)->metalink != nullptr)
	  url = ((LrYumRepoConf*)nextPtr->data)->metalink;
	else if(((LrYumRepoConf*)nextPtr->data)->mirrorlist != nullptr)
	  url = ((LrYumRepoConf*)nextPtr->data)->mirrorlist;
	
	
	xmlChar* doc_str = xmlEncodeEntitiesReentrant(xml.document, (xmlChar*) url.c_str());
	  
	xml.add_child(xml.currNodePtr, (xmlChar*) "repo", doc_str);
	xmlFree(doc_str);

	xml.add_attr((xmlChar*) "name", (xmlChar*) name.c_str());
      }
#endif
      //nextPtr = nextPtr->next;
    //}
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