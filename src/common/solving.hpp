#ifndef SOLVING_HPP
#define SOLVING_HPP

#include <string>
#include <iostream>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include "xml_handler.hpp"

//#include <glib-2.0/glib/gerror.h>
//#include <glib-2.0/glib/gslist.h>
#include <glib.h>

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>

//LIBREPO
#include <librepo/yum.h>
#include <librepo/handle.h>

namespace ssds_solving {

	class solve{ 
    	    public:
		
		solve(/* repos class instance with all enabled repos handlers */); 
	        ~solve();
		void fillSack();
		std::string query(const char* request);
	    	bool parseMessage(std::string message);
		void getRequest(xmlDocPtr xml, std::vector<std::string> &request, std::vector<std::string> &repos, int64_t &countRequest, int64_t &countRepos);
		std::string answer(std::string message);
		
		//SOLVING
		HySack sack;
		
		//XML
		xmlDocPtr xml_document;
		
		//METADATA
		std::vector<LrResult*> repo_info;
	    private:
		/* repos class instance with all enabled repos handlers */
	};

}//ssds_ solving
#endif
