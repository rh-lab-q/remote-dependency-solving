//============================================================================
// Name		: Solving.cpp
// Author	: Jozkar
// Editor	: mruprich
// Version	: 0.1
// Copyright	: GNU GPL
// Description	: Solving class at server side of SSDS
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/

#include "solving.hpp"
//#include "../../../hawkey/hawkey/src/package_internal.h"


namespace ssds_solving {
  
  //solve object loads all packages installed in current system
  //NOTE!!! - right now, everything is only static to create first working example. Later it needs to be redone
  solve::solve(/* repos class instance */){
    //repo_info = new std::vector<LrYumRepo*>();
    
    
    /* Creating sack */
    this->sack = hy_sack_create(NULL, NULL, NULL,HY_MAKE_CACHE_DIR);
    
    if(hy_sack_load_system_repo(sack, NULL, HY_BUILD_CACHE) == 0)
      std::cout << "load_system_repo v cajku, kontrolni pocet: " << hy_sack_count(sack) << std::endl;
    
    /* Loading repo metadata into sack */
//     HyRepo repo = hy_repo_create("pokus");
//     hy_repo_set_string(repo, HY_REPO_MD_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/repomd.xml");
//     hy_repo_set_string(repo, HY_REPO_PRIMARY_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/e2a28baab2ea4632fad93f9f28144cda3458190888fdf7f2acc9bc289f397e96-primary.xml.gz");
//     hy_repo_set_string(repo, HY_REPO_FILELISTS_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/abb4ea5ccb9ad46253984126c6bdc86868442a4662dbcfa0e0f51b1bb209331e-filelists.xml.gz");
//     
//     if(hy_sack_load_yum_repo(sack, repo, 0) == 0)
//       std::cout << "load_yum_repo v cajku, kontrolni pocet: " << hy_sack_count(sack) << std::endl;
        
  }

  solve::~solve() {

  }
  
  void solve::fill_sack(ssds_repo::repo_metadata &metadata)
  {
    for(ssds_repo::metadata_files_location* loc : metadata.files_locations)
    {
      HyRepo repo = hy_repo_create("pokus");
      std::string repomd_path = loc->repomd + "/repodata/repomd.xml";
//       std::cout << "pokus ve fill sack, filelists: " << loc->filelists << "\nprimary: " << loc->primary << "\nrepomd: " << loc->repomd << std::endl;
      hy_repo_set_string(repo, HY_REPO_MD_FN, repomd_path.c_str());
      hy_repo_set_string(repo, HY_REPO_PRIMARY_FN, loc->primary.c_str());
      hy_repo_set_string(repo, HY_REPO_FILELISTS_FN, loc->filelists.c_str());
        
      hy_sack_load_yum_repo(sack, repo, 0);
    }
    std::cout << "load_yum_repo v cajku, kontrolni pocet: " << hy_sack_count(sack) << std::endl;
  }

  
  std::string solve::query(const char* request){
    /* QUERY */
    std::cout << "query - na co se ptam?: " << request<< std::endl;
    
    HyQuery query = hy_query_create(this->sack);
    hy_query_filter(query, HY_PKG_NAME, HY_SUBSTR, request);
    hy_query_filter(query, HY_PKG_REPONAME, HY_NEQ, HY_SYSTEM_REPO_NAME);
    hy_query_filter_latest_per_arch(query, 1);
    
    /* Getting list of packages from the query */
    HyPackageList plist = hy_packagelist_create();
    plist = hy_query_run(query);
    
    std::cout << "pocet baliku nalezenych pomoci query: " << hy_packagelist_count(plist) << std::endl;
    
    HyPackage pkg;
    pkg = hy_packagelist_get(plist, 0);
    
    /* GOAL */
    HyGoal goal = hy_goal_create(sack);
    hy_goal_install(goal, pkg);
    if(hy_goal_run(goal)==0)
      std::cout << "Dependencies solving true = dependence v poradku" << std::endl;
    
    std::string answer = hy_package_get_name(pkg) + (std::string)"-" + hy_package_get_version(pkg) + (std::string)"-" + hy_package_get_release(pkg) + (std::string)"-" + hy_package_get_arch(pkg);
    return answer;
  }
  
  
  
