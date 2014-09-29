#ifndef XML_HANDLER_H
#define XML_HANDLER_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <iostream>
#include <string>

namespace ssds_xml 
{
	class read_xml 
	{
		public:
			/*
			 * For searching through the dataNode use this syntax:
			 * xml_onject.find_node(//path/to/xml/node);
			 * 
			 * For going through the NodeSet we created earlier:
			 * for(xmlpp::NodeSet::iterator iter=xml_object.currNodes.begin(); iter!=xml_object.currNodes.end(); iter++)
			 * 
			 * TODO - kontrola parsovani zda dobre probehlo
			 * 		- kontrola root node
			 */
			read_xml()
			{
			}
			
			/*
			 * Parse xml document from file on given path
			 */
			void parse_xml_file(std::string path)
			{
				document = xmlParseFile(path.c_str());
				
				if(document == NULL)
					std::cout << "neco se podelalo" << std::endl;
				
				rootNodePtr = xmlDocGetRootElement(document);
				currNodePtr = rootNodePtr;

				while(currNodePtr != NULL)
				{
					if((!xmlStrcmp(currNodePtr->name, (const xmlChar *)"data")))
						break;
					
					currNodePtr = currNodePtr->next;
				}
				
				dataNodePtr = currNodePtr;
			}
			
			/*
			 * Returns int value of the <code> tag
			 */
			int get_code()
			{
				currNodePtr = rootNodePtr->xmlChildrenNode;
				
				while(currNodePtr != NULL)
				{
					if((!xmlStrcmp(currNodePtr->name, (const xmlChar *)"code")))
						break;
					
					currNodePtr = currNodePtr->next;
				}

				xmlChar *content;
				content = xmlNodeListGetString(document, currNodePtr->xmlChildrenNode, 1);
				
				int ret = atoi((char* )content);
				free(content);
				return ret;
			}
			
			/*
			 * 
			 */
			int find_node_by_path(xmlChar* path)
			{
				xmlXPathContextPtr context;
				xmlXPathObjectPtr result;
				
				context = xmlXPathNewContext(document);
				result = xmlXPathEvalExpression(path, context);
				
				if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
					xmlXPathFreeObject(result);
					return 0;
				}
				
				xmlChar* keyword;
				
				for (int i=0; i < result->nodesetval->nodeNr; i++) {
					keyword = xmlNodeListGetString(document, result->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
					printf("keyword: %s\n", keyword);
				    xmlFree(keyword);
				}
				
				
			}
			
		public:
			xmlDocPtr document;
			xmlNodePtr currNodePtr;
			xmlNodePtr rootNodePtr;
			xmlNodePtr dataNodePtr;

	};
	
	
	class create_xml
	{
		
	public:
		create_xml()
		{
		}	
	};

}



#endif