#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

#include "client.h"
#include "../common/repo_handler.h"
#include "../common/errors.h"
#include "../common/util.h"
#include "../common/cfg_parsing.h"


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

/******************* This is an interesting concept if we want to call a function after the program exits **************/
// void end() __attribute__((destructor));
// 
// void end()
// {
// 	printf("In the end() function\n");
// }

int main(int argc, char* argv[]) {
    setlogmask (LOG_UPTO (LOG_NOTICE));
    /*******************************************************************/
    /* Check root rights                                               */
    /*******************************************************************/
    uid_t uid = geteuid();
    if(uid != 0) {
        syslog(LOG_ERR, "This program has to be run under the root user.");
        printf("Error: This program has to be run under the root user.\n");
        return ROOT_ERROR;
    }

    /*******************************************************************/
    /* Client return code variable                                     */
    /*******************************************************************/
    int status = OK;

    /*******************************************************************/
    /* Setting up garbage collector and setting callback functions     */
    /*******************************************************************/
    signal(SIGINT, rds_signal_handler);
    signal(SIGBUS, rds_signal_handler);
    signal(SIGSEGV, rds_signal_handler);
    signal(SIGTERM, rds_signal_handler);

    /*******************************************************************/
    /* Parsing parameters 					     */
    /*******************************************************************/

    syslog(LOG_NOTICE, "rds-client program was initiated");

    ParamOptsCl* params = init_params_cl();
    if(parse_params_cl(argc, argv, params) != 0) {
        status = PARAMS_ERROR;
        goto end;
    }
    
#ifdef DEBUG
    printf("DEBUG: Parameters successfully parsed.\n");
#endif
    if((params->pkg_count == 0) && (params->command == PAR_UPDATE))
        params->command = PAR_UPDATE_ALL;
    
    /********************************************************************/
    /* Load configuration                                               */
    /********************************************************************/
    char *server_address, *id;
    long int port;
    read_cfg(&id, &server_address, &port);

    /********************************************************************/
    /*  Getting architecture, release and path to @System.solv          */ 
    /********************************************************************/
    char pathToOriginalSolv[100], 
    *pathToOriginalYum = "/etc/yum.conf",
    *pathToBackupSolv = "/var/cache/ssds/@System.solv",
    *pathToBackupYum = "/var/cache/ssds/yum.conf";

    char *arch = NULL, *release = NULL, *message = NULL;
    resolve_dependency_file_path(pathToOriginalSolv, &arch, &release);

    /********************************************************************/
    /* Networking part - connecting to server                           */
    /********************************************************************/
    printf("Info: Contacting the server. This might take a while.\n");
    int socket;

    status = client_connect(&socket, server_address, port);

    if(status != OK) goto end;

    /********************************************************************/
    /* Checking client ID                                               */
    /********************************************************************/
    if(id == NULL) {  //TODO - create request-response communication with server   
        status = get_new_id(socket, &id, arch, release);
        if(status != OK) goto end;

        status = send_file(socket, SEND_SOLV, pathToOriginalSolv);
        if(status != OK) goto end;
        
        if(copy_file(pathToOriginalSolv, pathToBackupSolv) != OK)
            syslog(LOG_WARNING, "rds-client was unable to create @System.solv backup file");

        status = send_file(socket, SEND_YUM_CONF, pathToOriginalYum);
        if(status != OK) goto end;
        
        if(copy_file(pathToOriginalYum, pathToBackupYum) != OK)
            syslog(LOG_WARNING, "rds-client was unable to create yum.conf backup file");
    }
    else {
        if(compare_files(pathToOriginalSolv, pathToBackupSolv) != OK) {
            int ans = question("DNF install packages by themselves. We need to make initial steps again. If you don't want to use RDS call rds-client --disconnect. Do you agree to make initial steps again?", YES_NO);

            if(ans == NO) {
                syslog(LOG_ERR, "Initial steps of rds-client declined by user");
                fprintf(stderr, "Error: Initial steps of rds declined. The program cannot continue.\n");
                goto end;
            }

            status = send_file(socket, SEND_SOLV, pathToOriginalSolv);
            if(status != OK) goto end;
            
            if(copy_file(pathToOriginalSolv, pathToBackupSolv) != OK)
                syslog(LOG_WARNING, "rds-client was unable to create @System.solv backup file");
        }
        if(compare_files(pathToOriginalYum, pathToBackupYum) != OK ) {
            int ans = question("New yum configuration will be sent to server. Do you agree?", YES_NO);
            if(ans == NO){
                syslog(LOG_ERR, "Sending of the new yum.conf to the server was declined");
                fprintf(stderr, "Sending of the new yum.conf to the server was declined. The program cannot continue.\n");
                goto end;
            }

            status = send_file(socket, SEND_YUM_CONF, pathToOriginalYum);
            if(status != OK) goto end;
            
            if(copy_file(pathToOriginalYum, pathToBackupYum) != OK)
                syslog(LOG_WARNING, "rds-client was unable to create yum.conf backup file");
        }
    }


    // this switch is now just for info about selected command
    switch(params->command)
    {
        case PAR_INSTALL: 
            #ifdef DEBUG
                printf("DEBUG: Switch for operation - INSTALL was chosen\n");
            #endif
                
            status = send_repo(params, arch, release, socket, GET_INSTALL);
            if(status != OK) break; 

            if(check_repo(socket, &message) != ANSWER_OK) {
                syslog(LOG_WARNING, "%s",message);
                printf("Server warning: %s\n", message);
            }

            status = answer_process(socket, GET_INSTALL);
        break;

        case PAR_UPDATE: 
            #ifdef DEBUG
                printf("DEBUG: Switch for operation - UPDATE was chosen\n");
            #endif

            status = send_repo(params, arch, release, socket, GET_UPDATE);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK) {
                syslog(LOG_WARNING, "%s",message);
                printf("Server warning: %s\n", message);
            }

            status = answer_process(socket, GET_UPDATE);
        break;

        case PAR_ERASE: 
            #ifdef DEBUG
                printf("DEBUG: Switch for operation - ERASE was chosen\n");
            #endif

            status = send_repo(params, arch, release, socket, GET_ERASE);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK) {
                syslog(LOG_WARNING, "%s",message);
                printf("Server warning: %s\n", message);
            }

            status = answer_process(socket, GET_ERASE);
        break;

        case PAR_CHK_DEP: 
            #ifdef DEBUG
                printf("DEBUG: Switch for operation - DEPENDENCY CHECK was chosen\n");
            #endif
                printf("Dependency check operation was not implemented yet.\n");
        break;

        case PAR_UPDATE_ALL:
            #ifdef DEBUG
                printf("DEBUG: Switch for operation - UPDATE ALL was chosen\n");
            #endif
            
            status = send_repo(params, arch, release, socket, GET_UPDATE_ALL);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK) {
                syslog(LOG_WARNING, "%s",message);
                printf("Server warning: %s\n", message);
            }

            status = answer_process(socket, GET_UPDATE);
        break;
    } 

    end:
        #ifdef DEBUG
            printf("Client reached the end.\n");
        #endif
        free_params_cl(params);

    return status;
}
