#include "xml_handler.hpp"
#include "repo_handler.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include <boost/program_options.hpp>
//#include <librepo/librepo.h

/*
 * TODO - c++ neco co parsuje .repo soubory v yum?? zkusit jestli neni neco pro c++
 * 
 * 
 * 
 */

namespace ssds_repo{
  parse_repo::parse_repo()
  {
    DIR *directory;
    struct dirent *entry;

    directory = opendir("/etc/yum.repos.d/");

    if (directory == NULL) {
      perror("opendir");
    }



    while((entry = readdir(directory))){//vector repolist is filled with file descriptors
      if(strcmp(entry->d_name,".") !=0 && strcmp(entry->d_name,"..") !=0){//need to get rid of . and ..
	std::ifstream* file = new std::ifstream;
	std::string path = "/etc/yum.repos.d/"+(std::string)entry->d_name;
	file->open(path);
	repolist.push_back(file);
      }
    }
    
    free(entry);

  }


  /*
  * Parses .repo files, creates nodes in xml representing repo name and url (metalink | mirrorlist)
  */
  void parse_repo::get_repo_url(ssds_xml::create_xml& xml)
  {	
    if(xml.find_node_by_path((xmlChar* )"//data/repolist") == nullptr){//items will be added into this node
      xml.add_child(xml.dataNodePtr, (xmlChar*) "repolist", (xmlChar*) "");//if it is not there I create it
      xml.currNodePtr = xml.addedNodePtr;//addedNodePtr might be needed later co I use currNodePtr instead
    }
    
    


    for(std::vector<std::ifstream*>::iterator it = repolist.begin(); it != repolist.end(); it++){
      std::string line;
      std::string url;
      std::string name;
      int enabled = 0;

      while(std::getline(**it, line)){
	if(line.find("name")!=std::string::npos)
	  name = line.substr(5);//only text after name= will be needed

	if(line.find("baseurl")!=std::string::npos && line.find("#baseurl")==std::string::npos)
	  url = line.substr(8);//only text after baseurl= will be needed

	if(line.find("metalink")!=std::string::npos && line.find("#metalink")==std::string::npos)
	  url = line.substr(9);//only text after metalink= will be needed

	if(line.find("mirrorlist")!=std::string::npos && line.find("#mirrorlist")==std::string::npos)
	  url = line.substr(11);//only text after metalink= will be needed

	if(line.find("enabled=1")!=std::string::npos && line.find("#enabled")==std::string::npos)
	  enabled = 1;

	if(strcmp(line.c_str(), "") == 0 && enabled == 1){
	  size_t barch = url.find("$basearch");

	  if(barch != std::string::npos)
	  url.replace(barch, barch+9, "x86_64");

	  xmlChar* doc_str = xmlEncodeEntitiesReentrant(xml.document, (xmlChar*) url.c_str());
	  
	  xml.add_child(xml.currNodePtr, (xmlChar*) "repo", doc_str);
	  xmlFree(doc_str);

	  xml.add_attr((xmlChar*) "name", (xmlChar*) name.c_str());
	  enabled = 0;
	}
      }
    }
  }
  
  void parse_repo::free_resources()
  {
    repolist.erase(repolist.begin(), repolist.end());
  }

}