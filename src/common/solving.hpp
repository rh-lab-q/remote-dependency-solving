#ifndef _SOLVING_HPP
#define _SOLVING_HPP

//SSDS
#include "xml_handler.hpp"
#include "repo_handler.hpp"

//STANDARD
#include <string>
#include <iostream>

//LIBXML
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>

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
#include <hawkey/package.h>

//LIBREPO
#include <librepo/yum.h>
#include <librepo/handle.h>

//SSDS
#include "xml_handler.hpp"
#include "repo_handler.hpp"

namespace ssds_solving {

	class solve{ 
    	    public:
		
		solve(/* repos class instance with all enabled repos handlers */); 
                    ~solve();
		void fill_sack(ssds_repo::repo_metadata &metadata);
		std::string query(const char* request);
	    	bool parseMessage(std::string message);
		void getRequest(xmlDocPtr xml, std::vector<std::string> &request, std::vector<std::string> &repos, int64_t &countRequest, int64_t &countRepos);
		std::string answer(std::string message);
		
		//SOLVING
		HySack sack;
		
		//XML
		xmlDocPtr xml_document;

	    private:
		/* repos class instance with all enabled repos handlers */
	};

}//ssds_ solving
#endif
