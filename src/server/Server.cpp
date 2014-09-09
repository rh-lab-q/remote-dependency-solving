//============================================================================
// Name        : Server.cpp
// Author      : brumlablo
// Version     :
// Copyright   : GNU GPL
// Description : Hello World in C, Ansi-style
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/
/*synchronous start of server based on boost.asio library*/

#include "Server.hpp"


ssds_server::Server::Server(){
}

/*ssds_server::Server::Server(const ssds_server::Server& other) {

}*/

ssds_server::Server::~Server() {

}
int ssds_server::Server::newPort(int newport){
	port = newport;
	return port;
}
boost::asio::io_service& ssds_server::Server::getIo(){
	return io_service_;
}

using namespace boost::asio;

int main() {


	ssds_server::Server mainserver;
	//logger::log my_log;
	int portnum = 40002;
	portnum = mainserver.newPort(portnum);
	boost::asio::io_service &ios = mainserver.getIo();

	ip::tcp::endpoint endpoint_(ip::tcp::v4(),portnum);
	ip::tcp::acceptor acceptor_(ios,endpoint_);  //listener
	ip::tcp::socket sock(ios);
	std::cout << "Server is ready..." << "\n";

	try {
		while(42)
		{
			acceptor_.accept(sock);

			std::string message = "Server says hello!\n";

			boost::system::error_code ec;
			write(sock, buffer(message), ec);

		}
	} /*future handling errors and exceptions*/
		catch (std::exception& e){
		    std::cerr << "Demonstrated exception: " << e.what() << "\n";
		}
	return 0;
}
