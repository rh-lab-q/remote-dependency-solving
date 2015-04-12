#ifndef _REPO_HANDLER_HPP
#define _REPO_HANDLER_HPP

#include <stdio.h>
#include <string>
#include <string.h>
#include <librepo/repoconf.h>
#include <librepo/librepo.h>
#include <vector>
#include <iostream>
#include <glib-2.0/glib/gerror.h>
#include <glib-2.0/glib/gslist.h>

//SSDS
#include "xml_handler.hpp"
#include "json_handler.hpp"
//#include "solving.hpp"

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>

namespace ssds_repo{  
  class metadata_files_location{
  public:
    std::string repomd;
    std::string filelists;
    std::string primary;
  };
  
  class parse_repo{
  public:
    parse_repo();
    void parse_default_repo();
    void get_repo_url(ssds_json::json_create &json);
    void free_resources();
    
  public:
    std::vector<std::ifstream*> repolist;
    LrYumRepoConfs * repoHandler;
    LrYumRepoConf * singleRepo;
    
  };
  
  class repo_metadata{
  public:
    repo_metadata();
    int locate_repo_metadata_by_url();
    
    std::vector<ssds_xml::xml_node*> urls;
    std::vector<metadata_files_location*> files_locations;
    
  private:
    void download_repo_metadata_by_url(ssds_xml::xml_node &repo_node);
  };
}

#endif