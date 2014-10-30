#include <iostream>
//needs to be compiled with -lboost_system option
#include "client.hpp"
#include <string>
#include <exception>
#include <fstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/program_options.hpp>
#include "../common/logger.hpp"
#include "../common/xml_handler.hpp"
#include "../common/repo_handler.hpp"
#include "../common/params.hpp"

namespace params = boost::program_options;

int main(int argc, const char* argv[]){
	logger::log my_log; //logger init
	ssds_client::client client; //object for network handling
	ssds_params::params parameters;
	
	parameters.parse_params(argc, argv);
	
	/********************************************************/
	/* Parsing arguments using boost library*/
	/********************************************************/
	/*std::vector<std::string> files;//vector of packages to download
	
	params::options_description desc("Allowed parameters");
	desc.add_options()
			("help", "produce help message")
			("checkdep", "only shows packages required to perform install - nothing will be installed")
			("install", "install requested packages")
			("pkg", params::value<std::vector<std::string>>(&files),"name of package(s)")
	;
	
	params::variables_map vm;
	params::store(params::parse_command_line(argc, argv, desc), vm);
	params::notify(vm);

	if(vm.count("help")){
		std::cout << "tady ma byt usage" << std::endl;
		return 1;
	}
	
	if(vm.count("checkdep") && vm.count("install")){
		std::cout << "install and checkdep cannot run together" << std::endl;
		return 0;
	}
	
	if(files.size() == 0){
		std::cout << "No packages were provided" << std::endl;
		return 1;
	}*/

	/*******************************************************************/
	/* Creating xml with all the info*/
	/*******************************************************************/
	ssds_xml::xml_debug debug; //for xml flushing
	ssds_repo::parse_repo repo; //for parsing .repo files
	ssds_xml::create_xml xml; //for creating xml
	
	xml.add_code((xmlChar* )"001");
	
	//repo.get_repo_url(xml);//get all repo info
	xml.add_child(xml.dataNodePtr, (xmlChar* ) "req_packages", (xmlChar* ) "");
	xml.currNodePtr = xml.addedNodePtr;//addedNodePtr may change in the iteration so I use currNodePtr instead 
	
	for(std::vector<std::string>::iterator it = parameters.packages.begin(); it != parameters.packages.end(); it++){
		xml.add_child(xml.currNodePtr, (xmlChar*) "package", (xmlChar*) (*it).c_str());
	}
	
	xml.doc_to_str();
	xml.str_output += "\n";
	
	//debug.flush_xml(xml.rootNodePtr, 0);*/
	
	
	/**************************************************************/
	/* Networking part - sending data to server and recieving*/
	/***************************************************************/
	using namespace boost::asio;
	
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
		//std::cout << endpoint << std::endl;
	}

	ip::tcp::socket my_socket(client.io_service_object);
	my_socket.connect(endpoint);

	//openning a connection through the created socket
	//connect(my_socket, resolver.resolve(query));
	//read the answer from server
	for (;;) {
		boost::array<char, 128> buf;
		boost::system::error_code error;

		std::string msg = "pokus z klienta\n";
		//size_t len = my_socket.read_some(buffer(buf), error);
		write(my_socket, boost::asio::buffer(xml.str_output));
		
		size_t len = my_socket.read_some(buffer(buf), error);
		std::cout.write(buf.data(), len);

		if(error==error::eof)
			break;
		else if (error)
			throw boost::system::system_error(error);

		
	}	
	//my_log.add_log(logINFO) << "message from client" << std::endl;
	//log.add_log("This message is sent to client logger");*/
	xml.free_resources();
	repo.free_resources();
	return 0;
}