  // parsing message
  bool solve::parseMessage(std::string message){
	this->xml_document = xmlParseMemory(message.c_str(), message.size());
	return true;
  }

  // findout, what user has and what user wants
  void solve::getRequest(xmlDocPtr xml, std::vector<std::string> &request, std::vector<std::string> &repos, int64_t &countRequest, int64_t &countRepos){

    // init
    countRequest = 0;
    countRepos = 0;

    // parsing
    // todo controll if xml contains both path
    // while parsing repo nodes, wwith content of nodes has to be parsed even attr of nodes
    xmlXPathContextPtr context;
    xmlXPathObjectPtr requestResult, repoResult;
    xmlNodePtr requestNode, repoNode;
    xmlChar *requestPath = (xmlChar *)"/ssds/data/req_pkgs",
            *repoPath = (xmlChar *)"/ssds/data/repolist";

    context = xmlXPathNewContext(xml);
    requestResult = xmlXPathEvalExpression(requestPath, context);
    repoResult = xmlXPathEvalExpression(repoPath, context);

    requestNode = requestResult->nodesetval->nodeTab[0];
    repoNode = repoResult->nodesetval->nodeTab[0];
        
    xmlXPathFreeObject(requestResult);
    xmlXPathFreeObject(repoResult);
    xmlXPathFreeContext(context);
    
    xmlChar *ret;
    requestNode = requestNode->xmlChildrenNode;
    while(requestNode != NULL){

      if((!xmlStrcmp(requestNode->name, (const xmlChar*)"pkg"))){
        ret = xmlNodeListGetString(xml,requestNode->xmlChildrenNode,1);
        std::string xxx = (const char*) ret;
        request.push_back(xxx);
        countRequest++;			
        xmlFree(ret);
      }
      requestNode = requestNode->next;
    }

    if(repoNode != NULL){
      repoNode = repoNode->xmlChildrenNode;
    }
    
    while(repoNode != NULL){

      if((!xmlStrcmp(repoNode->name, (const xmlChar*)"repo"))){
        ret = xmlNodeListGetString(xml,repoNode->xmlChildrenNode,1);
        std::string xxx = (const char*) ret;
        repos.push_back(xxx);
        countRepos++;
        xmlFree(ret);
      }
      repoNode = repoNode->next;
    }

    // control print
    xmlChar* buff;
    int buffsize;

    xmlDocDumpFormatMemory(xml, &buff, &buffsize, 1);
    std::cout << "str_output: " << std::endl;
    std::string str_output = (char* )buff;

    free(buff);
    // end control print
    std::cout << "Control print (result of parsing)" << std::endl << str_output << std::endl;
        
  }

  // result of SSDS
  std::string solve::answer(ssds_xml::read_xml &client_data){
    std::string ret_msg = "";
    std::vector<ssds_xml::xml_node*> requested_pkgs;
    
    client_data.get_node_by_path((xmlChar* )"//data/req_pkgs/pkg", requested_pkgs);
    
    for(ssds_xml::xml_node* pkg : requested_pkgs)
    {
      ret_msg += query(pkg->value.c_str());
      ret_msg += "\n";
    }
    
    // local variables (int64_t used for multiplaform using)
//     int64_t countRequest = 0, countRepos = 0;	
//     unsigned int i;
//     std::vector<std::string> request;
//     std::vector<std::string> repos;
// 
//     // convert income string to xml document	
//     //xmlDoc *xml = parseMessage(message);
//     getRequest(this->xml_document, request, repos, countRequest, countRepos);
// 
//     // controll prints
//     std::cout << "Control print (references)" << std::endl << countRequest << " " << countRepos << std::endl;
//     std::string ret_msg;
// 
//     for(i=0; i<request.size();i++){
//       std::cout << request[i] << std::endl;
//       ret_msg += query(request[i].c_str());
//       ret_msg += "; ";
//     }
// 
//     for(i=0; i<repos.size();i++){
//               std::cout << repos[i] << std::endl;
//     }
//     
//     ret_msg = "Connection accepted. Answer: "+ret_msg+"\n";
    ret_msg += "\n";
    return ret_msg;
  }
}
