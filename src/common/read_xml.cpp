#include "xml_handler.hpp"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include <iostream>
#include <string>
#include <vector>

/*
* TODO - kontrola parsovani zda dobre probehlo
* 		- kontrola root node
*/
namespace ssds_xml {
  read_xml::read_xml(){}

  /*
  * Parse xml document from file on given path
  */
  void read_xml::parse_xml_file(std::string path)
  {
    document = xmlParseFile(path.c_str());

    if(document == NULL)
      std::cout << "Interni error - document v xml" << std::endl;

    rootNodePtr = xmlDocGetRootElement(document);
    currNodePtr = rootNodePtr;

    //ssds_xml::xml_debug debug;
    //debug.flush_xml_new(rootNodePtr, 0);

    while(currNodePtr != NULL){
      if((!xmlStrcmp(currNodePtr->name, (const xmlChar *)"data")))
      break;

      currNodePtr = currNodePtr->next;
    }

      dataNodePtr = currNodePtr;
  }
  
  /*
   * Parse xml from string
   */
  void read_xml::parse_xml_string(std::string message)
  {
    this->document = xmlParseMemory(message.c_str(), message.size());
  }

  /*
  * Returns int value of the <code> tag
  */
  int read_xml::get_code()
  {
    currNodePtr = rootNodePtr->xmlChildrenNode;

    while(currNodePtr != NULL){
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
  *for(std::vector<ssds_xml::xml_node*>::iterator it = result->begin(); it != result->end(); it++){//result is of the same type as ret_vector_ptr
  *	std::cout << (*it)->value << std::endl; //this prints the value of the node i.e. any text between <tag> and </tag>
  *
  *	for(std::vector<ssds_xml::xml_attr*>::iterator itt = (*it)->attributes.begin(); itt != (*it)->attributes.end(); itt++){
  *		std::cout << "\tattr_name: " << (*itt)->name << ", attr_value:" << (*itt)->value << std::endl;//this prints attributes values and names
  *	}
  *}
  */
  bool read_xml::get_node_by_path(xmlChar* path, std::vector<xml_node*>* ret_vector_ptr)
  {
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(document);
    result = xmlXPathEvalExpression(path, context);

    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
      xmlXPathFreeObject(result);
      return false;
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
    
    xmlXPathFreeObject(result);
    return true;
  }

  /*
  * Function for adding new tags - but for creating new XML from scraps, use create_xml class from creat_xml.cpp
  */
  xmlNodePtr read_xml::add_node_by_path(xmlChar* xpath)
  {
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(document);
    result = xmlXPathEvalExpression(xpath, context);

    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
      xmlXPathFreeObject(result);
      std::cout << "node not found" << std::endl;
      return nullptr;
    }

    if(result->nodesetval->nodeNr > 1){
      std::cout << "ambiguous xpath expression" << std::endl;
      return nullptr;
    }

    std::cout << "node created" << std::endl;
    return currNodePtr;
    //std::cout << result->nodesetval->nodeTab[0]->name << std::endl;
  }
  
  /*
   * All structures used for parsing XML need to be freed in the end
   */
  void read_xml::free_resources()
  {
    xmlFreeDoc(document);
  }
}
