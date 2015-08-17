#include "client.h"

// #define VERSION_HAWKEY @HKY_22@
//for debugging
//#define DEBUG

int main(int argc, char* argv[]){
  /*******************************************************************/
  /* Setting up garbage collector and setting callback functions */
  /*******************************************************************/
  uid_t uid = geteuid();
  if(uid != 0)
  {
    ssds_log(logERROR, "This program has to be run under the root user otherwise no packages can be installed, erased or updated.\n");
    return 1;
  }
  
  ssds_gc_init();
  signal(SIGINT, ssds_signal_handler);
  signal(SIGBUS, ssds_signal_handler);
  signal(SIGSEGV, ssds_signal_handler);
  signal(SIGTERM, ssds_signal_handler);

  /*******************************************************************/
  /* Parsing parameters */
  /*******************************************************************/
  
  ssds_log(logSSDS, "Client startup.\n");

  ParamOptsCl* params = init_params_cl();
  
  if(parse_params_cl(argc, argv, params) != 0)
  {
    ssds_gc_cleanup();
    return PARAMS_ERROR;
  }

  ssds_log(logDEBUG, "Client params initialized.\n");
  ssds_log(logDEBUG, "Client params parsed. Package count %d.\n", params->pkg_count);  
  ssds_log(logMESSAGE, "Client startup. Required package count %d.\n", params->pkg_count); 
 
  // this switch is now just for info about selected command
  switch(params->command)
  {

	case PAR_INSTALL: 
		ssds_log(logMESSAGE, "Installation of packages was selected.\n");
		/* TODO (even in update and dep. check)
		 *  check id (if NULL - ask for it from server by message)
		 *  connect to server
		 *  if new id - send @System.solv
		 *  parse repo	
		 *  send repo info to server
		 *  if some repo on client side missing - read warning from server
		 *  send message with requested operation and package names to server
		 *  read dep result
		 *  print dep result to user
		 *  
		 *  ----- only in update and install -----
		 *  ask if (update/install)/download/cancel 
		 *  download packages
		 *  install them
		 */
		break;
	case PAR_UPDATE: 
		ssds_log(logMESSAGE, "Update of packages was selected.\n");
                break;
	case PAR_ERASE: 
		ssds_log(logMESSAGE, "Erase of packages was selected.\n");
		break;
	case PAR_CHK_DEP: 
		ssds_log(logMESSAGE, "Dependency check of packages was selected.\n");
		break;
  } 
 /* SsdsJsonRead *json_read;
  char *repo_output, *msg_output;
  SsdsJsonAnswer* answer_from_srv;
  GList *package_list;

  int ret_code = create_json(params, &json_read, &repo_output, &msg_output);
  if (ret_code != 0) return ret_code;

  ret_code = network_part(json_read, repo_output, msg_output, &answer_from_srv);
  if (ret_code != 0) return ret_code;

  ret_code = download_packages(answer_from_srv, &package_list);
  if (ret_code != 0) return ret_code;

  ret_code = install_packages(answer_from_srv, package_list);
  if (ret_code != 0) return ret_code;

  return 0;
}


int create_json(ParamOptsCl *params, SsdsJsonRead **json_read_ret, char **repo_output_ret, char **msg_output_ret)
{*/
  
  check_for_missing_repos(); //check if client misses some repositories

  /*******************************************************************/
  /* Creating json with all the info*/
  /*******************************************************************/
  ssds_log(logDEBUG, "Client JSON creating. Package count %d.\n", params->pkg_count);

  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();
  ssds_log(logDEBUG, "Local repo info initialized on %d. Package count %d.\n", local_repo, params->pkg_count);
  
  SsdsJsonCreate* json_gen = ssds_js_cr_init(SEND_REPO); 
  ssds_log(logDEBUG, "Json create initialized on %d. Package count %d.\n", json_gen, params->pkg_count);

  SsdsJsonRead* json_read = ssds_js_rd_init();
  ssds_log(logDEBUG, "Json read initialized on %d. Package count %d.\n", json_read, params->pkg_count);
  
  ssds_js_cr_insert_code(json_gen, SEND_REPO); //insert code into json
  ssds_log(logDEBUG, "Inserted code %d into json. Package count %d.\n", SEND_REPO, params->pkg_count);
  
  /*
   *  Getting architecture, release and path to @System.solv
   */
  char path[100];
  char *arch = NULL, *release = NULL;
  ssds_resolve_dependency_file_path(path, &arch, &release);

  // parsing local repo
  if(!ssds_parse_default_repo(local_repo))
  {
     ssds_gc_cleanup();
     return REPO_ERROR;
  }
  ssds_log(logDEBUG, "Local repo is parsed. Package count %d.\n", params->pkg_count);

  ssds_get_repo_urls(local_repo, json_gen, arch, release);
  ssds_log(logDEBUG, "Getting repo urls. Package count %d.\n", params->pkg_count);
  
  ssds_log(logDEBUG, "Loop thrue required packages. Package count %d.\n", params->pkg_count);
  for(int i = 0; i < params->pkg_count; i++)
  {
    char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
    ssds_js_cr_add_package(json_gen, pkg);
    ssds_log(logDEBUG, "Added %s package as %d in order.\n", pkg, i);
  }
  ssds_log(logDEBUG, "Loop is done.\n");

  char* repo_output;
  ssds_log(logDEBUG, "Generating output message with repo info to server.\n");
  repo_output = ssds_js_cr_to_string(json_gen);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\n", repo_output);
  
  /*******************************************************************/
  /* Creating json for communication with server*/
  /* For now client only sends to server @System.solv file*/
  /* TODO - create some client identification.*/
  /*******************************************************************/

  SsdsJsonCreate* json_msg = ssds_js_cr_init(SEND_SOLV);
  ssds_js_cr_insert_code(json_msg, SEND_SOLV);

  char* msg_output;
  ssds_log(logDEBUG, "Generating output message with info about sending @System.solv file to server.\n");
  msg_output = ssds_js_cr_to_string(json_msg);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\n---- END OF PARSING PART ----\n\n", msg_output);
  
/*  *json_read_ret = json_read;
  *repo_output_ret = repo_output;
  *msg_output_ret = msg_output;
  return 0;
}

int network_part(SsdsJsonRead *json_read, char *repo_output, char *msg_output, SsdsJsonAnswer** answer_from_srv_ret)
{*/
  /***************************************************************/
  /* Networking part - sending data to server and recieving      */
  /***************************************************************/
  ssds_log(logDEBUG, "Begin of network part.\n");

  int data_sock, comm_sock, connection_try = 1;

  comm_sock = ssds_socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  data_sock = ssds_socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  ssds_log(logDEBUG, "Setted up socket descriptor.\n");

  ssds_log(logDEBUG, "Setting up connection to server.\n");
  struct sockaddr_in server_data;
  struct sockaddr_in server_comm;

  char *server_address;
  long int comm_port, data_port;
  read_cfg(&server_address, &comm_port, &data_port);

  server_comm.sin_addr.s_addr = inet_addr(server_address);
  server_data.sin_addr.s_addr = inet_addr(server_address);
  ssds_log(logDEBUG, "Set server address.\n");

  server_comm.sin_family = AF_INET;
  server_data.sin_family = AF_INET;
  ssds_log(logDEBUG, "Set comunication protocol.\n");

  server_comm.sin_port = htons(comm_port);
  server_data.sin_port = htons(data_port);
  ssds_log(logDEBUG, "Set server port.\n");
  
  ssds_log(logDEBUG, "Socket controll.\n");
  if(comm_sock == -1 || data_sock == -1)
  {
    ssds_log(logERROR, "Client encountered an error when creating sockets for communication and data.\n");
    ssds_gc_cleanup();
    return SOCKET_ERROR;
  }

  ssds_log(logDEBUG, "Socket controll - OK.\n"); 
  
  ssds_log(logMESSAGE, "Trying to connect to server...(1 of 3)\n");
  while((connect(comm_sock, (struct sockaddr *)&server_comm, sizeof(server_comm)) < 0) && (connection_try < 3))
  {  
     ssds_log(logMESSAGE, "Unable to connect to comm. socket on server. Trying that again above 5 sec.\n");
     sleep(5);
     ssds_log(logMESSAGE, "Trying to connect to server...(%d of 3)\n", ++connection_try);
  }

  if(connection_try == 3){
    ssds_log(logERROR, "Unable to connect comm. socket on server. Please, check out your network connection and try it again later.\n");
    ssds_gc_cleanup();
    return NETWORKING_ERROR;
  }

  connection_try = 1;
  
  ssds_log(logMESSAGE, "Trying to connect to data socket...(1 of 3)\n");
  while((connect(data_sock,(struct sockaddr *)&server_data, sizeof(server_data)) < 0) && (connection_try < 3))
  {
      ssds_log(logMESSAGE, "Unable to connect to data socket. Trying again in 5 sec.\n");
      sleep(5);
      ssds_log(logMESSAGE, "Trying to connect to data socket...(%d of 3)\n", ++connection_try);
  }
  if(connection_try == 3){
    ssds_log(logERROR, "Unable to connect data socket on server. Please, check out your network connection and try it again later.\n");
    ssds_gc_cleanup();
    return NETWORKING_ERROR;
  }
  ssds_log(logMESSAGE, "Connection to server is established.\n");
  
  /***********************************************************/
  /* Sending @System.solv file                               */
  /***********************************************************/
  ssds_log(logMESSAGE, "Sending info about sending @System.solv file to server.\n");
  write(comm_sock, msg_output, strlen(msg_output));
  ssds_log(logMESSAGE, "Message sent.\n");

  ssds_log(logDEBUG, "Path to sys.solv file : %s\n",path);
  FILE * f;
  f = fopen(path,"rb");

  ssds_log(logDEBUG, "Opening @System.solv file.\n");
  if(f == NULL)
  {
    ssds_log(logERROR,"Error while opening @System.solv file.\n");
    ssds_gc_cleanup();
    return FILE_ERROR;
  }

  ssds_log(logDEBUG, "Preparing .solv variables.\n");

  char buffer[131072];
  char msg_length[10];
  char* server_response;
  size_t bytes_read = 0;

  ssds_log(logDEBUG, "Getting server response\n");
  server_response = sock_recv(comm_sock);
  ssds_log(logDEBUG, "Sending @System.solv file.\n");
  while((bytes_read = fread(buffer, 1, 131072, f)) != 0)
  {
      snprintf(msg_length,10,"%d",(int)bytes_read);
      write(comm_sock, msg_length, strlen(msg_length));
      ssds_log(logDEBUG, "Command sent.\n");
      write(data_sock, buffer, bytes_read);
      ssds_log(logDEBUG, "Data sent.\n");

      server_response = sock_recv(comm_sock);

      // TODO: fix this nonsens vvvvvvvvvvvvv
      if(strcmp(server_response,"OK") != 0)
      {
        ssds_log(logERROR, "Networking error while sending @System.solv file.\n");
        ssds_gc_cleanup();
        return NETWORKING_ERROR;
      }
  }
  msg_output = "@System.solv file sent";
  write(comm_sock, msg_output, strlen(msg_output));

  /***********************************************************/
  /* Sending repo info to server                             */
  /***********************************************************/
  ssds_log(logMESSAGE, "Sending message with repo info to server.\n");
  write(comm_sock, repo_output, strlen(repo_output));
  ssds_log(logDEBUG, "Message sent.\n");

  ssds_log(logMESSAGE, "Reading answer from server.\n");
  char* buf = sock_recv(comm_sock);
  ssds_log(logDEBUG, "Checking answer.\n");
  
  if(buf == NULL)
  {
    ssds_log(logERROR, "Error while recieving data\n");
    ssds_gc_cleanup();
    return NETWORKING_ERROR;
  }
  ssds_log(logDEBUG, "Answer is OK.\n\n%s\n\n", buf);
  
  // parse response
  ssds_log(logDEBUG, "Parsing answer.\n");

  if(!ssds_js_rd_parse(buf, json_read))
  {
    ssds_log(logERROR, "Error while parsing answer from the server\n");
    ssds_gc_cleanup();
    return JSON_ERROR;
  }
  
  int num_install = ssds_js_rd_get_count(json_read, "install_pkgs");

  for(int id_app = 0; id_app < num_install; id_app++){

	ssds_log(logDEBUG, "Parse init.\n");
	SsdsJsonAnswer* answer_from_srv = ssds_js_rd_answer_init();
	ssds_log(logDEBUG, "Parse answer.\n");
	
	ssds_js_rd_parse_answer(answer_from_srv, json_read, id_app);

	ssds_log(logDEBUG, "Answer parsed.\n");
  
  	/***********************************************************/
	/* Downloading packages part                               */
	/***********************************************************/

	ssds_log(logDEBUG, "Begin downloading part.\n");
        ssds_log(logMESSAGE, "Working on package: %s.\n", answer_from_srv->name);

  	// required variables
	gboolean return_status;
	LrHandle *handler;
	GSList *package_list = NULL;
	LrPackageTarget *target;
	GError *error = NULL;

	for(guint i = 0; i < g_slist_length(answer_from_srv->pkgList); i++){
	     SsdsJsonInstall* inst = (SsdsJsonInstall*)g_slist_nth_data(answer_from_srv->pkgList, i);
	     ssds_log(logMESSAGE, "Downloading preparation for package: %s\n", inst->pkg_name);
   
	     ssds_log(logDEBUG, "Downloading preparation.\n");
	     handler = lr_handle_init();
	     ssds_log(logDEBUG, "Download handler initied.\n");
	     lr_handle_setopt(handler, NULL, LRO_METALINKURL, inst->metalink);
	     ssds_log(logDEBUG, "Array of URLs is setted.\n");
	     lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
	     ssds_log(logDEBUG, "Repo type is setted.\n");
	     lr_handle_setopt(handler, NULL, LRO_PROGRESSCB, progress_callback);
	     ssds_log(logDEBUG, "Progress callback is setted.\n");

	     // Prepare list of target
	     target = lr_packagetarget_new_v2(handler, inst->pkg_loc, DOWNLOAD_TARGET_INSTALL, 
					      LR_CHECKSUM_UNKNOWN, NULL, 0, inst->base_url, TRUE, 
					      progress_callback, inst->pkg_name, end_callback, NULL, &error);
	     package_list = g_slist_append(package_list, target);
  	}
  
	  // Download all packages        
	  ssds_log(logMESSAGE, "Downloading packages.\n");
	  return_status = lr_download_packages(package_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
  
 	 if(!return_status || error != NULL){
	      ssds_log(logERROR, "%d: %s\n", error->code, error->message);
	      g_error_free(error);
	      ssds_gc_cleanup();
	      return DOWNLOAD_ERROR;
	 }

	 ssds_log(logMESSAGE, "All packages were downloaded successfully.\n");

  	/*********************************************************/
  	/* Installing packages                                   */
  	/*********************************************************/

	  ssds_log(logMESSAGE, "Installing packages.\n"); 
	  for(GSList *elem = package_list; elem; elem = g_slist_next(elem))
	  {
	      char command[300];
	      LrPackageTarget *target = (LrPackageTarget *)elem->data;

	      if(!target->err)
	      {
        	  sprintf(command, "rpm --install --nodeps %s", target->local_path);
	          ssds_log(logMESSAGE,"Installing package: %s\n",(char *)target->cbdata);
//      	    system(command);
//		    unlink(target->local_path);
      	      }else{
	          ssds_log(logERROR, "Package Error: %s\n", target->err);
      	      }

  	  }
  
	  ssds_log(logMESSAGE, "All packages was installed correctly.\n\n\tPackage %s is ready to use.\n\n",answer_from_srv->name); 
  	  g_slist_free_full(package_list, (GDestroyNotify) lr_packagetarget_free);
	  ssds_free(answer_from_srv);
  }

  ssds_log(logSSDS, "End of client.\n\n");
    
  ssds_gc_cleanup();

  return EXIT_SUCCESS;
}
