//============================================================================
// Name		: server.hpp
// Author	: mruprich
// Version	: 0.2
// Copyright	: GNU GPL
// Description	: Server side of SSDS
//============================================================================

#ifndef _SERVER_HPP
#define _SERVER_HPP

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
#include <thread>
#include <utility>//for std::move()

//SSDS
#include "../common/logger.hpp"
#include "../common/solving.hpp"
#include "../common/repo_handler.hpp"
#include "../common/xml_handler.hpp"
#include "../common/json_handler.hpp"


namespace ssds_server {
	class server : private boost::noncopyable {
	public:
            server();
            ~server();
            boost::asio::io_service& getIo();
            int newPort(int);
            void listen();
            void connect();
            void close();
            void process_connection(boost::asio::ip::tcp::socket& sock);
            
            
            void session(boost::asio::ip::tcp::socket sock,boost::system::error_code ec);
		
		
		
	private:
	  boost::asio::io_service io_service_;         /*basic object for boost operations*/
	  int port = 4242;
	};
}//ssds_ server
#endif
