#include <iostream>
//needs to be compiled with -lboost_system option
//#include "client.hpp"
#include "../common/logger.hpp"
#include "../common/xml_handler.hpp"
#include "../common/repo_handler.hpp"
#include <string>
#include <exception>
#include <fstream>

//using namespace boost::asio;

int main(int argc, const char* argv[]){
	//ssds_client::client client; //object for network handling
	logger::log my_log; //logger init
	ssds_xml::xml_debug debug;
	ssds_repo::parse_repo repo;
	
	ssds_xml::create_xml xml;
	xml.add_code((xmlChar* )"213");
	
	debug.flush_xml(xml.rootNodePtr, 0);
	
	if(xml.find_node_by_path((xmlChar*) "//data") == nullptr)
		std::cout << "not found" << std::endl;
	
	//xml.add_child((xmlChar* ) "repolist", (xmlChar* ) "");
	
	//repo.get_repo_url(xml);
	
	//debug.flush_xml(xml.rootNodePtr, 0);
	

	/*
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
	//log.add_log("This message is sent to client logger");*/

	return 0;
}
