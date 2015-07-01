#include "client.h"

// #define VERSION_HAWKEY @HKY_22@
//for debugging
//#define DEBUG

int main(int argc, char* argv[]){
  /*******************************************************************/
  /* Parsing parameters */
  /*******************************************************************/
  ssds_log(logDEBUG, "Client startup\n");
  
  ParamOptsCl* params = init_params_cl();
  
  ssds_log(logDEBUG, "Client params initialized.\n");

  if(parse_params_cl(argc, argv, params) == -1)
    return 1;

  ssds_log(logDEBUG, "Client params parsed. Package count %d.\n", params->pkg_count);  
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

  char* output;
  ssds_log(logDEBUG, "Generating output message to server.\n");
  output = ssds_js_to_string(json_gen);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\nEND OF PARSING PART\n\n", output);
  
#if 1
  /***************************************************************/
  /* Networking part - sending data to server and recieving      */
  /***************************************************************/
  ssds_log(logDEBUG, "Begin of networking part.\n");

  int socket_desc;
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
  
  ssds_log(logDEBUG, "Trying to connect to server...\n");
  if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)
  {  
    ssds_log(logDEBUG, "Connection error\n");
    return 1;
  }
  ssds_log(logDEBUG, "Connection to server is established.\n");
  
  ssds_log(logDEBUG, "Sending message to server.\n");
  write(socket_desc, output, strlen(output));
  ssds_log(logDEBUG, "Message send.\n");

  ssds_log(logDEBUG, "Reading answer fromserver.\n");
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

  ssds_log(logDEBUG, "Begin downloading part. \n---- END OF MESSAGES ----\n");
  // required variables
  gboolean return_status;
  LrHandle *handler;
  GSList *package_list = NULL;
  LrPackageTarget *target;
  GError *error = NULL;
  char **packages, **urls;
  
  // parse response
  if(!ssds_read_parse(output,json_read)){
      ssds_log(logERROR, "Error while parsing recived data\n");
      return 1;
  }

                               
  // TODO:
  // get available urls
  // ssds_read_get_urls(urls, json_read);
  
  // get names of packages
  int num_pkgs/* = ssds_read_get_packages_string(pkgs,packages,json_read)*/;
  
  printf("%s", buf);
  
  handler = lr_handle_init();
  lr_handle_setopt(handler, NULL, LRO_URLS, urls);
  lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
  
  for(int i = 0; i < num_pkgs; i++){
  
     // Prepare list of target
     target = lr_packagetarget_new(handler, packages[i], DOWNLOAD_TARGET, LR_CHECKSUM_UNKNOWN,
                                   NULL, 0, NULL, TRUE, NULL, NULL, &error);
     package_list = g_slist_append(package_list, target);
  }
  
  // Download all packages        
  return_status = lr_download_packages(package_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
  
  if(!return_status || error != NULL){
      ssds_log(logERROR, "%d: %s\n", error->code, error->message);
      g_error_free(error);
      return 1;
  }
  

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
