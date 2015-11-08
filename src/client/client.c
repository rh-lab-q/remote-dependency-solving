/**
 * Server side dependency solving 
 * transfer of dependency solving from local machine to server when installing new packages
 * 
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký, Walter Scherfel, Šimon Matěj
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
  secure_write(socket, message, strlen(message));
  ssds_log(logMESSAGE, "Message sent.\n");

  *id = NULL;
  //TODO: read answer from server
  ssds_free(json_gen);

  return OK;
}

int ssds_send_file(int socket, int type, char *path)
{
  SsdsJsonCreate* json_msg = ssds_js_cr_init(type);

  char* msg_output;

  /***********************************************************/
  /* Sending file                                            */
  /***********************************************************/
  ssds_log(logDEBUG, "Path to file : %s\n",path);
  FILE * f;
  f = fopen(path,"rb");

  ssds_log(logDEBUG, "Opening file.\n");
  if(f == NULL)
  {
    ssds_log(logERROR,"Error while opening file %s.\n", path);
    return FILE_ERROR;
  }

  ssds_log(logDEBUG, "Getting size of file.\n");

  fseek(f, 0, SEEK_END);
  ssize_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  ssds_log(logDEBUG, "Size of file is %d.\n", size);
  ssds_js_cr_set_read_bytes(json_msg, (int) size);
  msg_output = ssds_js_cr_to_string(json_msg);
  secure_write(socket, msg_output, strlen(msg_output));
  ssds_log(logDEBUG, "Preparing variables.\n");

  char buffer[131072];
  size_t bytes_read = 0;

  ssds_log(logDEBUG, "Sending file.\n");
  while((bytes_read = fread(buffer, 1, 131072, f)) != 0)
  {
      secure_write(socket, buffer, bytes_read);
      ssds_log(logDEBUG, "Data sent.\n");
  }
  ssds_free(json_msg);

  char *buff = sock_recv(socket);
  
  SsdsJsonRead *json = ssds_js_rd_init();
  
  ssds_js_rd_parse(buff, json);

  int rc = ssds_js_rd_get_code(json);

  if(rc != ANSWER_OK)
  {
     ssds_log(logERROR, "Message from server: %s\n", ssds_js_rd_get_message(json));
     rc = NETWORKING_ERROR;
  }else{
     rc = OK;
  }
  ssds_free(buff);

  return rc; 
}

int compare_files(char *fileOne, char *fileTwo){
  
  FILE *f1, *f2;
  f1 = fopen(fileOne,"r");
  f2 = fopen(fileTwo,"r");

  ssds_log(logDEBUG, "Opening files for compare.\n");
  if(f1 == NULL)
  {
    ssds_log(logERROR,"Error while opening file %s.\n", fileOne);
    return FILE_ERROR;
  }

  if(f2 == NULL)
  {
    ssds_log(logERROR,"Error while opening file %s.\n", fileTwo);
    return FILE_ERROR;
  }
  
  char ch1 = fgetc(f1), 
       ch2 = fgetc(f2);
  
  while((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2))
  {
      ch1 = fgetc(f1), 
      ch2 = fgetc(f2);
  }
  
  if(ch1 == ch2)
  {
     ssds_log(logDEBUG, "Files are identical.\n");
     rc = OK;
  }else{
     ssds_log(logDEBUG, "Files are different.\n");
     rc = FILE_ERROR;
  }

  fclose(f1);
  fclose(f2);
  
  return rc; 
}

int copy_file(char *source, char *destination)
{
  FILE *s, *d;
  s = fopen(source,"r");
  d = fopen(destination,"w");

  ssds_log(logDEBUG, "Opening files for copy.\n");
  if(s == NULL)
  {
    ssds_log(logERROR,"Error while opening file %s.\n", source);
    return FILE_ERROR;
  }

  if(d == NULL)
  {
    ssds_log(logERROR,"Error while opening file %s.\n", destination);
    return FILE_ERROR;
  }
  
  char ch;
  
  while((ch = fgetc(source)) != EOF)
  {
      fputc(ch, d);
  }
  
  fclose(s);
  fclose(d);
  
  return OK; 
}

int ssds_send_repo(ParamOptsCl* params, char *arch, char *release, int socket, int action)
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
  secure_write(socket, repo_output, strlen(repo_output));
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

