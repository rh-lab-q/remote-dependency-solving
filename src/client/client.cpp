#include <iostream>
//needs to be compiled with -lboost_system option
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "../common/logger.hpp"
#include <string>

using namespace boost::asio;

int main(int argc, const char* argv[]){
	//pouzit libxml
	
	
	logger::log my_log;

	//io_service object is needed to access socket
	io_service io_service;

	//resolver for dns query
	ip::tcp::resolver resolver(io_service);
	ip::tcp::resolver::query query("tik.cesnet.cz", "daytime");
	
	//a list of responses is returned
	ip::tcp::resolver::iterator dns_return_list = resolver.resolve(query);
	ip::tcp::socket my_socket(io_service);

	//openning a connection through the created socket
	connect(my_socket, dns_return_list);

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

	my_log.get_info(logINFO) << "message from client" << std::endl;
	my_log.get_info(logINFO) << "message from client" << std::endl;
	my_log.get_info(logINFO) << "message from client" << std::endl;
	my_log.get_info(logINFO) << "message from client" << std::endl;
	//log.add_log("This message is sent to client logger");

	return 0;
}
