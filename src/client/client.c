#include "client.h"

int ssds_get_new_id(int socket, char **id, char *arch, char *release)
{
	char *message;
	SsdsJsonCreate* json_gen = ssds_js_cr_init(GENERATE_ID);
        
        ssds_log(logDEBUG, "Inserted code %d into json.\n", GENERATE_ID);

        ssds_js_cr_gen_id(json_gen, arch, release);    // generate message for server
        ssds_log(logDEBUG, "Generated JSON for server with params: arch=%s, release=%s.\n", arch, release);

        ssds_log(logDEBUG, "Generating message string.\n");
        message = ssds_js_cr_to_string(json_gen);
        ssds_log(logDEBUG, "Message string generated: \t%s\n", message);

        ssds_log(logMESSAGE, "Sending message to server.\n");
        write(socket, message, strlen(message));
        ssds_log(logMESSAGE, "Message sent.\n");

	*id = NULL;
	//TODO: read answer from server
	ssds_free(json_gen);

	return OK;
}

int ssds_send_System_solv(int comm_sock, int data_sock, char *path)
{
  SsdsJsonCreate* json_msg = ssds_js_cr_init(SEND_SOLV), *json_solv = ssds_js_cr_init(SOLV_MORE_FRAGMENT);
  SsdsJsonRead* json_read = ssds_js_rd_init();

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
    return FILE_ERROR;
  }

  ssds_log(logDEBUG, "Preparing .solv variables.\n");

  char buffer[131072];
  char* msg_length;
  char* server_response;
  size_t bytes_read = 0;

  ssds_log(logDEBUG, "Sending @System.solv file.\n");
  while((bytes_read = fread(buffer, 1, 131072, f)) != 0)
  {
      ssds_js_cr_set_read_bytes(json_solv, (int) bytes_read);
      msg_length = ssds_js_cr_to_string(json_solv);  

      write(comm_sock, msg_length, strlen(msg_length));
      ssds_log(logDEBUG, "Command sent.\n");
      write(data_sock, buffer, bytes_read);
      ssds_log(logDEBUG, "Data sent.\n");

      ssds_free(json_solv);
      json_solv = ssds_js_cr_init(SOLV_MORE_FRAGMENT);
      
      server_response = sock_recv(comm_sock);
      ssds_js_rd_parse(server_response, json_read);
  
      if(ssds_js_rd_get_code(json_read) != ANSWER_OK)
      {
         ssds_log(logERROR, "%s\n", ssds_js_rd_get_message(json_read));
         return NETWORKING_ERROR;
      }
     
      ssds_free(json_read);
      json_read = ssds_js_rd_init();
  }

  ssds_js_cr_insert_code(json_msg, SOLV_NO_MORE_FRAGMENT);
  msg_output = ssds_js_cr_to_string(json_msg);
  write(comm_sock, msg_output, strlen(msg_output));
  ssds_log(logDEBUG, "Message sent.\n");

  return OK; 
}

int ssds_send_repo(ParamOptsCl* params, char *arch, char *release, int comm_sock, int action)
{
  
  ssds_log(logDEBUG, "Client repo info JSON creating.\n");

  SsdsJsonCreate *json_gen = ssds_js_cr_init(action);
  SsdsLocalRepoInfo* local_repo = ssds_repo_parse_init();

  ssds_log(logDEBUG, "Local repo info initialized.\n");

  // parsing local repo
  if(!ssds_parse_default_repo(local_repo))
  {
     return REPO_ERROR;
  }
  ssds_log(logDEBUG, "Local repo is parsed.\n");
  
  ssds_get_repo_urls(local_repo, json_gen, arch, release);
  ssds_log(logDEBUG, "Getting repo urls.\n");
  
  ssds_log(logDEBUG, "Loop thrue required packages.\n");
  for(int i = 0; i < params->pkg_count; i++)
  {
     char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
     ssds_js_cr_add_package(json_gen, pkg);
     ssds_log(logDEBUG, "Added %s package as %d in order.\n", pkg, i+1);
  }
  ssds_log(logDEBUG, "Loop is done.\n");
  
  char* repo_output;
  ssds_log(logDEBUG, "Generating output message with repo info to server.\n");
  repo_output = ssds_js_cr_to_string(json_gen);
  ssds_log(logDEBUG, "Message generated.\n\n%s\n\n", repo_output);

  /***********************************************************/
  /* Sending repo info to server                             */
  /***********************************************************/
  ssds_log(logMESSAGE, "Sending message with repo info to server.\n");
  write(comm_sock, repo_output, strlen(repo_output));
  ssds_log(logDEBUG, "Message sent.\n");

  return OK;
}

int ssds_check_repo(int socket, char **message)
{
	char *buffer = sock_recv(socket);
	SsdsJsonRead *json = ssds_js_rd_init();
        int rc = -1;

        ssds_js_rd_parse(buffer, json);

        rc = ssds_js_rd_get_code(json);
      
        if(rc != ANSWER_OK)
        {
           *message = ssds_js_rd_get_message(json);
        }

        return rc;
}
