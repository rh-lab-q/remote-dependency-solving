//============================================================================
// Name		: server_main.cpp
// Author	: brumlablo
// Editor	: Jozkar, mruprich
// Version	: 0.2
// Copyright	: GNU GPL
// Description	: Server side of SSDS
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/
/*synchronous start of server based on boost.asio library*/

#include "server.hpp"
#include "../common/solving.hpp"

//#include <hawkey/sack.h>


using namespace boost::asio;

#if 0
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
#endif

int main() {

	ssds_server::server mainserver;
	
	logger::log my_log;
	int portnum = 40002;
	portnum = mainserver.newPort(portnum);
	boost::asio::io_service &ios = mainserver.getIo();	
	ip::tcp::endpoint endpoint_(ip::tcp::v4(),portnum);
	ip::tcp::acceptor acceptor_(ios,endpoint_);  //listener
	std::cout << "Server is ready..." << "\n";
	boost::system::error_code ec;
	//ip::tcp::socket sock(ios);
	
	ssds_solving::solve solveHandler;
	
	
#if 1
	try {
			//ip::tcp::iostream streams("");
		while(42) {
  			ip::tcp::socket sock(ios);
			acceptor_.accept(sock); //second argument can be error handler
			
			std::cout<<"some connection was accepted" << std::endl;
			
			mainserver.process_connection(sock);
#if 0
			int64_t size;
			boost::asio::read(sock,boost::asio::buffer(&size,sizeof(size)));
			
			std::vector<char> buf(size);
			size_t len = sock.read_some(buffer(buf), ec);
			std::string input_message = std::string(buf.begin(), buf.end());

			std::cout<< "Message has " << len << " characters." << std::endl;
		
			//solving
			std::string message = solvePoint.answer(input_message);
#endif	
			
			sock.close();

			//far future		    
			//std::thread(session, std::move(sock).std::move(ec)).detach();

		}
	} /*handling exceptions*/
	catch (std::exception& e){
		//std::ostringstream os;
		//os << "Server: "<< e.what();
		/*std::cerr*/my_log.add_log(logERROR,e.what());
	}

#endif
	return 0;
}
