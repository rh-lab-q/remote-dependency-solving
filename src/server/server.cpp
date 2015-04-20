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
    ssds_json::json_read json_parser;
    ssds_json::json_create json_answer;
    ssds_repo::repo_metadata metadata;
    ssds_xml::xml_debug debug;
    
    boost::system::error_code ec;
    int64_t size;

    
    //recieving data
    boost::asio::read(sock,boost::asio::buffer(&size,sizeof(size)));
    std::vector<char> buf(size);
    size_t len = sock.read_some(buffer(buf), ec);
    std::string input_message = std::string(buf.begin(), buf.end());
    
    json_parser.parse_data((char*)input_message.c_str());
    struct ssds_json::json_read::pkgInfo* pkgs = json_parser.pkg_info_init();
    json_parser.get_packages(pkgs);
    json_parser.get_repo_info();
    
    int list_len = (int)g_slist_length(json_parser.repoInfoList);
    for(int i=0; i<list_len; i++)
    {
      ssds_json::json_read::repoInfo* repo_info = (ssds_json::json_read::repoInfo*)g_slist_nth_data(json_parser.repoInfoList, i);
      std::cout << repo_info->name << std::endl;
    }
    
    /*
     * Here I would put something that will decide what to do according to the code from the client
     */
    metadata.locate_repo_metadata_by_url(json_parser);
    
    std::cout<< "Message has " << len << " characters." << std::endl;
    solvePoint.fill_sack(metadata);
    
    solvePoint.answer(json_parser, json_answer);
    std::string message = json_answer.json_to_string();
//     json_answer.json_dump();
//     message="some random message";
    write(sock, buffer(message), ec);
  }//process_connection


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
