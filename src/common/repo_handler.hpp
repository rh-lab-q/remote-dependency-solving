#ifndef _REPO_HANDLER_HPP
#define _REPO_HANDLER_HPP
#include <string>


namespace ssds_repo{
  class parse_repo{
  public:
    parse_repo();
    void get_repo_url(ssds_xml::create_xml& xml);
    
  public:
    std::vector<std::ifstream*> repolist;
    
  };
}

#endif