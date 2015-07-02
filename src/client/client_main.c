#include "client.h"

// #define VERSION_HAWKEY @HKY_22@
//for debugging
//#define DEBUG

int main(int argc, char* argv[]){
  /*******************************************************************/
  /* Parsing parameters */
  /*******************************************************************/
  ssds_log(logINFO, "Client startup\n");
  
  ParamOptsCl* params = init_params_cl();
  
  ssds_log(logDEBUG, "Client params initialized.\n");

  if(parse_params_cl(argc, argv, params) == -1)
    return 1;

  ssds_log(logDEBUG, "Client params parsed. Package count %d.\n", params->pkg_count);  
  ssds_log(logMESSAGE, "Client startup. Required package count %d.\n", params->pkg_count); 

  /*******************************************************************/
  /* Creating json with all the info*/
  /*******************************************************************/
#ifndef DEBUG

  ssds_log(logDEBUG, "Client JSON creating. Package count %d.\n", params->pkg_count);

  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();
  ssds_log(logDEBUG, "Local repo info initialized on %d. Package count %d.\n", local_repo, params->pkg_count);
  
  SsdsJsonCreate* json_gen = ssds_js_cr_init();
  ssds_log(logDEBUG, "Json create initialized on %d. Package count %d.\n", json_gen, params->pkg_count);

  SsdsJsonRead* json_read = ssds_json_read_init();
  ssds_log(logDEBUG, "Json read initialized on %d. Package count %d.\n", json_read, params->pkg_count);
  
  ssds_js_insert_code(json_gen, 123); //insert code into json
  ssds_log(logDEBUG, "Inserted code 123 into json. Package count %d.\n", params->pkg_count);

  ssds_parse_default_repo(local_repo); //parsing local repo
  ssds_log(logDEBUG, "Local repo is parsed. Package count %d.\n", params->pkg_count);

  ssds_get_repo_urls(local_repo, json_gen);
  ssds_log(logDEBUG, "Getting repo urls. Package count %d.\n", params->pkg_count);
  
  ssds_log(logDEBUG, "Loop thrue required packages. Package count %d.\n", params->pkg_count);
  for(int i=0; i<params->pkg_count; i++)
  {
    char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
    ssds_js_add_package(json_gen, pkg);
    ssds_log(logDEBUG, "Added %s package as %d in order.\n", pkg, i);
  }
  ssds_log(logDEBUG, "Loop is done.\n");

  char* repo_output;
  ssds_log(logDEBUG, "Generating output message with repo info to server.\n");
  repo_output = ssds_js_to_string(json_gen);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\n---- END OF PARSING PART ----\n\n", repo_output);
  
  /*******************************************************************/
  /* Creating json for communication with server*/
  /* For now client only sends to server that he is going to send @system.solv file*/
  /* TODO - create some client identification and store @system.solv file.*/
  /*******************************************************************/

  SsdsJsonCreate* json_msg = ssds_js_cr_init();
  ssds_js_insert_code(json_msg, 10); //code for sending system.solv file, this code can change in time

  char* msg_output;
  ssds_log(logDEBUG, "Generating output message with info about sending @system.solv file to server.\n");
  msg_output = ssds_js_to_string(json_msg);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\n---- END OF PARSING PART ----\n\n", msg_output);


#if 1
  /***************************************************************/
  /* Networking part - sending data to server and recieving      */
  /***************************************************************/
  ssds_log(logDEBUG, "Begin of network part.\n");

  int socket_desc, connection_try = 1;
//   const char * message = "Hello from client\n";
  socket_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  ssds_log(logDEBUG, "Setted up socket descriptor.\n");

  ssds_log(logDEBUG, "Setting up connection to server.\n");
  struct sockaddr_in server;
  server.sin_addr.s_addr=inet_addr("127.0.0.1");
  ssds_log(logDEBUG, "Set server address.\n");

  server.sin_family=AF_INET;
  ssds_log(logDEBUG, "Set comunication protocol.\n");

  server.sin_port=htons(2345);
  ssds_log(logDEBUG, "Set server port.\n");
  
  ssds_log(logDEBUG, "Socket controll.\n");
  if(socket_desc==-1)
  {
    ssds_log(logERROR, "Client encountered an error when creating socket for communication\n");
    return 1;
  }
  ssds_log(logDEBUG, "Socket controll - OK\n"); 
  
  ssds_log(logMESSAGE, "Trying to connect to server...(1 of 3)\n");
  while((connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) && (connection_try < 3))
  {  
     ssds_log(logMESSAGE, "Unable to contact server. Trying that again above 5 sec.\n");
     sleep(5);
     ssds_log(logMESSAGE, "Trying to connect to server...(%d of 3)\n", ++connection_try);
  }
  if(connection_try == 3){
    ssds_log(logERROR, "Unable to contact server. Please, check out your network connection and try it again later.\n");
    return 1;
  }
  ssds_log(logMESSAGE, "Connection to server is established.\n");
  
  /***********************************************************/
  /* Sending @System.solv file                               */
  /***********************************************************/
  ssds_log(logMESSAGE, "Sending info about sending @System.solv file to server.\n");
  write(socket_desc, msg_output, strlen(msg_output));
  ssds_log(logMESSAGE, "Message sent.\n");

  char path[100];
  ssds_resolve_dependency_file_path(path);
  FILE * f;

  ssds_log(logDEBUG, "Path to sys.solv file : %s\n",path);
  f = fopen(path,"rb");
  if(f == NULL)
  {
      ssds_log(logERROR,"Error while opening @System.solv file.\n");
      return 1;
  }

  char buffer[1048576];
  char* server_response;
  int i = 0;
  size_t bytes = 0;
  while((bytes = fread(buffer, 1, 1048576, f)) != 0)
  {
      ssds_log(logDEBUG, "Read %d bytes of data for the %d time.\n",bytes, i);
      i++;
      write(socket_desc, buffer, bytes);
      server_response = sock_recv(socket_desc);
  }
  msg_output = "@System.solv file sent";
  write(socket_desc, msg_output, strlen(msg_output));



  /***********************************************************/
  /* Sending repo info to server                             */
  /***********************************************************/
  ssds_log(logMESSAGE, "Sending message with repo info to server.\n");
  write(socket_desc, repo_output, strlen(repo_output));
  ssds_log(logDEBUG, "Message sent.\n");

  ssds_log(logMESSAGE, "Reading answer from server.\n");
  char* buf=sock_recv(socket_desc);
  ssds_log(logDEBUG, "Checking answer.\n");
  if(buf == NULL)
  {
    ssds_log(logERROR, "Error while recieving data\n");
    return 1;
  }
  ssds_log(logDEBUG, "Answer is OK.\n\n%s\n\n");

  /***********************************************************/
  /* Downloading packages part                               */
  /***********************************************************/

  ssds_log(logDEBUG, "Begin downloading part.\n");
  // required variables
  gboolean return_status;
  LrHandle *handler;
  GSList *package_list = NULL;
  LrPackageTarget *target;
  GError *error = NULL;
  char **packages, **urls;
  
  // parse response
  ssds_log(logDEBUG, "Parsing answer.\n");
  if(!ssds_read_parse(buf,json_read)){
      ssds_log(logERROR, "Error while parsing recived data\n");
      return 1;
  }
  ssds_log(logDEBUG, "Answer parsed.\n");

                               
  // TODO:
  // get available urls
  // ssds_log(logDEBUG, "Reading URLs from answer.\n");
  // ssds_read_get_urls(urls, json_read);
  
  // get names of packages
  // ssds_log(logDEBUG, "Reading packages name from answer.\n");
  int num_pkgs/* = ssds_read_get_packages_string(pkgs,packages,json_read)*/;
  ssds_log(logMESSAGE, "Number of package to install: %d.\n", num_pkgs);
  
  printf("%s", buf);
  ssds_log(logDEBUG, "Downloading preparation.\n");
  handler = lr_handle_init();
  ssds_log(logDEBUG, "Download handler initied.\n");
  lr_handle_setopt(handler, NULL, LRO_URLS, urls);
  ssds_log(logDEBUG, "Array of URLs is setted.\n");
  lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
  ssds_log(logDEBUG, "Array of packages is setted.\n");
  
  ssds_log(logDEBUG, "Loop thrue all packages to download.\n");
  ssds_log(logMESSAGE, "Package to download:\n");
  for(int i = 0; i < num_pkgs; i++){
  
     // Prepare list of target
     ssds_log(logDEBUG, "Package name: %s as %d in order.\n", packages[i], i+1);
     ssds_log(logMESSAGE, "\t%s\n", packages[i]);
     target = lr_packagetarget_new(handler, packages[i], DOWNLOAD_TARGET, LR_CHECKSUM_UNKNOWN,
                                   NULL, 0, NULL, TRUE, NULL, NULL, &error);
     package_list = g_slist_append(package_list, target);
     ssds_log(logDEBUG, "Package added to download list.\n");
  }
  
  // Download all packages        
  ssds_log(logMESSAGE, "Downloading packages.\n");
  return_status = lr_download_packages(package_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
  
  if(!return_status || error != NULL){
      ssds_log(logERROR, "%d: %s\n", error->code, error->message);
      g_error_free(error);
      return 1;
  }

  ssds_log(logMESSAGE, "All packages were downloaded successfully.  \n---- END OF MESSAGES ----\n");
  

  /*********************************************************/
  /* Installing packages                                   */
  /*********************************************************/

  for(GSList *elem = package_list; elem; elem = g_slist_next(elem)){

      char command[300];
      LrPackageTarget *target = (LrPackageTarget *)elem->data;

      if(!target->err){
          sprintf(command, "rpm --install --nodeps %s", target->local_path);
          printf("%s\n",command);
          system(command);
      }else{
          ssds_log(logERROR, "Package Error: %s\n", target->err);
      }

  }
 
  g_slist_free_full(package_list, (GDestroyNotify) lr_packagetarget_free);
  lr_handle_free(handler);
  
  free(buf);
#endif
#endif
  return 0;
}
