#ifndef _XML_HANDLER_H
#define _XML_HANDLER_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include <iostream>
#include <string>
#include <vector>

/*
 * 
 * TODO - 
 * 
 */


namespace ssds_xml
{
	/*
	 * Class for debugging all xml documents
	 * !!!!Important!!!!! Functions in this class are not optimized, they are used only for debugging and they shouldn't be used in running app
	 */
	class xml_debug{
	public:
		xml_debug()
		{
			
		}
	
		/*
		 * Prints whole xml from the xmlNode given as an argument. The output is structured for better 
		 */
		void flush_xml(xmlNode * a_node, int indent)
		{
			xmlNode *curr_node = NULL;
			std::string tab = "";
							
			for(int i=0; i<indent; i++) {//indentation
				tab += "\t";
			}
			
			curr_node = a_node;
			
			if(curr_node->type == 1){
				std::cout << tab << "<" << curr_node->name;
				if(curr_node->properties == nullptr)//without any attributes - the tag can be closed
					std::cout << ">" << std::endl;
				else{//attributes out
					xmlAttrPtr attribute = curr_node->properties;
					while(attribute!=nullptr){
						std::cout << " " << attribute->name << "=\"";//name of attribute
						std::cout << attribute->children->content << "\"";//value of the attribute
										
						attribute = attribute->next;
					}
					std::cout << ">" << std::endl;//end bracket	
				}
			}
			
			else if(curr_node->type == 3){
				if(!xmlIsBlankNode(curr_node))
					std::cout<< tab<<curr_node->content<<std::endl;
			}
				
			if(curr_node->type == 1 && curr_node->children != nullptr)
				flush_xml(curr_node->children, indent+1);
			//for(curr_node)
			
			if(curr_node->type == 1)
				std::cout << tab << "</" << curr_node->name << ">" << std::endl;
			
			if(curr_node->next != nullptr)
				flush_xml(curr_node->next, indent);
		}
	};
	
	
	
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
	
	/*
	 * Class for parsing XML document
	 */
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
				
				//ssds_xml::xml_debug debug;
				//debug.flush_xml_new(rootNodePtr, 0);

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
			 * Function finds all nodes with specified name and returns its contents in a structure reachable through ret_vector_ptr
			 * Structure referenced by ret_vector_ptr needs to be dynamically allocated before calling this func
			 * After calling this func, it is very simple to go through the structure:
			 * 
			 * for(std::vector<ssds_xml::xml_node*>::iterator it = result->begin(); it != result->end(); it++){//result is of the same type as ret_vector_ptr
			 *		std::cout << (*it)->value << std::endl; //this prints the value of the node i.e. any text between <tag> and </tag>
			 *		
			 *		for(std::vector<ssds_xml::xml_attr*>::iterator itt = (*it)->attributes.begin(); itt != (*it)->attributes.end(); itt++){
			 *			std::cout << "\tattr_name: " << (*itt)->name << ", attr_value:" << (*itt)->value << std::endl;//this prints attributes values and names
			 *		}
			 *	}
			 */
			void get_node_by_path(xmlChar* path, std::vector<xml_node*>* ret_vector_ptr)
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
			
			/*
			 * Function for adding new tags - but for creating new XML from scraps, use create_xml class below
			 */
			xmlNodePtr add_node_by_path(xmlChar* xpath)
			{
				xmlXPathContextPtr context;
				xmlXPathObjectPtr result;
							
				context = xmlXPathNewContext(document);
				result = xmlXPathEvalExpression(xpath, context);
								
				if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
					xmlXPathFreeObject(result);
					std::cout << "nenaslo" << std::endl;
					return nullptr;
				}
							
				if(result->nodesetval->nodeNr > 1){
					std::cout << "ambiguous xpath expression" << std::endl;
					return nullptr;
				}
									
									
				std::cout << "probehlo" << std::endl;
							
				//std::cout << result->nodesetval->nodeTab[0]->name << std::endl;
									
									
									
			}
			
			
			
			
			
		public:
			xmlDocPtr document;
			xmlNodePtr currNodePtr;
			xmlNodePtr rootNodePtr;
			xmlNodePtr dataNodePtr;

	};
	
	
	
	
	/*
	 * Class for creating new xml documents from blank page
	 */
	class create_xml
	{
		
	public:
		create_xml()
		{			
			std::string base = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ssds><code></code><data></data></ssds>";//basic structure
			document = xmlParseMemory(base.c_str(), base.size());
			
			rootNodePtr = xmlDocGetRootElement(document);
			
			find_node_by_path((xmlChar*) "//data");
			dataNodePtr = currNodePtr;
		}
		
		/*
		 * Adds status code into the xml
		 */
		void add_code(xmlChar* code)
		{
			currNodePtr = rootNodePtr->xmlChildrenNode;
							
			while(currNodePtr != NULL)
			{
				if((!xmlStrcmp(currNodePtr->name, (const xmlChar *)"code")))
				break;
						
				currNodePtr = currNodePtr->next;
			}
			
			xmlNodeSetContent(currNodePtr, code);
		}
		
		/*
		 * Either returns nullptr if the node isn't there or currNodePtr points to node that was found
		 */
		xmlNodePtr find_node_by_path(xmlChar* xpath)
		{
			xmlXPathContextPtr context;
			xmlXPathObjectPtr result;
													
			context = xmlXPathNewContext(document);
			result = xmlXPathEvalExpression(xpath, context);
											
			if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
				xmlXPathFreeObject(result);
				return nullptr;
			}
			else{
				currNodePtr = result->nodesetval->nodeTab[0];
			}
		}
		
		/*
		 * Adds new child element, currNodePtr will point to this new node
		 */
		void add_child(xmlNodePtr node, xmlChar* name, xmlChar* content)
		{
			addedNodePtr = xmlNewChild(node, nullptr, name, content);
		}
		
		/*
		 * Adds attribute pointed to by currNodePtr 
		 */
		void add_attr(xmlChar* name, xmlChar* value)
		{
			xmlNewProp(addedNodePtr, name, value);
		}
		
		/*
		 * Adds node next to currNode
		 */
		void add_sibling()
		{
			
		}
		
	public:
		xmlTextWriterPtr writer;
		xmlDocPtr document;//whole xml document
		xmlNodePtr rootNodePtr;//root node of xml document mainly for flush_xml
		xmlNodePtr currNodePtr;//current working node, holds a pointer to the first node returned by find_node_by_path
		xmlNodePtr dataNodePtr;//data node for adding new children
		xmlNodePtr addedNodePtr;//pointer to node that was just added by add_child function - use for add attributes to new node (function add_attr)
	};
	
	

}



#endif
