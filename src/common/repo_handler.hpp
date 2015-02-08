#ifndef _REPO_HANDLER_HPP
#define _REPO_HANDLER_HPP
#include <string>
#include <librepo/repoconf.h>
//#include "../../../librepo/librepo/librepo/repoconf.h"

namespace ssds_repo{
  class parse_repo{
  public:
    parse_repo();
    void get_repo_url(ssds_xml::create_xml& xml);
    void free_resources();
    
  public:
    std::vector<std::ifstream*> repolist;
    LrYumRepoConfs * repoHandler;
    LrYumRepoConf * singleRepo;
    
  };
}

#endif