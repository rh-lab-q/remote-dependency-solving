#include "server.h"

int ssds_server_init(int *comm_desc, int *comm_addr_len)
{

  /*************************************************************************/
  /* Initialization of server                                              */
  /*************************************************************************/
  
  int enable = 1, status = OK;
  struct sockaddr_in server_comm;

  *comm_desc = ssds_socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  
  setsockopt(*comm_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

  if(*comm_desc == -1)
  {
    ssds_log(logERROR, "Server encountered an error when creating socket for communication.\n");
    status = SOCKET_ERROR;
    goto initEnd;
  }

  ssds_log(logDEBUG, "Socket ready.\n");

  long int comm_port;
  read_srv_cfg(&comm_port);

  server_comm.sin_family = AF_INET;
  server_comm.sin_addr.s_addr = INADDR_ANY;
  server_comm.sin_port = htons(comm_port);

  if(bind(*comm_desc, (struct sockaddr*)&server_comm, sizeof(server_comm)) < 0)
  {
    ssds_log(logERROR, "Server wasn't able to bind with communication socket.\n");
    status = SOCKET_ERROR;
    goto initEnd;
  }

  if(setsockopt(*comm_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(enable)) < 0){
    ssds_log(logERROR, "Server wasn't able to set communication socket option.\n");
    status = SOCKET_ERROR;
    goto initEnd;
  }

  ssds_log(logINFO, "Server set up. Waiting for incoming connections.\n");

  if(listen(*comm_desc, 5) != 0)
  {
    ssds_log(logERROR, "Listen failed on communication socket on server.\n");
    status = SOCKET_ERROR;
    goto initEnd;
  }

  *comm_addr_len = sizeof(server_comm);
 
initEnd:
  return status;
}

int ssds_server_accept_connection(int comm_desc, int comm_addr_len)
{
  int socket, status = OK, client_end = 0;
  struct sockaddr_in client_comm;    

  if((socket = ssds_accept(comm_desc, (struct sockaddr *) &client_comm, (socklen_t*)&comm_addr_len)) < 0)
  {
    ssds_log(logERROR, "Accept connection has failed.\n");
    status = SOCKET_ERROR;
    goto acceptEnd;
  }
 
  char *client_ip = inet_ntoa(client_comm.sin_addr);
  ssds_log(logMESSAGE, "Connection accepted from ip address %s\n", client_ip);

  // FORK HERE ?

  while(!client_end){
	status = ssds_server_process(socket, client_ip, &client_end);
	if(status != OK) goto acceptEnd;
  }

acceptEnd:
  ssds_close(socket);
  ssds_log(logDEBUG, "End of communication with client.\n");

  return status;
}

int ssds_server_process(int socket, char *client_ip, int *client_end)
{
  int status = OK;
  char *buf = sock_recv(socket), *msg;
  SsdsJsonCreate *json_send = ssds_js_cr_init(ANSWER_OK);
  SsdsJsonRead *json_read = ssds_js_rd_init();

  if(buf == NULL)
  {
    ssds_log(logERROR, "Recieving of message has failed.\n");
    status = NETWORKING_ERROR;
    goto processEnd;
  }

  if(!ssds_js_rd_parse(buf, json_read))//parse incoming message
  {
     ssds_log(logERROR, "False data recieved from %s. Client rejected.\n", client_ip);
     goto processEnd;
  }

  ssds_log(logDEBUG, "%s\n\n", buf);
	
  switch(ssds_js_rd_get_code(json_read))
  {
        case SEND_SOLV:
            ssds_log(logDEBUG, "Got message with code %d (client is going to send @System.solv file).\n", SEND_SOLV);

            int bytes_to_write = ssds_js_rd_get_read_bytes(json_read);
            
 	    FILE * f = fopen("@System.solv","wb"); //for now the file is in the same directory as server;
            if(f == NULL)
            {
                ssds_log(logERROR,"Error while creating @System.solv file.\n");
                ssds_js_cr_insert_code(json_send, ANSWER_ERROR);
		ssds_js_cr_set_message(json_send, "Error while creating @System.solv file on server side.");
                msg = ssds_js_cr_to_string(json_send);
		secure_write(socket, msg, strlen(msg));
                status = FILE_ERROR;
                goto processEnd;
            }

            char* comm_buffer;
            int bytes_written = 0, bytes_read;

            while(1)
            {
                bytes_read = sock_solv_recv(socket, &comm_buffer);
                if(fwrite(comm_buffer ,1 ,bytes_read ,f) != bytes_read){
			ssds_log(logERROR, "Error while writing to @System.solv file.\n");
			ssds_js_cr_insert_code(json_send, ANSWER_ERROR);
			ssds_js_cr_set_message(json_send, "Error while writing to @System.solv file on server side.\n");
			*client_end = 1;
			break;
		}
		ssds_free(comm_buffer);
		bytes_written += bytes_read;
	
		if(bytes_read < 0){
			ssds_js_cr_set_message(json_send, "Error while reading data fom client.");
			*client_end = 1;
			break;
		}
		if(bytes_written == bytes_to_write)
                {
                  break;
                }

                ssds_log(logDEBUG, "%.0f %% of @System.solv file is written.\n", 
				   (((double)bytes_written/(double)bytes_to_write)*100));
            }
            msg = ssds_js_cr_to_string(json_send);
            secure_write(socket, msg, strlen(msg));
            fclose(f);
	    ssds_log(logDEBUG, "Finished writing @System.solv file.\n");
            break;

        case GET_INSTALL:
				case GET_UPDATE:
				case GET_UPDATE_ALL:
ssds_js_rd_get_count(json_read, "pokus");
            /* Checking repo files */
            /* TODO here should be checking of cached repo files !!! */
            ssds_log(logDEBUG, "Repo files checking.\n");

            msg = ssds_js_cr_to_string(json_send);
            secure_write(socket, msg, strlen(msg));

            /* Dependency solving part */
            ssds_log(logMESSAGE, "\n\nDEPENDENCY SOLVING.\n\n");

						char** pkgs = NULL;
						int pkg_count = 0;
            //SsdsPkgInfo* pkgs = ssds_js_rd_pkginfo_init();
						if(ssds_js_rd_get_code(json_read) != GET_UPDATE_ALL)
						{
							pkg_count = ssds_js_rd_get_count(json_read, "req_pkgs");
							pkgs = (char**)malloc((pkg_count+1)*sizeof(char*));
							ssds_js_rd_get_packages(pkgs, json_read);

							ssds_log(logDEBUG, "Packages parsed. Packages from client:\n");
							for(int i = 0; i < pkg_count; i++)
							{
								ssds_log(logDEBUG, "\t%s\n", pkgs[i]);
							}
						}
            ssds_log(logDEBUG, "Getting repo info from client.\n");

            SsdsRepoInfoList* list = ssds_js_rd_list_init();
            ssds_js_rd_repo_info(json_read, list);

            guint len = g_slist_length(list->repoInfoList);

            ssds_log(logDEBUG, "Repositories, count: %d: \n", len);
            for(unsigned int i = 0; i < len; i++)
            {
              SsdsRepoInfo* info = (SsdsRepoInfo*)g_slist_nth_data(list->repoInfoList, i);
              ssds_log(logDEBUG, "\t%d: %s\n", i, info->name);
            }

            SsdsRepoMetadataList* meta_list = ssds_repo_metadata_init();
            ssds_locate_repo_metadata(/*json, */list, meta_list);

            HySack sack;
            #if VERSION_HAWKEY
                sack = hy_sack_create(NULL, NULL, NULL, NULL, HY_MAKE_CACHE_DIR);
            #else
                sack = hy_sack_create(NULL, NULL, NULL, HY_MAKE_CACHE_DIR);
            #endif
            
            hy_sack_load_system_repo(sack, NULL, HY_BUILD_CACHE);
            HySack* sack_p = &sack;
            ssds_fill_sack(sack_p, meta_list);
            
						if(ssds_dep_query(pkgs, json_send, sack_p, ssds_js_rd_get_code(json_read), pkg_count) == -1)
						{
							ssds_js_cr_insert_code(json_send, ANSWER_NO_DEP);
							ssds_js_cr_set_message(json_send, "Dependencies for requested package were not resolved. Package cannot be installed\n");
						}
						else
							ssds_js_cr_insert_code(json_send, ANSWER_OK);
//             ssds_dep_answer(json_read, json_send, sack_p);

            ssds_js_cr_dump(json_send);
            msg = ssds_js_cr_to_string(json_send);
//             printf("%s\n\n", msg);
            secure_write(socket, msg, strlen(msg));
            *client_end = 1;
            break;

        
        case GET_DEPENDENCY:
        case GET_ERASE:
           /* Checking repo files */
           /* TODO here should be checking of cached repo files !!! */
           ssds_log(logDEBUG, "Repo files checking.\n");

           msg = ssds_js_cr_to_string(json_send);
           secure_write(socket, msg, strlen(msg));
           *client_end = 1;
           break;

       default: //client_end = 1;
           break;
  }
//
processEnd:
  ssds_free(json_read);
  ssds_free(json_send);
  return status;
}
