#include "xml_handler.hpp"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include <iostream>
#include <string>
#include <vector>

namespace ssds_xml{
  create_xml::create_xml()
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
  void create_xml::add_code(xmlChar* code)
  {
    currNodePtr = rootNodePtr->xmlChildrenNode;

    while(currNodePtr != NULL){
      if((!xmlStrcmp(currNodePtr->name, (const xmlChar *)"code")))
      break;

      currNodePtr = currNodePtr->next;
    }

    xmlNodeSetContent(currNodePtr, code);
  }

  /*
  * Either returns nullptr if the node isn't there or currNodePtr points to node that was found
  */
  xmlNodePtr create_xml::find_node_by_path(xmlChar* xpath)
  {
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(document);
    result = xmlXPathEvalExpression(xpath, context);

    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
      xmlXPathFreeObject(result);
      xmlXPathFreeContext(context);
      return nullptr;
    }
    else{
      currNodePtr = result->nodesetval->nodeTab[0];
    }
    
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
 
    return currNodePtr;
  }

  /*
  * Adds new child element, currNodePtr will point to this new node
  */
  void create_xml::add_child(xmlNodePtr node, xmlChar* name, xmlChar* content)
  {
    addedNodePtr = xmlNewChild(node, nullptr, name, content);
  }

  /*
  * Adds attribute pointed to by currNodePtr 
  */
  void create_xml::add_attr(xmlChar* name, xmlChar* value)
  {
    xmlNewProp(addedNodePtr, name, value);
  }

  /*
  * Adds node next to currNode
  */
  void create_xml::add_sibling()
  {

  }
  
  void create_xml::doc_to_str()
  {
    xmlChar* buff;
    int buffsize;
    
    xmlDocDumpFormatMemory(document, &buff, &buffsize, 1);
    str_output = (char* )buff;
    
    free(buff);
    
    //std::cout << buff << std::endl;
    
  }
  
  void create_xml::free_resources()
  {
    xmlFreeDoc(document);
  }
}
