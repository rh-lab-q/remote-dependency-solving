#include "client.h"


//for debugging
//#define DEBUG

int main(int argc, const char* argv[]){
  /*******************************************************************/
  /* Parsing parameters */
  /*******************************************************************/
  ParamOpt* params = init_params();
  
  if(parse_params(argc, argv, params) == -1)
    return 1;
  
  
  
  /*******************************************************************/
  /* Creating json with all the info*/
  /*******************************************************************/
//   ssds_repo::parse_repo repo; //for parsing .repo files
//   ssds_json::json_create json_gen;
//   ssds_json::json_read json_read;
//   json_gen.insert_code(1);
//   repo.parse_default_repo();
//   repo.get_repo_url(json_gen);
	
#ifndef DEBUG
  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();
  SsdsJsonCreate* json_gen = ssds_js_cr_init();
  SsdsJsonRead* json_read = ssds_json_read_init();
  
  ssds_js_insert_code(json_gen, 123); //insert code into json
  ssds_parse_default_repo(local_repo); //parsing local repo
  ssds_get_repo_urls(local_repo, json_gen);
  
  ssds_js_dump(json_gen);
  char* output;
  output = ssds_js_to_string(json_gen);
  
#if 1
  /**************************************************************/
  /* Networking part - sending data to server and recieving*/
  /***************************************************************/
  int socket_desc;
//   const char * message = "Hello from client\n";
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  struct sockaddr_in server;
  server.sin_addr.s_addr=inet_addr("127.0.0.1");
  server.sin_family=AF_INET;
  server.sin_port=htons(2345);
  
  if(socket_desc==-1)
  {
    ssds_log("Client encountered an error when creating socket for communication.", logERROR);
    return 1;
  }
  
  if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)
  {  
    ssds_log("Connection error.", logERROR);
    return 1;
  }
  
  write(socket_desc, output, strlen(output));
  char* buf=sock_recv(socket_desc);
  if(buf == NULL)
  {
    ssds_log("Error while recieving data.", logERROR);
    return 1;
  }
  
  printf("%s", buf);
  
  free(buf);
#endif
#endif
  return 0;
}
