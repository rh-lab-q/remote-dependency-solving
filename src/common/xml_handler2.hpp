#ifndef XML_HANDLER_H
#define XML_HANDLER_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <iostream>
#include <string>
#include <vector>

namespace ssds_xml
{
	//object representing xml attribute 
	class xml_attr{
		public:
			std::string name;
			std::string value;
	};	

	//object representing xml node
	class xml_node{
		public:
			std::string value;
			std::vector<xml_attr*> attributes;
	};
	

	class read_xml 
	{
		public:
			/*
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
			 *TODO - tady to udelam asi tak ze se vrati int a zustane ukazatel kam ma, zbytek se udela primo v main 
			 */
			void find_node_by_path(xmlChar* path, std::vector<xml_node*>* ret_vector_ptr)
			{
				xmlXPathContextPtr context;
				xmlXPathObjectPtr result;
				
				context = xmlXPathNewContext(document);
				result = xmlXPathEvalExpression(path, context);
				
				if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
					xmlXPathFreeObject(result);
					return;
				}
				
				//std::vector<xml_node*> ret_vector;
				xmlChar* keyword;
				xmlAttrPtr attribute;
				
				for (int i=0; i < result->nodesetval->nodeNr; i++) {
					keyword = xmlNodeListGetString(document, result->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
					attribute = result->nodesetval->nodeTab[i]->properties;
					
					xml_node* new_node = new xml_node;//create new_node, this one will be added at the end of return vector
					new_node->value=(char* )keyword;//value of the node extracted from xml will be added as a string
					
					while(attribute!=nullptr){//while for attributes extraction
						xml_attr* new_attr = new xml_attr;
						new_attr->value = (char* )xmlNodeListGetString(document, attribute->children, 1);//value of the attribute
						new_attr->name = (char* )attribute->name;//name of attribute
						
						new_node->attributes.push_back(new_attr);//addding the extracted attribute to the node
						attribute=attribute->next;//next attribute
					}
					
					ret_vector_ptr->push_back(new_node);//here I put the whole node with attributes into the vector
					xmlFree(keyword);
				}
				
				/*
				for(std::vector<xml_node*>::iterator it = ret_vector_ptr->begin(); it != ret_vector_ptr->end(); it++){
					std::cout << "value: " << (*it)->value << std::endl;
					
					for(std::vector<xml_attr*>::iterator itt = (*it)->attributes.begin(); itt != (*it)->attributes.end(); itt++){
						std::cout << "\tattr_name: " << (*itt)->name << ", attr_value:" << (*itt)->value << std::endl;
					}
				}*/
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
		
	public:
		xmlDocPtr document;
		xmlNodePtr rootNodePtr;
	};

}



#endif