int ssds_answer_process(int socket, int action)
{
	ssds_log(logMESSAGE, "Waiting for answer from server.\n");
	
	char *buf = sock_recv(socket);
	SsdsJsonRead *json = ssds_js_rd_init();
	int rc = -1;
	
	ssds_log(logDEBUG, "Checking answer.\n");
	
	if(buf == NULL)
	{
		ssds_log(logERROR, "Error while recieving data\n");
		rc = NETWORKING_ERROR;
		goto End;
	}
	ssds_log(logDEBUG, "Some answer has been delivered.\n\n%s\n\n", buf);
	
	// parse response
	ssds_log(logDEBUG, "Parsing answer.\n");
	
	if(!ssds_js_rd_parse(buf, json))
	{
		ssds_log(logERROR, "Error while parsing answer from the server\n");
		rc = JSON_ERROR;
		goto End;
	}
	
	rc = ssds_js_rd_get_code(json);
	
	if(rc == ANSWER_WARNING)
	{
	   	ssds_log(logWARNING,"%s\n", ssds_js_rd_get_message(json));
	   	goto End;//TODO - asi vymyslet co s warningama protoze warningem by to koncit nemuselo ten program
	}
	
	if(rc == ANSWER_ERROR)
	{
		ssds_log(logERROR,"%s\n", ssds_js_rd_get_message(json));
	   	goto End;
	}
	
	if(rc == ANSWER_NO_DEP)
	{
		ssds_log(logERROR, "Answer no dep.\n");
		goto End;
	}
	
  	int num_install = ssds_js_rd_get_count(json, "install"),
		num_update = ssds_js_rd_get_count(json, "upgrade"),
		num_erase = ssds_js_rd_get_count(json, "erase"),
		num_obsolete = ssds_js_rd_get_count(json, "obsolete"),
		num_unneeded; //to num_unneeded zatim nepouzijem protoze to hazi divny vysledky
	
	printf("Number of packages to\n\tinstall: %d\n\tupdate: %d\n\terase: %d\n\tmaybe erase: %d\n", num_install, num_update, num_erase, num_obsolete);
	
	if(!num_install && !num_update && !num_erase && !num_obsolete)
	{
		ssds_log(logMESSAGE,"Nothing to do.\n");
		goto End;
	}
	
	GSList 	*install_pkgs = ssds_js_rd_parse_answer("install", json),
			*update_pkgs = ssds_js_rd_parse_answer("upgrade", json),
			*erase_pkgs = ssds_js_rd_parse_answer("erase", json);	

	ssds_log(logMESSAGE, "Result from server:\n");
	
	if(num_install)
	{								
		printf("Packages for install\n");
		
		for(guint i = 1; i < g_slist_length(install_pkgs); i++)
		{
			SsdsJsonPkg* pkg = (SsdsJsonPkg*)g_slist_nth_data(install_pkgs, i);
			printf("\t%s\n", pkg->pkg_name);
		}
	}

	if(num_update)
	{								
		printf("Packages for update\n");
		
		for(guint i = 1; i < g_slist_length(update_pkgs); i++)
		{
			SsdsJsonPkg* pkg = (SsdsJsonPkg*)g_slist_nth_data(update_pkgs, i);
			printf("\t%s\n", pkg->pkg_name);
		}
	}
	
	if(num_erase)
	{								
		printf("Packages for erase\n");
		
		for(guint i = 1; i < g_slist_length(erase_pkgs); i++)
		{
			SsdsJsonPkg* pkg = (SsdsJsonPkg*)g_slist_nth_data(erase_pkgs, i);
			printf("\t%s\n", pkg->pkg_name);
		}
	}			
	
	int ans = ssds_question("Is it ok?",((!num_install && !num_update && num_erase)? YES_NO : YES_NO_DOWNLOAD));
	
	if(ans == NO)
	{
		ssds_log(logMESSAGE,"Action interupted by user.\n");
		goto parseEnd;
	}

	rc = ssds_download(ans, install_pkgs, update_pkgs, erase_pkgs);
	    
    parseEnd:
	    g_slist_free_full(install_pkgs, (GDestroyNotify) ssds_free);
	    g_slist_free_full(update_pkgs, (GDestroyNotify) ssds_free);
	    g_slist_free_full(erase_pkgs, (GDestroyNotify) ssds_free);
	End:
		ssds_free(json);
	
	return rc;
}

