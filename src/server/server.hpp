#ifndef SERVER_HPP
#define SERVER_HPP

//#include <stdlib.h>
//#include <stdio.h>

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <ctime>
#include <cstdlib>
#include "../common/logger.hpp"
#include <thread>
#include <utility>//for std::move()


//void session(boost::asio::ip::tcp::socket sock,boost::system::error_code ec);

namespace ssds_server {

	class server : private boost::noncopyable {
	public:
		server();
	    /*Server(const Server&other);*/
	    ~server();
	    boost::asio::io_service& getIo();
	    int newPort(int);
		//void handle_accept(const boost::system::error_code& ec);
		void listen();
		void connect();
		void close();
		
		void session(boost::asio::ip::tcp::socket sock,boost::system::error_code ec);
		
	private:
		boost::asio::io_service io_service_;         /*basic object for boost operations*/
		int port = 4242;
	};
}//ssds_ server
#endif
