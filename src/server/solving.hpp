#ifndef SOLVING_HPP
#define SOLVING_HPP

#include <string>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include "../common/xml_handler.hpp"

namespace ssds_solving {

	class solve{ 
    	    public:
		
		solve(/* repos class instance with all enabled repos handlers */); 
	        ~solve();
	    	xmlDocPtr parseMessage(std::string message);
		void getRequest(xmlDocPtr xml, std::vector<std::string> &request, std::vector<std::string> &repos, int64_t &countRequest, int64_t &countRepos);
		std::string answer(std::string message);
	    private:
		/* repos class instance with all enabled repos handlers */
	};

}//ssds_ solving
#endif
