#include "client.h"


//for debugging
//#define DEBUG
namespace params = boost::program_options;

int main(int argc, const char* argv[]){
  if(parse_params(argc, argv) == -1)
    return 1;
#if 0
  ssds_client::client client; //object for network handling
  
  /*******************************************************************/
  /* Creating xml with all the info*/
  /*******************************************************************/
  ssds_repo::parse_repo repo; //for parsing .repo files
  ssds_json::json_create json_gen;
  ssds_json::json_read json_read;
	
#ifndef DEBUG
  json_gen.insert_code(1);
  repo.parse_default_repo();
  
//   for(std::vector<std::string>::iterator it = parameters.packages.begin(); it != parameters.packages.end(); it++){
//     json_gen.add_package((char*)(*it).c_str());
//   }
  
  repo.get_repo_url(json_gen);
  char* output;
  output = json_gen.json_to_string();
//   std::cout << "output: " << output <<"\n\n" << std::endl;
  json_gen.json_dump();
  
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
  }

  //openning a connection through the created socket
  //connect(my_socket, resolver.resolve(query));
  ip::tcp::socket my_socket(client.io_service_object);
  my_socket.connect(endpoint);

  boost::array<char, 128> buf;
  boost::system::error_code error;

  //findout length of xml string
  std::string string_output = output;
  int64_t size = string_output.size();
  
  //write information about data length	
  write(my_socket, boost::asio::buffer(&string_output, sizeof(size)));

  //write data
  write(my_socket, boost::asio::buffer(string_output));

  std::string rec="";
  //read the answer from server
  for (;;) {
    size_t len = my_socket.read_some(buffer(buf), error);
//     json_read.parse_data(buf.data());
//     std::cout.write(buf.data(), len);
    rec+=buf.data();
    
    
    if(error==error::eof)
              break;
    else if (error)
              throw boost::system::system_error(error);
  }

  std::cout << rec << std::endl;
  //my_log.add_log(logINFO) << "message from client" << std::endl;
  //log.add_log("This message is sent to client logger");*/
#endif	
  repo.free_resources();
#endif
  return 0;
}
