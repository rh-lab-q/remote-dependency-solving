//============================================================================
// Name		: server.cpp
// Author	: brumlablo
// Editor	: Jozkar, mruprich	
// Version	: 0.2
// Copyright	: GNU GPL
// Description	: Server side of SSDS
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/
/*synchronous start of server based on boost.asio library*/

#include "server.hpp"

namespace ssds_server {
  using namespace boost::asio;
  
  server::server(){
  }

  server::~server() {
    
  }
  
  int server::newPort(int newport){
	  port = newport;
	  return port;
  }
  
  boost::asio::io_service& server::getIo(){
	  return io_service_;
  }

  /*
   * This function processes incoming data - it will probably be forked later
   */
  void server::process_connection(boost::asio::ip::tcp::socket& sock)
  {
    ssds_solving::solve solvePoint;
    ssds_xml::read_xml xml;
    ssds_repo::repo_metadata metadata;
    
    boost::system::error_code ec;
    int64_t size;
    
    boost::asio::read(sock,boost::asio::buffer(&size,sizeof(size)));
    
    std::vector<char> buf(size);
    size_t len = sock.read_some(buffer(buf), ec);
    std::string input_message = std::string(buf.begin(), buf.end());
    
    //solvePoint.parseMessage(input_message);
    xml.parse_xml_string(input_message);
    std::cout << "zde" <<std::endl;
    xml.get_node_by_path((xmlChar* )"//data/repolist/repo", metadata.urls);
    
    //metadata.by_url((*metadata.urls).at(0)->value);
    
    //debug
//     for(std::vector<ssds_xml::xml_node*>::iterator it = metadata.urls->begin(); it != metadata.urls->end(); it++){//result is of the same type as ret_vector_ptr
//       std::cout << (*it)->value << std::endl;
//     }
    
    std::cout<< "Message has " << len << " characters." << std::endl;
    //std::cout << "Message:" << input_message << std::endl;
    
    //std::string message = solvePoint.answer(input_message);
    
    std::string message = "this is some random message from server";
    write(sock, buffer(message), ec);
  }


  void server::session(ip::tcp::socket sock,boost::system::error_code ec){
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
}
