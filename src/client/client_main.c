#include "client.h"


//for debugging
//#define DEBUG

int main(int argc, const char* argv[]){
  /*******************************************************************/
  /* Parsing parameters */
  /*******************************************************************/
  ParamOptsCl* params = init_params_cl();
  
  if(parse_params_cl(argc, argv, params) == -1)
    return 1;
  
  /*******************************************************************/
  /* Creating json with all the info*/
  /*******************************************************************/
#ifndef DEBUG
  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();
  SsdsJsonCreate* json_gen = ssds_js_cr_init();
  SsdsJsonRead* json_read = ssds_json_read_init();
  
  ssds_js_insert_code(json_gen, 123); //insert code into json
  ssds_parse_default_repo(local_repo); //parsing local repo
  ssds_get_repo_urls(local_repo, json_gen);
  
  for(int i=0; i<params->pkg_count; i++)
  {
    char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
    ssds_js_add_package(json_gen, pkg);
  }
  
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
    ssds_log(logERROR, "Client encountered an error when creating socket for communication\n");
    return 1;
  }
  
  if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server))<0)
  {  
    ssds_log(logERROR, "Connection error\n");
    return 1;
  }
  
  write(socket_desc, output, strlen(output));
  char* buf=sock_recv(socket_desc);
  if(buf == NULL)
  {
    ssds_log(logERROR, "Error while recieving data\n");
    return 1;
  }

  /***********************************************************/
  /* Downloading packages part                               */
  /***********************************************************/

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
  int num_pkgs/* = ssds_read_get_packages_string(pkgs,packages,json_read)*/, i = 0;
  
  printf("%s", buf);
  
  handler = lr_handle_init();
  lr_handle_setopt(handler, NULL, LRO_URLS, urls);
  lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
  
  for(i; i < num_pkgs; i++){
  
     // Prepare list of target
     target = lr_packagetarget_new(handler, packages[i], DOWNLOAD_TARGET, LR_CHECKSUM_UNKNOWN,
                                   NULL, 0, NULL, TRUE, NULL, NULL, &error);
     package_list = g_slist_append(package_list, target);
  }
  
  // Download all packages        
  return_status = lr_download_packages(package_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
  
  if(!return_status || error != NULL){
//       char *err_message;
//       sprintf(err_message,"%d: %s\n", error->code, error->message);
      ssds_log(logERROR, "%d: %s\n", error->code, error->message);
      g_error_free(error);
      return 1;
  }
  
  g_slist_free_full(package_list, (GDestroyNotify) lr_packagetarget_free);
  lr_handle_free(handler);
  
  free(buf);
#endif
#endif
  return 0;
}
