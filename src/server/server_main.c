/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "server.h"


#if 0
void session(ip::tcp::socket sock,boost::system::error_code ec){
	try {
		while(42)
		{
			char data[1024];
			size_t lengthsome = sock.read_some(boost::asio::buffer(data), ec);
			 if (ec == boost::asio::error::eof) {
				 std::cout << "Connection closed." << "\n";
		 	 	 break; // Connection closed cleanly by peer.
			 }
			  else if (ec)
			      throw boost::system::system_error(ec);
		      	  boost::asio::write(sock, boost::asio::buffer(data, lengthsome));
		 }//while
		}//try
	 catch (std::exception& e) {
		    std::cerr << "Exception in thread: " << e.what() << "\n";
	 }
}
#endif

int main(int argc, char* argv[]) {
  /*******************************************************************/
  /* Setting up garbage collector and setting callback functions */
  /*******************************************************************/
  /*ssds_gc_init();
  signal(SIGINT, ssds_signal_handler);
  signal(SIGBUS, ssds_signal_handler);
  signal(SIGSEGV, ssds_signal_handler);
  signal(SIGTERM, ssds_signal_handler);
  */
  ssds_log(logDEBUG, "Server started.\n");

  parse_params_srv(argc, argv);
    
  ssds_log(logDEBUG, "Params parsed.\n");
  /*************************************************************************
  * 
  * 	Establishing port, socket etc for the communication
  * 
  *************************************************************************/
  int comm_desc, comm_sock, data_desc, data_sock, enable = 1;
  char* client_ip;

  comm_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  data_desc=socket(AF_INET, SOCK_STREAM, 0);//AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  int yes = 1;

  setsockopt(comm_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(data_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  
  if(comm_desc == -1)
  {
    ssds_log(logERROR, "Server encountered an error when creating socket for communication.\n");
   // ssds_gc_cleanup();
    return 1;
  }
  if(data_desc == -1)
  {
    ssds_log(logERROR, "Server encountered an error when creating socket for sending data.\n");
   // ssds_gc_cleanup();
    return 1;
  }
  
  ssds_log(logDEBUG, "Socket ready.\n");
  
  struct sockaddr_in server_comm, server_data, client_comm, client_data;

  server_comm.sin_family = AF_INET;
  server_comm.sin_addr.s_addr = INADDR_ANY;
  server_comm.sin_port = htons(2345);

  server_data.sin_family = AF_INET;
  server_data.sin_addr.s_addr = INADDR_ANY;
  server_data.sin_port = htons(2346);

  if(bind(comm_desc, (struct sockaddr*)&server_comm, sizeof(server_comm)) < 0)
  {
    ssds_log(logERROR, "Server wasn't able to bind with communication socket.\n");
   // ssds_gc_cleanup();
    return 1;
  }
 
  if(setsockopt(comm_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(enable)) < 0){
    ssds_log(logERROR, "Server wasn't able to set communication socket option.\n");
   // ssds_gc_cleanup();
    return 1;
  }

  if(bind(data_desc, (struct sockaddr*)&server_data, sizeof(server_data)) < 0)
  {
    ssds_log(logERROR, "Server wasn't able to bind with data socket\n");
   // ssds_gc_cleanup();
    return 1;
  }
  
  if(setsockopt(data_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(enable)) < 0){
    ssds_log(logERROR, "Server wasn't able to set data socket option.\n");
   // ssds_gc_cleanup();
    return 1;
  }

  ssds_log(logINFO, "Server started. Waiting for incoming connections.\n");
  
  if(listen(comm_desc, 5) != 0)
  {
    ssds_log(logERROR, "Listen failed on communication socket on server.\n");
   // ssds_gc_cleanup();
    return 1;
  }

  if(listen(data_desc, 5) != 0)
  {
    ssds_log(logERROR, "Listen failed on data socket on server.\n");
   // ssds_gc_cleanup();
    return 1;
  }
  
  int comm_addr_len = sizeof(server_comm);
  int data_addr_len = sizeof(server_data);
  char* buf;
  int client_finished = 0;
  
  while(1)
  {
    client_finished = 0;
    if((comm_sock = accept(comm_desc, (struct sockaddr *) &client_comm, (socklen_t*)&comm_addr_len)) < 0)
    {
      ssds_log(logERROR, "Accept connection has failed.\n");
  //    ssds_gc_cleanup();
      return 1;
    }
    if((data_sock = accept(data_desc, (struct sockaddr *) &client_data, (socklen_t*)&data_addr_len)) < 0)
    {
      ssds_log(logERROR, "Accept on data socket has failed");
     // ssds_gc_cleanup();
      return 1;
    }
    client_ip = inet_ntoa(client_comm.sin_addr);
    ssds_log(logMESSAGE, "Connection accepted from ip address %s\n", client_ip);

    //reading messages in loop and resolving them
    while(!client_finished)
    {
        buf = sock_recv(comm_sock);
 
        if(buf == NULL)
        {
          ssds_log(logERROR, "Recieving of message has failed.\n");
     //     ssds_gc_cleanup();
          return 1;
        }        

        SsdsJsonRead* json = ssds_json_read_init();
        if(!ssds_read_parse(buf, json))//parse incoming message
        {
          ssds_log(logERROR, "False data recieved from %s. Client rejected.\n", client_ip);
          continue;
        }

        ssds_log(logDEBUG, "%s\n\n", buf);
        switch(ssds_read_get_code(json))
        {
        case 10:
            ssds_log(logDEBUG, "Got message with code 10 (client is going to send @System.solv file).\n");
            FILE * f = fopen("@System.solv","wb"); //for now the file is in the same directory as server;
            if(f == NULL)
            {
                ssds_log(logERROR,"Error while creating @System.solv file.\n");
       //         ssds_gc_cleanup();
                return 1;
            }
            char* data_buffer;
            char* comm_buffer;
            char* end_ptr;
            size_t bytes_written, bytes_to_write;
            int i = 0;
            write(comm_sock, "OK", strlen("OK"));
            while(1)
            {
                comm_buffer = sock_recv(comm_sock);
                if(strcmp(comm_buffer,"@System.solv file sent") == 0)
                {
                    break;
                }
                data_buffer = sock_recv(data_sock);

                bytes_to_write = strtol(comm_buffer, &end_ptr, 10);
                bytes_written = fwrite(data_buffer ,1 ,bytes_to_write ,f);

                write(comm_sock, "OK", strlen("OK"));
                ssds_log(logDEBUG, "Writing %d bytes to @System.solv file for the %d. time.\n", bytes_written, ++i);
            }
            fclose(f);
            ssds_log(logDEBUG, "Finished writing @System.solv file.\n");
            break;

        case 123:

            /* Dependency solving part */
            ssds_log(logMESSAGE, "\n\nDEPENDENCY SOLVING.\n\n");

            SsdsPkgInfo* pkgs = ssds_read_pkginfo_init();
            ssds_read_get_packages(pkgs, json);

            ssds_log(logDEBUG, "Packages parsed. Packages from client:\n");
            for(int i = 0; i < pkgs->length; i++)
            {
              ssds_log(logDEBUG, "\t%s\n", pkgs->packages[i]);
            }

            ssds_log(logDEBUG, "Getting repo info from client.\n");

            SsdsRepoInfoList* list = ssds_read_list_init();
            ssds_read_repo_info(json, list);

            guint len = g_slist_length(list->repoInfoList);

            ssds_log(logDEBUG, "Repositories, count: %d: \n", len);
            for(unsigned int i = 0; i < len; i++)
            {
              SsdsRepoInfo* info = (SsdsRepoInfo*)g_slist_nth_data(list->repoInfoList, i);
              ssds_log(logDEBUG, "\t%d: %s\n", i, info->name);
            }

            SsdsRepoMetadataList* meta_list = ssds_repo_metadata_init();
            ssds_locate_repo_metadata(/*json, */list, meta_list);

            //TODO - change this so that it doesn't need to be created manually
            HySack sack;
            #if VERSION_HAWKEY
              sack = hy_sack_create(NULL, NULL, NULL, NULL, HY_MAKE_CACHE_DIR);
            #else
              sack = hy_sack_create(NULL, NULL, NULL, HY_MAKE_CACHE_DIR);
            #endif

            hy_sack_load_system_repo(sack, NULL, HY_BUILD_CACHE);
            HySack* sack_p = &sack;
            ssds_fill_sack(sack_p, meta_list);

            SsdsJsonCreate* answer = ssds_js_cr_init();
            ssds_dep_answer(json, answer, sack_p);
            
            ssds_js_dump(answer);
            char* message = ssds_js_to_string(answer);
//             printf("%s\n\n", message);
            write(comm_sock, message, strlen(message));
            client_finished = 1;

            break;
        }
    }
  //ssds_gc_remove_socket(comm_sock);
  close(comm_sock);
  //ssds_gc_remove_socket(data_sock);
  close(data_sock);
  }
  //ssds_solving::solve solveHandler;

  
  
//   HySack* sack = ssds_solve_init();
//   ssds_fill_sack(sack, list);

#if 0
  try {
    ip::tcp::iostream streams("");
    while(42) {
      ip::tcp::socket sock(ios);
      acceptor_.accept(sock); //second argument can be error handler

      std::cout<<"Some connection was accepted" << std::endl;

      mainserver.process_connection(sock);
#if 0
      int64_t size;
      boost::asio::read(sock,boost::asio::buffer(&size,sizeof(size)));

      std::vector<char> buf(size);
      size_t len = sock.read_some(buffer(buf), ec);
      std::string input_message = std::string(buf.begin(), buf.end());

      std::cout<< "Message has " << len << " characters." << std::endl;

      solving
      std::string message = solvePoint.answer(input_message);
#endif	

      sock.close();

      //far future		    
      //std::thread(session, std::move(sock).std::move(ec)).detach();

    }
  } /*handling exceptions*/
  catch (std::exception& e){
    //std::ostringstream os;
    //os << "Server: "<< e.what();
    /*std::cerr*///my_log.add_log(logERROR,e.what());
    ssds_log("pokus", logERROR);
    //ssds_log(e.what(), logERROR); //might not work since e.what returns virtual const char*
  }

#endif
  return 0;
}
