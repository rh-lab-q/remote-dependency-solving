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


void session(ip::tcp::socket sock,boost::system::error_code ec){
	try {
		while(42)
		{
			char data[1024];
			size_t lengthsome = sock.read_some(boost::asio::buffer(data), ec);
			 if (ec == boost::asio::error::eof) {
				 std::cout << "Connection closed." << "\n";
		 	 	 break; // Connection closed cleanly by peer.
			 }
			  else if (ec)
			      throw boost::system::system_error(ec);
		      	  boost::asio::write(sock, boost::asio::buffer(data, lengthsome));
		 }//while
		}//try
	 catch (std::exception& e) {
		    std::cerr << "Exception in thread: " << e.what() << "\n";
	 }
}


int main() {

	ssds_server::Server mainserver;
	logger::log my_log;
	int portnum = 40002;
	portnum = mainserver.newPort(portnum);
	boost::asio::io_service &ios = mainserver.getIo();
	ip::tcp::endpoint endpoint_(ip::tcp::v4(),portnum);
	ip::tcp::acceptor acceptor_(ios,endpoint_);  //listener
	std::cout << "Server is ready..." << "\n";
	boost::system::error_code ec;
	ip::tcp::socket sock(ios);

	try {
			//ip::tcp::iostream streams("");
		while(42) {
			acceptor_.accept(sock); //second argument can be error handler
			std::string message = "Server says hello!\n";
			write(sock, buffer(message), ec);
		    //std::thread(session, std::move(sock).std::move(ec)).detach();

		}
	} /*handling exceptions*/
	catch (std::exception& e){
		std::ostringstream os;
		os << "Server: "<< e.what();
		/*std::cerr*/my_log.add_log(logERROR,os.str());
	}


	return 0;
}