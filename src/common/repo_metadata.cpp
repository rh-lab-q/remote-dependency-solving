#include "repo_handler.hpp"

namespace ssds_repo {
  /**********************************************************************
   *	Class for geting repository metadata by given url
   **********************************************************************/ 
  repo_metadata::repo_metadata()
  {
    this->urls = new std::vector<ssds_xml::xml_node*>();
  }
  
  /*
   * 	Downloads repo metadata based on given url address
   */
  void repo_metadata::by_url(std::string url)
  {
    
  }
  
  
}