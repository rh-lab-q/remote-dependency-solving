#ifndef _XML_HANDLER_HPP
#define _XML_HANDLER_HPP
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlwriter.h>
#include <string>
#include <exception>
#include <fstream>
#include <vector>

namespace ssds_xml{
 enum url_type :short{
      SSDS_BASEURL = 1,
      SSDS_MIRRORLIST,
      SSDS_METALINK
 }; 
 
 class xml_attr{
  public:
    std::string name;
    std::string value;
 };
 
 class xml_node{
  public:
    std::string value;
    std::vector<xml_attr*> attributes;
 };
 
 class xml_debug{
 public:
   xml_debug();
   void flush_xml(xmlNode * a_node, int indent);
   int validate_xml(xmlDocPtr doc);
   void flush_url_vector(std::vector<xml_node*>* urls);
 };
 
 class read_xml{
 public:
   read_xml();
   void parse_xml_file(std::string path);
   void parse_xml_string(std::string message);
   int get_code();
   bool get_node_by_path(xmlChar* path, std::vector<xml_node*>* ret_vector_ptr);
   xmlNodePtr add_node_by_path(xmlChar* xpath);
   void free_resources();
   void flush_url_vector(std::vector<xml_node*>* urls);
   
   
   xmlDocPtr document;
   xmlNodePtr currNodePtr;
   xmlNodePtr rootNodePtr;
   xmlNodePtr dataNodePtr;

 };
 
  /*
  * Class for creating new xml documents from blank page
  */
 class create_xml{
 public:
   create_xml();
   void add_code(xmlChar* code);
   xmlNodePtr find_node_by_path(xmlChar* xpath);
   void add_child(xmlNodePtr node, xmlChar* name, xmlChar* content);
   void add_attr(xmlChar* name, xmlChar* value);
   void add_sibling();
   void doc_to_str();
   void free_resources();
   
   xmlTextWriterPtr writer;
   xmlDocPtr document;//whole xml document
   xmlNodePtr rootNodePtr;//root node of xml document mainly for flush_xml
   xmlNodePtr currNodePtr;//current working node, holds a pointer to the first node returned by find_node_by_path
   xmlNodePtr dataNodePtr;//data node for adding new children
   xmlNodePtr addedNodePtr;//pointer to node that was just added by add_child function - use for add attributes to new node (function add_attr)
   std::string str_output;
 };
}

#endif