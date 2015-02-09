#ifndef SOLVING_HPP
#define SOLVING_HPP

#include <string>
#include <iostream>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include "xml_handler.hpp"

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
/*
#include "../../../hawkey/hawkey/src/types.h"
#include "../../../hawkey/hawkey/src/sack.h"
#include "../../../hawkey/hawkey/src/repo.h"
#include "../../../hawkey/hawkey/src/query.h"
#include "../../../hawkey/hawkey/src/query_internal.h"
#include "../../../hawkey/hawkey/src/packagelist.h"
#include "../../../hawkey/hawkey/src/goal.h"
#include "../../../hawkey/hawkey/src/goal_internal.h"*/

namespace ssds_solving {

	class solve{ 
    	    public:
		
		solve(/* repos class instance with all enabled repos handlers */); 
	        ~solve();
		std::string query(const char* request);
	    	xmlDocPtr parseMessage(std::string message);
		void getRequest(xmlDocPtr xml, std::vector<std::string> &request, std::vector<std::string> &repos, int64_t &countRequest, int64_t &countRepos);
		std::string answer(std::string message);
		
		//SOLVING
		HySack sack;
	    private:
		/* repos class instance with all enabled repos handlers */
	};

}//ssds_ solving
#endif
