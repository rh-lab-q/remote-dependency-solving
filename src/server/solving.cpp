//============================================================================
// Name		: Solving.cpp
// Author	: Jozkar
// Version	: 0.1
// Copyright	: GNU GPL
// Description	: Solving class at server side of SSDS
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/

#include "solving.hpp"
namespace ssds_solving {
  
  solve::solve(/* repos class instance */){
  }

  solve::~solve() {

  }
  
  // parsing message
 xmlDocPtr solve::parseMessage(std::string message){
	xmlDocPtr document = xmlParseMemory(message.c_str(), message.size());
	return document;
  }

  // findout, what user has and whot user wants
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
	std::cout << repoNode << std::endl;
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
	std::string str_output = (char* )buff;
    
	free(buff);
	// end control print
	std::cout << "Control print (result of parsing)" << std::endl << str_output << std::endl;
	
  }

  // result of SSDS
  std::string solve::answer(std::string message){
	// local variables (int64_t used for multiplaform using)
	int64_t countRequest = 0, countRepos = 0;	
	std::vector<std::string> request;
	std::vector<std::string> repos;

	// convert income string to xml document	
	xmlDoc *xml = parseMessage(message);
	getRequest(xml, request, repos, countRequest, countRepos);

	// controll prints
	std::cout << "Control print (references)" << std::endl << countRequest << " " << countRepos << std::endl;

	for(int i=0; i<request.size();i++){
		std::cout << request[i] << std::endl;
	}

	for(int i=0; i<repos.size();i++){
		std::cout << repos[i] << std::endl;
	}
	return "Connection accepted. In future, server will tell you what to do.\n";
  }
}
