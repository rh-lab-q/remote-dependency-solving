#include "client.h"

// #define VERSION_HAWKEY @HKY_22@
//for debugging
//#define DEBUG

int main(int argc, char* argv[]){

  /*******************************************************************/
  /* Setting up garbage collector and setting callback functions     */
  /*******************************************************************/
  uid_t uid = geteuid();
  if(uid != 0)
  {
    ssds_log(logERROR, "This program has to be run under the root user otherwise no packages can be installed, erased or updated.\n");
    return ROOT_ERROR;
  }
  
  ssds_gc_init();
  signal(SIGINT, ssds_signal_handler);
  signal(SIGBUS, ssds_signal_handler);
  signal(SIGSEGV, ssds_signal_handler);
  signal(SIGTERM, ssds_signal_handler);

  /*******************************************************************/
  /* Parsing parameters 					     */
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
 
  /********************************************************************/
  /* Load configuration                                               */
  /********************************************************************/

  char *server_address, *id;
  long int comm_port, data_port;
  read_cfg(&id, &server_address, &comm_port, &data_port);

  /********************************************************************/
  /*  Getting architecture, release and path to @System.solv          */ 
  /********************************************************************/
  char path[100];
  char *arch = NULL, *release = NULL;
  ssds_resolve_dependency_file_path(path, &arch, &release);

  /********************************************************************/
  /* Networking part - connecting to server                           */
  /********************************************************************/
  ssds_log(logDEBUG, "Network part.\n");

  int data_sock, comm_sock, status;

  status = client_connect(&data_sock, &comm_sock, server_address, data_port, comm_port);

  if(status != OK)
  {
        ssds_gc_cleanup();
        return status;
  }

  ssds_log(logDEBUG, "Data socket: %d   Communication socket: %d.\n", data_sock, comm_sock);


  /********************************************************************/
  /* JSON send and read initialization                                */
  /********************************************************************/
  
  ssds_log(logDEBUG, "Client JSON creating. Package count %d.\n", params->pkg_count);
  
  SsdsJsonCreate* json_gen = ssds_js_cr_init(SEND_REPO);
  ssds_log(logDEBUG, "Json create initialized on %d. Package count %d.\n", json_gen, params->pkg_count);

  SsdsJsonRead* json_read = ssds_js_rd_init();
  ssds_log(logDEBUG, "Json read initialized on %d. Package count %d.\n", json_read, params->pkg_count);
                            
  /********************************************************************/
  /* Checking client ID                                               */
  /********************************************************************/

  if(id == NULL)
  {     
        //char *id;
        //int status = ssds_get_new_id(comm_sock, &id);
        char *message;
	ssds_js_cr_insert_code(json_gen, GENERATE_ID); //insert code into json
	ssds_log(logDEBUG, "Inserted code %d into json.\n", GENERATE_ID);

	ssds_js_cr_gen_id(json_gen, arch, release);    // generate message for server
        ssds_log(logDEBUG, "Generated JSON for server with params: arch=%s, release=%s.\n", arch, release);

        ssds_log(logDEBUG, "Generating message string.\n");
        message = ssds_js_cr_to_string(json_gen);
        ssds_log(logDEBUG, "Message string generated: \t%s\n", message);

  	ssds_log(logMESSAGE, "Sending message to server.\n");
	write(comm_sock, message, strlen(message));
	ssds_log(logMESSAGE, "Message sent.\n");

        // TODO: reading answer with generated ID from server
        ssds_free(json_gen);
        json_gen = ssds_js_cr_init(SEND_REPO);
	// ssds_send_System_solv(comm_sock, data_sock, path);
  }


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
		rpmts ts;
		int rc, noticeFlags = 0;

	        rpmReadConfigFiles(NULL, NULL);
        	ts = rpmtsCreate();
	        rpmtsSetRootDir(ts, NULL);
		
		for(int i = 0; i < params->pkg_count; i++){
 		    char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
                    int res = ssds_add_to_erase(ts, pkg);
		    if(res != OK){
			ssds_log(logERROR, "Unable to erase requested package.\n");
			ssds_gc_cleanup();
			return ERASE_ERROR;
		    }   
		}

		rpmprobFilterFlags flag = 0;

        	noticeFlags |= INSTALL_LABEL | INSTALL_HASH;
	        rpmtsSetNotifyCallback(ts, rpmShowProgress,(void *) noticeFlags);

        	rc = rpmtsRun(ts, NULL, flag);
	        if(rc == 0){
        	       ssds_log(logMESSAGE, "All packages was correctly erased.\n");
		       rc = OK;
	        }else{
                       ssds_log(logERROR, "Erasing ends with code %d\n", rc);
		       rc = ERASE_ERROR;
	        }

        	rpmtsClean(ts);
	        rpmtsFree(ts);
		ssds_gc_cleanup();
		return rc;

	case PAR_CHK_DEP: 
		ssds_log(logMESSAGE, "Dependency check of packages was selected.\n");
		break;
  } 
  
 // check_for_missing_repos(); //check if client misses some repositories

  /*******************************************************************/
  /* Creating repo info JSON                                         */
  /*******************************************************************/
  ssds_log(logDEBUG, "Client repo info JSON creating. Package count %d.\n", params->pkg_count);

  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();
  ssds_log(logDEBUG, "Local repo info initialized on %d. Package count %d.\n", local_repo, params->pkg_count);
  
  ssds_js_cr_insert_code(json_gen, SEND_REPO); //insert code into json
  ssds_log(logDEBUG, "Inserted code %d into json. Package count %d.\n", SEND_REPO, params->pkg_count);
  
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

  	// required variables for downloading
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
	     ssds_log(logDEBUG, "Array of URLs is set.\n");
	     lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
	     ssds_log(logDEBUG, "Repo type is set.\n");
	     lr_handle_setopt(handler, NULL, LRO_PROGRESSCB, progress_callback);
	     ssds_log(logDEBUG, "Progress callback is set.\n");

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
	  
	 // required variables for rpmlib
         rpmts ts;

         rpmReadConfigFiles(NULL, NULL);
         ts = rpmtsCreate();
         rpmtsSetRootDir(ts, NULL);
                                          
	 ssds_log(logMESSAGE, "Installing packages.\n"); 
	 for(GSList *elem = package_list; elem; elem = g_slist_next(elem))
	 {
	      LrPackageTarget *target = (LrPackageTarget *)elem->data;

	      if(!target->err)
	      {
        	ssds_add_to_transaction(ts, target->local_path, SSDS_INSTALL);
      	      }else{
	          ssds_log(logERROR, "Package Error: %s\n", target->err);
      	      }

  	 }

	 rpmprobFilterFlags flag = 0;
	 
	 int nf = 0, rc;
	 
	 nf |= INSTALL_LABEL | INSTALL_HASH;
         rpmtsSetNotifyCallback(ts, rpmShowProgress,(void *) nf);
	 
	 rc = rpmtsRun(ts, NULL, flag);
         if(rc == 0){
                ssds_log(logMESSAGE, "All packages was installed correctly.\n\n\tPackage %s is ready to use.\n\n",answer_from_srv->name);
         }else{
		ssds_log(logWARNING, "Install ends with code %d\n", rc);
	 }
         
	 rpmtsClean(ts);
         rpmtsFree(ts);
	 g_slist_free_full(package_list, (GDestroyNotify) lr_packagetarget_free);
	 ssds_free(answer_from_srv);
  }

  ssds_log(logSSDS, "End of client.\n\n");
    
  ssds_gc_cleanup();

  return EXIT_SUCCESS;
}
