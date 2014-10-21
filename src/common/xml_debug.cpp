#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include <iostream>
#include <string>
#include <vector>
#include "xml_handler.hpp"

namespace ssds_xml {
  xml_debug::xml_debug(){}

  /*
  * Prints whole xml from the xmlNode given as an argument. The output is structured for better 
  */
  void xml_debug::flush_xml(xmlNode * a_node, int indent)
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
}