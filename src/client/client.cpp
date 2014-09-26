#include <iostream>
//needs to be compiled with -lboost_system option
#include "client.hpp"
#include "../common/logger.hpp"
#include "../common/xml_handler.hpp"
#include <string>
#include <exception>
#include <fstream>

using namespace boost::asio;

int main(int argc, const char* argv[]){
	//pouzit libxml
	ssds_client::client client;
	logger::log my_log;

	ssds_xml::read_xml xml_object;
	xml_object.read_xml_from_file("test.xml");
	std::cout << xml_object.get_code() << std::endl;
		
		
	xml_object.find_node("//repolist/repo");
	for(xmlpp::NodeSet::iterator iter=xml_object.currNodes.begin(); iter!=xml_object.currNodes.end(); iter++)
	{
		std::cout << xml_object.get_node_attr((*iter));
		std::cout << " -> " << xml_object.get_node_content((*iter)) << std::endl;
	}
		
	std::cout << "\n\n";

	ssds_xml::create_xml xml_obj_cr;
	xml_obj_cr.insert_code(213);
	xml_obj_cr.print_xml();

	
	//resolver for dns query
	ip::tcp::resolver resolver(client.io_service_object);
	ip::tcp::resolver::query query("localhost", "40002");
	
	//a list of responses is returned
	ip::tcp::resolver::iterator iter = resolver.resolve(query);
	ip::tcp::resolver::iterator end;
	ip::tcp::endpoint endpoint;
	
	//try to connect to any of the returned endpoints
	while(iter != end)
	{
		endpoint = *iter++;
		std::cout << endpoint << std::endl;
	}


	ip::tcp::socket my_socket(client.io_service_object);
	my_socket.connect(endpoint);

	//openning a connection through the created socket
	//connect(my_socket, resolver.resolve(query));
	//read the answer from server
	for (;;) {
		boost::array<char, 128> buf;
		boost::system::error_code error;

		size_t len = my_socket.read_some(buffer(buf), error);

		if(error==error::eof)
			break;
		else if (error)
			throw boost::system::system_error(error);

		std::cout.write(buf.data(), len);
	}	
	//my_log.add_log(logINFO) << "message from client" << std::endl;
	//log.add_log("This message is sent to client logger");

	return 0;
}
