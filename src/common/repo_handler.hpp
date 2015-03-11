#ifndef _REPO_HANDLER_HPP
#define _REPO_HANDLER_HPP
#include <string>
#include <string.h>
#include <librepo/repoconf.h>
#include <librepo/librepo.h>
#include <vector>
#include <iostream>
#include <glib-2.0/glib/gerror.h>
#include <glib-2.0/glib/gslist.h>
#include <stdio.h>
#include "../common/xml_handler.hpp"

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>

namespace ssds_repo{
  class parse_repo{
  public:
    parse_repo();
    void parse_default_repo();
    void get_repo_url(ssds_xml::create_xml& xml);
    void free_resources();
    
  public:
    std::vector<std::ifstream*> repolist;
    LrYumRepoConfs * repoHandler;
    LrYumRepoConf * singleRepo;
    
  };
  
  class repo_metadata{
  public:
    repo_metadata();
    void by_url(ssds_xml::xml_node* repo_node, std::vector<LrYumRepo*> &repo_info);
    
    std::vector<ssds_xml::xml_node*>* urls;
    
  private:
    
  };
}

#endif