/**
 * Server side dependency solving 
 * transfer of dependency solving from local machine to server when installing new packages
 * 
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký, Walter Scherfel, Šimon Matěj
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is rds_free software; you can redistribute it and/or
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

#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmps.h>

#include "client.h"
#include "../common/network_util.h"
#include "../common/repo_handler.h"
#include "../common/log_handler.h"
#include "../common/json_handler.h"
#include "../common/util.h"
#include "../common/packages.h"
#include "../common/errors.h"
#include "../common/util.h"


int get_new_id(int socket, char **id, char *arch, char *release) {
    char *message;
    JsonCreate* json_gen = js_cr_init(GENERATE_ID);

    rds_log(logDEBUG, "Inserted code %d into json.\n", GENERATE_ID);

    js_cr_gen_id(json_gen, arch, release);    // generate message for server
    rds_log(logDEBUG, "Generated JSON for server with params: arch=%s, release=%s.\n", arch, release);

//     rds_log(logDEBUG, "Generating message string.\n");
    message = js_cr_to_string(json_gen);
    rds_log(logDEBUG, "Message string generated: \t%s\n", message);

    rds_log(logMESSAGE, "Sending initial message to server.\n");
    secure_write(socket, message, strlen(message));
    rds_log(logMESSAGE, "Message sent.\n");

    *id = NULL;
    //TODO: read answer from server
    js_cr_dispose(json_gen);

    return OK;
}

int send_file(int socket, int type, char *path) {
    JsonCreate* json_msg = js_cr_init(type);

    char* msg_output;

    /***********************************************************/
    /* Sending file                                            */
    /***********************************************************/
    rds_log(logDEBUG, "Path to file : %s\n",path);
    FILE * f;
    f = fopen(path,"rb");

    rds_log(logDEBUG, "Opening file.\n");
    if(f == NULL) {
        rds_log(logERROR,"Error while opening file %s.\n", path);
        return FILE_ERROR;
    }

    rds_log(logDEBUG, "Getting size of file.\n");

    fseek(f, 0, SEEK_END);
    ssize_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    rds_log(logDEBUG, "Size of file is %d.\n", size);
    js_cr_set_read_bytes(json_msg, (int) size);
    msg_output = js_cr_to_string(json_msg);
    secure_write(socket, msg_output, strlen(msg_output));
    rds_log(logDEBUG, "Preparing variables.\n");

    char buffer[131072];
    size_t bytes_read = 0;

    rds_log(logDEBUG, "Sending file.\n");
    while((bytes_read = fread(buffer, 1, 131072, f)) != 0) {
        secure_write(socket, buffer, bytes_read);
        rds_log(logDEBUG, "Data sent.\n");
    }
    js_cr_dispose(json_msg);

    char *buff = sock_recv(socket);

    JsonRead *json = js_rd_init();

    js_rd_parse(buff, json);

    int rc = js_rd_get_code(json);

    if(rc != ANSWER_OK) {
        rds_log(logERROR, "Message from server: %s\n", js_rd_get_message(json));
        rc = NETWORKING_ERROR;
    }
    else {
        rc = OK;
    }
    free(buff);

    return rc; 
}

