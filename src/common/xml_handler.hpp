//#ifndef XML_HANDLER
//#define XML_HANDLER

/*
 * !!!! TODO !!!!
 * <repolist>
 * 	<repo name="repo_name">
 * 		<baseurl> || <metalink>
 * 		<gpgkey> && <gpgcheck>
 * 	</repo>
 * </repolist>
 * 
 * 
 * 
 * 
 * 
 */


#include <libxml++/libxml++.h>
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
			 * 
			 */
			read_xml()
			{
			}

			/*
			 * Parse XML file into parser object
			 */
			int read_xml_from_file(const std::string path)
			{
				try
				{
					parser.parse_file(path);
					rootNode = parser.get_document()->get_root_node();
					
					xmlpp::NodeSet node(rootNode->find("//data"));
					dataNode = *node.begin();

					//just testing usage - printing name of root node
					//const Glib::ustring nodename = rootNode->get_name();
					//std::cout << "root name : " << nodename << std::endl;
				}
				catch(const std::exception& ex)
				{
				    std::cerr << "Exception caught: " << ex.what() << std::endl;
				    return 0;
				}			
				
				return 1;
			}
			
			/*
			 * Parse XML in a string into parser object
			 */
			int read_xml_from_string(std::string xml)
			{
				try
				{
					parser.parse_memory(xml);
					rootNode = parser.get_document()->get_root_node();
					
					//just testing usage - printing name of root node
					//const Glib::ustring nodename = rootNode->get_name();
					//std::cout << "root name : " << nodename << std::endl;
				}
				catch(const std::exception& ex)
				{
				    std::cerr << "Exception caught: " << ex.what() << std::endl;
				    return 0;
				}
				
				return 1;
			}
			
			/*
			 * Prints whole parser object to stdout
			 */
			void print_xml()
			{
				parser.get_document()->write_to_stream(std::cout, "");
			}
			
			/*
			 * Extracts message code from XML
			 */
			int get_code()
			{
				xmlpp::NodeSet code_node(rootNode->find("//code"));
				
				const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*code_node.begin());
				const xmlpp::TextNode* text = nodeElement->get_child_text();
				
				//just convert text to int for return
				std::stringstream convert(text->get_content());
				int code;
				convert >> code;
				
				return code;
			}
			
			/*
			 * Allows to find nodes in the xml - if there are more than one it will point to the first one
			 * parameter name needs to be in xpath form - "//node/child/subchild..."
			 */
			int find_node(std::string name)
			{
				xmlpp::NodeSet nodeset(dataNode->find(name));
				currNodes = nodeset;
			}
			
			/*
			 * Function returns attribute of xml node as a string
			 * TODO - more than one attribute
			 */
			std::string get_node_attr(const xmlpp::Node* node)
			{
				const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node);
				const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
				const xmlpp::Attribute* attribute = *attributes.begin();
				std::string ret = attribute->get_name() + " : " + attribute->get_value();
				return ret;
			}
			
			/*
			 * Function return content of node as a string
			 */
			std::string get_node_content(const xmlpp::Node* node)
			{
				const xmlpp::Element* content = dynamic_cast<const xmlpp::Element*>(node);
				const xmlpp::TextNode* text = content->get_child_text();
				std::string ret = text->get_content();
				return ret;
			}
			
		private:
			const xmlpp::Node* rootNode;
			xmlpp::DomParser parser;
			
		public:
			const xmlpp::Node* dataNode;
			xmlpp::NodeSet currNodes;
	};
	
	class create_xml
	{
		
	public:
		create_xml()
		{
			rootNode = document.create_root_node("ssds");
			codeNode = rootNode->add_child("code");
			rootNode->add_child(codeNode, "data");
		}
		
		/*
		 * Simple function which inserts error code into the xml structure
		 */
		void insert_code(int code)
		{
			std::string code_str = std::to_string(code);
			codeNode->add_child_text(std::to_string(code));
		}
		
		/*
		 * For debug printing
		 */
		void print_xml()
		{
			document.write_to_stream(std::cout, "");
		}
		
		/*
		 * 
		 */
		
	
	public:
		xmlpp::Document document;
		xmlpp::DomParser parser;
	
	private:
		xmlpp::Element* rootNode;
		xmlpp::Element* codeNode;
		xmlpp::Element* dataNode;
		
	};

}
//#endif
