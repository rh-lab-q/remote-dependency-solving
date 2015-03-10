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
    ssds_xml::xml_debug debug;
    
    boost::system::error_code ec;
    int64_t size;
    
    boost::asio::read(sock,boost::asio::buffer(&size,sizeof(size)));
    
    std::vector<char> buf(size);
    size_t len = sock.read_some(buffer(buf), ec);
    std::string input_message = std::string(buf.begin(), buf.end());
    
    //solvePoint.parseMessage(input_message);
    xml.parse_xml_string(input_message);
    
    xml.get_node_by_path((xmlChar* )"//data/repolist/repo", metadata.urls);
    
    //debug.flush_url_vector(metadata.urls);
    //std::cout << "zde pred for v server.cpp" <<std::endl;
    int count = 0;
    std::cout << "server pred for: " << metadata.urls->size() << std::endl;
    
    for(std::vector<ssds_xml::xml_node*>::iterator it = metadata.urls->begin(); it != metadata.urls->end(); it++){
      metadata.by_url((*it), solvePoint.repo_info);
      count++;
      if(count==2)
	 break;
      //std::cout << "uvnitr for" << std::endl;
    }
    

    
    std::cout<< "Message has " << len << " characters." << std::endl;
    //std::cout << "Message:" << input_message << std::endl;
    
    //std::string message = solvePoint.answer(input_message);
    
    solvePoint.fillSack();
    
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