int compare_files(char *fileOne, char *fileTwo) {
    FILE *f1, *f2;
    f1 = fopen(fileOne,"r");
    f2 = fopen(fileTwo,"r");
    int rc = OK;
    
    rds_log(logDEBUG, "Opening files for compare.\n");
    if(f1 == NULL) {
        rds_log(logERROR,"Error while opening file %s.\n", fileOne);
        return FILE_ERROR;
    }

    if(f2 == NULL) {
        rds_log(logERROR,"Error while opening file %s.\n", fileTwo);
        return FILE_ERROR;
    }

    char ch1,ch2;
    ch1 = fgetc(f1);
    ch2 = fgetc(f2);

    while((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
        ch1 = fgetc(f1), 
        ch2 = fgetc(f2);
    }

    if(ch1 == ch2) {
        rds_log(logDEBUG, "Files are identical.\n");
    }
    else {
        rds_log(logDEBUG, "Files are different.\n");
        rc = FILE_ERROR;
    }

    fclose(f1);
    fclose(f2);

    return rc; 
}

int copy_file(char *source, char *destination) {
    FILE *s, *d;
    s = fopen(source,"rb");
    d = fopen(destination,"wb");

    rds_log(logDEBUG, "Opening files for copy.\n");
    
    if(s == NULL) {
        rds_log(logERROR,"Error while opening file %s.\n", source);
        return FILE_ERROR;
    }

    if(d == NULL) {
        rds_log(logERROR,"Error while opening file %s.\n", destination);
        return FILE_ERROR;
    }

    char ch[1];

    while(fread(ch,1,1,s) != 0) {
        fwrite(ch, 1, 1, d);
    }

    fclose(s);
    fclose(d);

    return OK; 
}

int send_repo(ParamOptsCl* params, char *arch, char *release, int socket, int action) {
    JsonCreate *json_gen;
    LocalRepoInfo* local_repo;
    char* repo_output;
    
    rds_log(logDEBUG, "Client repo info JSON creating.\n");

    json_gen = js_cr_init(action);
    local_repo = repo_parse_init();
    
    rds_log(logDEBUG, "Local repo info initialized.\n");

    // parsing local repo
    if(!parse_default_repo(local_repo)) {
        return REPO_ERROR;
    }
    rds_log(logDEBUG, "Local repo is parsed.\n");

    get_repo_urls(local_repo, json_gen, arch, release);
    rds_log(logDEBUG, "Getting repo urls.\n");

    rds_log(logDEBUG, "Loop thrue required packages.\n");
    for(int i = 0; i < params->pkg_count; i++) {
        char* pkg = (char*)g_slist_nth_data(params->pkgs, i);
        js_cr_add_package(json_gen, pkg);
        rds_log(logDEBUG, "Added %s package as %d in order.\n", pkg, i+1);
    }
    
    rds_log(logDEBUG, "Generating output message with repo info to server.\n");
    repo_output = js_cr_to_string(json_gen);
    rds_log(logDEBUG, "Message generated.\n\n%s\n\n", repo_output);

    /***********************************************************/
    /* Sending repo info to server                             */
    /***********************************************************/
    rds_log(logMESSAGE, "Sending message with repo info to server.\n");
    secure_write(socket, repo_output, strlen(repo_output));
    rds_log(logDEBUG, "Message sent.\n");

    free(repo_output);
    js_cr_dispose(json_gen);
    return OK;
}

int check_repo(int socket, char **message) {
    char *buffer;
    JsonRead *json;
    int rc = -1;

    buffer = sock_recv(socket);
    json = js_rd_init();
    js_rd_parse(buffer, json);

    rc = js_rd_get_code(json);

    if(rc != ANSWER_OK)
        *message = js_rd_get_message(json);

    return rc;
}

int answer_process(int socket, int action) {
    rds_log(logMESSAGE, "Waiting for answer from server.\n");

    char *buf;
    JsonRead *json;
    int rc = -1;

    buf = sock_recv(socket);
    json = js_rd_init();

    if(buf == NULL) {
        rds_log(logERROR, "Error while recieving data\n");
        rc = NETWORKING_ERROR;
        goto End;
    }
    rds_log(logDEBUG, "Some answer has been delivered.\n\n%s\n\n", buf);

    // parse response
    rds_log(logDEBUG, "Parsing answer.\n");

    if(!js_rd_parse(buf, json)) {
        rds_log(logERROR, "Error while parsing answer from the server\n");
        rc = JSON_ERROR;
        goto End;
    }

    rc = js_rd_get_code(json);

    if(rc == ANSWER_WARNING) {
        rds_log(logWARNING,"%s\n", js_rd_get_message(json));
        goto End;//TODO - asi vymyslet co s warningama protoze warningem by to koncit nemuselo ten program
    }

    if(rc == ANSWER_ERROR) {
        rds_log(logERROR,"%s\n", js_rd_get_message(json));
        goto End;
    }

    if(rc == ANSWER_NO_DEP) {
        rds_log(logERROR, "Answer no dep.\n");
        goto End;
    }


    int num_install = js_rd_get_count(json, "install"),
    num_update = js_rd_get_count(json, "upgrade"),
    num_erase = js_rd_get_count(json, "erase"),
    num_obsolete = js_rd_get_count(json, "obsolete"),
    num_unneeded; //to num_unneeded zatim nepouzijem protoze to hazi divny vysledky

    printf("Number of packages to\n\tinstall: %d\n\tupdate: %d\n\terase: %d\n\tmaybe erase: %d\n", num_install, num_update, num_erase, num_obsolete);

    if(!num_install && !num_update && !num_erase && !num_obsolete) {
        rds_log(logMESSAGE,"Nothing to do.\n");
        goto End;
    }

    GSList 	*install_pkgs = js_rd_parse_answer("install", json),
        *update_pkgs = js_rd_parse_answer("upgrade", json),
        *erase_pkgs = js_rd_parse_answer("erase", json);	

    rds_log(logMESSAGE, "Result from server:\n");

    if(num_install) {
        printf("Packages for install\n");

        for(guint i = 1; i < g_slist_length(install_pkgs); i++) {
            JsonPkg* pkg = (JsonPkg*)g_slist_nth_data(install_pkgs, i);
            printf("\t%s\n", pkg->pkg_name);
        }
    }

    if(num_update) {
        printf("Packages for update\n");

        for(guint i = 1; i < g_slist_length(update_pkgs); i++)
        {
            JsonPkg* pkg = (JsonPkg*)g_slist_nth_data(update_pkgs, i);
            printf("\t%s\n", pkg->pkg_name);
        }
    }

    if(num_erase) {
        printf("Packages for erase\n");

        for(guint i = 1; i < g_slist_length(erase_pkgs); i++)
        {
            JsonPkg* pkg = (JsonPkg*)g_slist_nth_data(erase_pkgs, i);
            printf("\t%s\n", pkg->pkg_name);
        }
    }			

    int ans = question("Is it ok?",((!num_install && !num_update && num_erase)? YES_NO : YES_NO_DOWNLOAD));

    if(ans == NO) {
        rds_log(logMESSAGE,"Action interupted by user.\n");
        goto parseEnd;
    }

    rc = download(ans, install_pkgs, update_pkgs, erase_pkgs);
        
    parseEnd:
        g_slist_free(install_pkgs);
        g_slist_free(update_pkgs);
        g_slist_free(erase_pkgs);
    End:
        free(json);

    return rc;
}

int download(int answer, GSList *install, GSList *update, GSList *erase) {
    int rc = OK;
    GSList  *install_list = NULL,
    *update_list = NULL;

    /***********************************************************/
    /* Downloading packages part                               */
    /***********************************************************/

    rds_log(logDEBUG, "Begin downloading part.\n");

    // required variables for downloading
    gboolean return_status;
    LrHandle *handler;
    LrPackageTarget *target;
    GError *error = NULL;

    for(guint i = 1; i < g_slist_length(install); i++) {
        JsonPkg* inst = (JsonPkg*)g_slist_nth_data(install, i);
        rds_log(logMESSAGE, "Downloading preparation for package: %s\n", inst->pkg_name);

        rds_log(logDEBUG, "Downloading preparation.\n");
        handler = lr_handle_init();
        rds_log(logDEBUG, "Download handler initied.\n");
        lr_handle_setopt(handler, NULL, LRO_METALINKURL, inst->metalink);
        rds_log(logDEBUG, "Array of URLs is set.\n");
        lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
        rds_log(logDEBUG, "Repo type is set.\n");
        lr_handle_setopt(handler, NULL, LRO_PROGRESSCB, progress_callback);
        rds_log(logDEBUG, "Progress callback is set.\n");

        // Prepare list of target
        target = lr_packagetarget_new_v2(handler, inst->pkg_loc, DOWNLOAD_TARGET,
        LR_CHECKSUM_UNKNOWN, NULL, 0, inst->base_url, TRUE,
        progress_callback, inst->pkg_name, end_callback, NULL, &error);
        install_list = g_slist_append(install_list, target);
    }

    for(guint i = 1; i < g_slist_length(update); i++) {
        JsonPkg* inst = (JsonPkg*)g_slist_nth_data(update, i);
        rds_log(logMESSAGE, "Downloading preparation for package: %s\n", inst->pkg_name);

        rds_log(logDEBUG, "Downloading preparation.\n");
        handler = lr_handle_init();
        rds_log(logDEBUG, "Download handler initied.\n");
        lr_handle_setopt(handler, NULL, LRO_METALINKURL, inst->metalink);
        rds_log(logDEBUG, "Array of URLs is set.\n");
        lr_handle_setopt(handler, NULL, LRO_REPOTYPE, LR_YUMREPO);
        rds_log(logDEBUG, "Repo type is set.\n");
        lr_handle_setopt(handler, NULL, LRO_PROGRESSCB, progress_callback);
        rds_log(logDEBUG, "Progress callback is set.\n");

        // Prepare list of target
        target = lr_packagetarget_new_v2(handler, inst->pkg_loc, DOWNLOAD_TARGET,
        LR_CHECKSUM_UNKNOWN, NULL, 0, inst->base_url, TRUE,
        progress_callback, inst->pkg_name, end_callback, NULL, &error);
        update_list = g_slist_append(update_list, target);
    }

    // Download all packages        
    rds_log(logMESSAGE, "Downloading packages.\n");
    return_status = lr_download_packages(install_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);

    if(!return_status || error != NULL) {
        rds_log(logERROR, "%d: %s\n", error->code, error->message);
        rc = DOWNLOAD_ERROR;
    }
    else {
        return_status = lr_download_packages(update_list, LR_PACKAGEDOWNLOAD_FAILFAST, &error);

        if(!return_status || error != NULL){
            rds_log(logERROR, "%d: %s\n", error->code, error->message);
            rc = DOWNLOAD_ERROR;
        }
        else {
            rds_log(logMESSAGE, "All packages were downloaded successfully.\n");
            if(answer == DOWNLOAD)
                rds_log(logMESSAGE, "Packages are in %s.\n", DOWNLOAD_TARGET);
            else
                rc = rpm_process(install_list, update_list, erase);
        }
    }

    g_error_free(error);
    g_slist_free_full(install_list, (GDestroyNotify) lr_packagetarget_free);
    g_slist_free_full(update_list, (GDestroyNotify) lr_packagetarget_free);

    return rc;
}

int rpm_process(GSList *install, GSList *update, GSList *erase) {
    /*********************************************************/
    /* Installing / Updating / Erasing packages              */
    /*********************************************************/

    // required variables for rpmlib
    int rc = OK;
    rpmts ts;

    rpmReadConfigFiles(NULL, NULL);
    ts = rpmtsCreate();
    rpmtsSetRootDir(ts, NULL);


    if(install != NULL) {
        rds_log(logMESSAGE, "Installing packages.\n");
        for(GSList *elem = install; elem; elem = g_slist_next(elem)) {
            LrPackageTarget *target = (LrPackageTarget *)elem->data;

            if(!target->err)
                add_to_transaction(ts, target->local_path, RDS_INSTALL);
            else {
                rds_log(logERROR, "Package Error: %s\n", target->err);
                rc = INSTALL_ERROR;
                goto rpmEnd;
            }
        }
    } 

    if(update != NULL) {
        rds_log(logMESSAGE, "Updating packages.\n");
        for(GSList *elem = update; elem; elem = g_slist_next(elem)) {
            LrPackageTarget *target = (LrPackageTarget *)elem->data;

            if(!target->err)
                add_to_transaction(ts, target->local_path, RDS_UPDATE);
            else {
                rds_log(logERROR, "Package Error: %s\n", target->err);
                rc = UPDATE_ERROR;
                goto rpmEnd;
            }
        }
    }

    if(g_slist_length(erase) > 1) {
        rds_log(logMESSAGE, "Erasing packages.\n");
        for(GSList *elem = erase; elem; elem = g_slist_next(elem)) {
            JsonPkg *pkg = (JsonPkg*)elem;
            printf("erase %s\n", pkg->pkg_name);
            /*rc = add_to_erase(ts, (char *)elem);
            if(rc != OK){
            rds_log(ERROR, "Unable to erase requested package.\n");
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

int question(char* question, int possibilities) {
    int status = NO, repeat = 1;
    char *answer = NULL, letter;
    switch(possibilities)
    {
        case YES_NO: 
            while(repeat) {
                rds_log(logQUESTION, "%s\n[y/n]: ", question);
                if(getline(&answer, &size, stdin) == -1){
                   // TODO error message after log reimpmelentation
                }else{
                     letter = answer[0];
                }

                switch(letter)
                {
                    case 'y': 
                        status = YES;
                        repeat = 0;
                    break;

                    case 'n': 
                        repeat = 0;
                    break;

                    default: 
                        rds_log(logWARNING, "Unsupported answer. You should choose y for yes or n for no.\n");
                    break;
                }
            }
        break;

        case YES_NO_DOWNLOAD: 
            while(repeat)
            {
                rds_log(logQUESTION, "%s\n[y/n/d]: ", question);
                if(getline(&answer, &size, stdin) == -1){
                    // TODO error message after log reimpmelentation
                }else{
                    letter = answer[0];
                }
                
                switch(letter)
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
                    rds_log(logWARNING, "Unsupported answer. You should choose y for yes, n for no or d for download only.\n");
                break;
                }
            }
        break;
    }

    return status;
}    