int ssds_download(int answer, GSList *install, GSList *update, GSList *erase)
{
	int rc = OK;
	GSList  *install_list = NULL,
			*update_list = NULL;

	/***********************************************************/
	/* Downloading packages part                               */
	/***********************************************************/
	
	ssds_log(logDEBUG, "Begin downloading part.\n");
	
	// required variables for downloading
	gboolean return_status;
	LrHandle *handler;
	LrPackageTarget *target;
	GError *error = NULL;
	
	for(guint i = 1; i < g_slist_length(install); i++){
	   SsdsJsonPkg* inst = (SsdsJsonPkg*)g_slist_nth_data(install, i);
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
	   target = lr_packagetarget_new_v2(handler, inst->pkg_loc, DOWNLOAD_TARGET,
	                                    LR_CHECKSUM_UNKNOWN, NULL, 0, inst->base_url, TRUE,
	                                    progress_callback, inst->pkg_name, end_callback, NULL, &error);
	   install_list = g_slist_append(install_list, target);
	}
	
	for(guint i = 1; i < g_slist_length(update); i++){
	   SsdsJsonPkg* inst = (SsdsJsonPkg*)g_slist_nth_data(update, i);
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
	   target = lr_packagetarget_new_v2(handler, inst->pkg_loc, DOWNLOAD_TARGET,
	                                    LR_CHECKSUM_UNKNOWN, NULL, 0, inst->base_url, TRUE,
	                                    progress_callback, inst->pkg_name, end_callback, NULL, &error);
	   update_list = g_slist_append(update_list, target);
	}
	
	// Download all packages        
	ssds_log(logMESSAGE, "Downloading packages.\n");
	return_status = lr_download_packages(install_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
	
	if(!return_status || error != NULL){
	
	  ssds_log(logERROR, "%d: %s\n", error->code, error->message);
	  rc = DOWNLOAD_ERROR;
	
	}else{
	
	 	return_status = lr_download_packages(update_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
	
		if(!return_status || error != NULL){
	
	  		ssds_log(logERROR, "%d: %s\n", error->code, error->message);
	  		rc = DOWNLOAD_ERROR;
	
		}else{
			ssds_log(logMESSAGE, "All packages were downloaded successfully.\n");
			if(answer == DOWNLOAD)
			{
				ssds_log(logMESSAGE, "Packages are in %s.\n", DOWNLOAD_TARGET);
			}else{
				rc = ssds_rpm_process(install_list, update_list, erase);
			}
		}
	}

	g_error_free(error);
	g_slist_free_full(install_list, (GDestroyNotify) lr_packagetarget_free);
	g_slist_free_full(update_list, (GDestroyNotify) lr_packagetarget_free);
	
	return rc;
}

int ssds_rpm_process(GSList *install, GSList *update, GSList *erase)
{
	/*********************************************************/
	/* Installing / Updating / Erasing packages              */
	/*********************************************************/
	
	// required variables for rpmlib
	int rc = OK;
	rpmts ts;
	
	rpmReadConfigFiles(NULL, NULL);
	ts = rpmtsCreate();
	rpmtsSetRootDir(ts, NULL);
	
	
	if(install != NULL)
	{
		ssds_log(logMESSAGE, "Installing packages.\n");
		for(GSList *elem = install; elem; elem = g_slist_next(elem))
		{
		    LrPackageTarget *target = (LrPackageTarget *)elem->data;
		
		    if(!target->err)
		    {
		      	ssds_add_to_transaction(ts, target->local_path, SSDS_INSTALL);
		    }else{
		        ssds_log(logERROR, "Package Error: %s\n", target->err);
				rc = INSTALL_ERROR;
				goto rpmEnd;
		    }
		}
	} 

	if(update != NULL) 
    {
        ssds_log(logMESSAGE, "Updating packages.\n");
        for(GSList *elem = update; elem; elem = g_slist_next(elem))
        {
        	LrPackageTarget *target = (LrPackageTarget *)elem->data;

            if(!target->err)
            {
            	ssds_add_to_transaction(ts, target->local_path, SSDS_UPDATE);
            }else{
                ssds_log(logERROR, "Package Error: %s\n", target->err);
		  		rc = UPDATE_ERROR;
		  		goto rpmEnd;
            }
    	}
    }

    if(g_slist_length(erase) > 1)
    {
    	ssds_log(logMESSAGE, "Erasing packages.\n");
        for(GSList *elem = erase; elem; elem = g_slist_next(elem))
        {
			SsdsJsonPkg *pkg = (SsdsJsonPkg*)elem;
			printf("erase %s\n", pkg->pkg_name);
        	/*rc = ssds_add_to_erase(ts, (char *)elem);
            if(rc != OK){
				ssds_log(logERROR, "Unable to erase requested package.\n");
                rc = ERASE_ERROR;
				goto rpmEnd;
            } */
        }
    }
    
	rpmprobFilterFlags flag = 0;

    int nf = 0;

    nf |= INSTALL_LABEL | INSTALL_HASH;
    rpmtsSetNotifyCallback(ts, rpmShowProgress,(void *) nf);

    rc = rpmtsRun(ts, NULL, flag);
    
  rpmEnd:
    rpmtsClean(ts);
    rpmtsFree(ts);
    
    return rc;
}

int ssds_question(char* question, int possibilities)
{
	
	int status = NO, repeat = 1;
    char answer;
	switch(possibilities)
	{
		case YES_NO: 
			while(repeat)
			{
				ssds_log(logQUESTION, "%s\n[y/n]: ", question);
				scanf("%c", &answer);
				
				switch(answer)
				{
					case 'y': 
						status = YES;
						repeat = 0;
						break;
						
					case 'n': 
						repeat = 0;
						break;
						
					default: 
						ssds_log(logWARNING, "Unsupported answer. You should choose y for yes or n for no.\n");
						scanf("%c", &answer); //eliminating enter		
						break;
				}
			}
			break;
			
		case YES_NO_DOWNLOAD: 
			while(repeat)
	        {
         		ssds_log(logQUESTION, "%s\n[y/n/d]: ", question);
                scanf("%c", &answer);
				
				switch(answer)
                {
                	case 'y': 
						status = YES;
                        repeat = 0;
	                    break;
	                    
					case 'n': 
						repeat = 0;
                	    break;
                	    
					case 'd': 
						status = DOWNLOAD;
						repeat = 0;
						break;
						
					default: 
						ssds_log(logWARNING, "Unsupported answer. You should choose y for yes, n for no or d for download only.\n");
						scanf("%c", &answer); //eliminating enter     
						break;
				}
			}
			break;
	}
		
	return status;
}    
