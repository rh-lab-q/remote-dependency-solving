#include "client.h"

// #define VERSION_HAWKEY @HKY_22@
//for debugging
//#define DEBUG

/* Client plan:
 *  check id (if NULL - ask for it from server by message) TODO: gen. ID on server and store it into cfg gile
 *  connect to server - done
 *  if new id - send @System.solv - done
 *  parse repo - done 
 *  send repo info to server - done
 *  if some repo on client side missing - read warning from server TODO: checking repo missing on server side
 *  send message with requested operation and package names to server TODO: add update and erase option in json
 *  read dep result - done
 *  print dep result to user TODO: print that
 *  
 *  ----- only in update and install -----
 *  ask if (update/install)/download/cancel 
 *  download packages - done
 *  install/update/erase them - done
 */

int main(int argc, char* argv[]){

  /*******************************************************************/
  /* Check root rights                                               */
  /*******************************************************************/
  uid_t uid = geteuid();
  if(uid != 0)
  {
    ssds_log(logERROR, "This program has to be run under the root user otherwise no packages can be installed, erased or updated.\n");
    return ROOT_ERROR;
  }
  
  /*******************************************************************/
  /* Client return code variable                                     */
  /*******************************************************************/
  int status = OK;
  
  /*******************************************************************/
  /* Setting up garbage collector and setting callback functions     */
  /*******************************************************************/
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
    status = PARAMS_ERROR;
    goto end;
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
  char *arch = NULL, *release = NULL, *message = NULL;
  ssds_resolve_dependency_file_path(path, &arch, &release);

  /********************************************************************/
  /* Networking part - connecting to server                           */
  /********************************************************************/
  ssds_log(logDEBUG, "Network part.\n");

  int data_sock, comm_sock;

  status = client_connect(&data_sock, &comm_sock, server_address, data_port, comm_port);

  if(status != OK) goto end;
 
  ssds_log(logDEBUG, "Data socket: %d   Communication socket: %d.\n", data_sock, comm_sock);

  /********************************************************************/
  /* Checking client ID                                               */
  /********************************************************************/

  if(id == NULL)
  {     
        status = ssds_get_new_id(comm_sock, &id, arch, release);
	if(status != OK) goto end;

	status = ssds_send_System_solv(comm_sock, data_sock, path);
        if(status != OK) goto end;

  }


  // this switch is now just for info about selected command
  switch(params->command)
  {

	case PAR_INSTALL: 
		ssds_log(logMESSAGE, "Installation of packages was selected.\n");

                status = ssds_send_repo(params, arch, release, comm_sock, GET_INSTALL);
		if(status != OK) break; 
				
                if(ssds_check_repo(comm_sock, &message) != ANSWER_OK)
		{
			ssds_log(logWARNING,"%s\n", message);
		}

		status = ssds_answer_process(comm_sock, GET_INSTALL);
		break;

	case PAR_UPDATE: 
		ssds_log(logMESSAGE, "Update of packages was selected.\n");
	        ssds_log(logERROR, "Update option has not been implemented yet.\n");

		/*status = ssds_send_repo(params, arch, release, comm_sock, GET_UPDATE);
                if(status != OK) break;

                if(ssds_check_repo(comm_sock, &message) != ANSWER_OK)
                {
                        ssds_log(logWARNING,"%s\n", message);
                }

                status = ssds_answer_process(comm_sock, GET_UPDATE);*/

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
                    rc = ssds_add_to_erase(ts, pkg);
		    if(rc != OK){
			ssds_log(logERROR, "Unable to erase requested package.\n");
			status = ERASE_ERROR;
			goto end;
		    }   
		}

		rpmprobFilterFlags flag = 0;

        	noticeFlags |= INSTALL_LABEL | INSTALL_HASH;
	        rpmtsSetNotifyCallback(ts, rpmShowProgress,(void *) noticeFlags);

        	rc = rpmtsRun(ts, NULL, flag);
	        if(rc == 0){
        	       ssds_log(logMESSAGE, "All packages was correctly erased.\n");
		       status = OK;
	        }else{
                       ssds_log(logERROR, "Erasing ends with code %d\n", rc);
		       status = ERASE_ERROR;
	        }

        	rpmtsClean(ts);
	        rpmtsFree(ts);

		/*status = ssds_send_repo(params, arch, release, comm_sock, GET_ERASE);
                if(status != OK) break;

                if(ssds_check_repo(comm_sock, &message) != ANSWER_OK)
                {
                        ssds_log(logWARNING,"%s\n", message);
                }

                status = ssds_answer_process(comm_sock, GET_ERASE);*/

		break;

	case PAR_CHK_DEP: 
		ssds_log(logMESSAGE, "Dependency check of packages was selected.\n");
		ssds_log(logERROR, "Dependency check option has not been implemented yet.\n");
		break;
  } 
  
end:
  ssds_log(logSSDS, "End of client.\n\n");
    
  ssds_gc_cleanup();

  return status;
}
