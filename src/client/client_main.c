#include <stdlib.h>
#include <signal.h>

#include "client.h"
#include "../common/repo_handler.h"
#include "../common/errors.h"
#include "../common/util.h"
#include "../common/cfg_parsing.h"


//#define VERSION_HAWKEY @HKY_22@
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

/******************* This is an interesting concept if we want to call a function after the program exits **************/
// void end() __attribute__((destructor));
// 
// void end()
// {
// 	printf("In the end() function\n");
// }

int main(int argc, char* argv[]) {
    printf("Client start\n");
    /*******************************************************************/
    /* Check root rights                                               */
    /*******************************************************************/
//     uid_t uid = geteuid();
//     if(uid != 0) {
//         rds_log(logERROR, "This program has to be run under the root user otherwise no packages can be installed, erased or updated.\n");
//         return ROOT_ERROR;
//     }

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

    rds_log(logSSDS, "Client startup.\n");

    ParamOptsCl* params = init_params_cl();

    
    if(parse_params_cl(argc, argv, params) != 0) {
        status = PARAMS_ERROR;
        goto end;
    }
    
    rds_log(logDEBUG, "Client params initialized.\n");
    rds_log(logDEBUG, "Client params parsed. Package count %d.\n", params->pkg_count);  
    rds_log(logMESSAGE, "Client startup. Required package count %d.\n", params->pkg_count); 

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
    rds_log(logDEBUG, "Network part.\n");

    int socket;

    status = client_connect(&socket, server_address, port);

    if(status != OK) goto end;

//     rds_log(logDEBUG, "Communication socket: %d.\n", socket);

    /********************************************************************/
    /* Checking client ID                                               */
    /********************************************************************/

    if(id == NULL) {  //TODO - create request-response communication with server   
        status = get_new_id(socket, &id, arch, release);
        if(status != OK) goto end;

        status = send_file(socket, SEND_SOLV, pathToOriginalSolv);
        if(status != OK) goto end;
        
        if(copy_file(pathToOriginalSolv, pathToBackupSolv) != OK)
            rds_log(logWARNING, "Unable to make @System.solv backup.\n");

        status = send_file(socket, SEND_YUM_CONF, pathToOriginalYum);
        if(status != OK) goto end;
        
        if(copy_file(pathToOriginalYum, pathToBackupYum) != OK)
            rds_log(logWARNING, "Unable to make yum.conf backup.\n");

    }
    else {
        if(compare_files(pathToOriginalSolv, pathToBackupSolv) != OK) {
            int ans = question("DNF install packages by themselves. We need to make initial steps again. If you don't want to use RDS call rds-client --disconnect. Do you agree to make initial steps again?", YES_NO);

            if(ans == NO) {
                rds_log(logMESSAGE,"Action interupted by user.\n");
                goto end;
            }

            status = send_file(socket, SEND_SOLV, pathToOriginalSolv);
            if(status != OK) goto end;
            
            if(copy_file(pathToOriginalSolv, pathToBackupSolv) != OK)
                rds_log(logWARNING, "Unable to make @System.solv backup.\n");
            
        }

        if( compare_files(pathToOriginalYum, pathToBackupYum) != OK ) {
            int ans = question("New yum configuration will be sent to server. Do yout agree?", YES_NO);
            if(ans == NO){
                rds_log(logMESSAGE,"Action interupted by user.\n");
                goto end;
            }

            status = send_file(socket, SEND_YUM_CONF, pathToOriginalYum);
            if(status != OK) goto end;
            
            if(copy_file(pathToOriginalYum, pathToBackupYum) != OK)
                rds_log(logWARNING, "Unable to make yum.conf backup.\n");
        }
    }


    // this switch is now just for info about selected command
    switch(params->command)
    {
        case PAR_INSTALL: 
            rds_log(logMESSAGE, "Installation of packages was selected.\n");

            status = send_repo(params, arch, release, socket, GET_INSTALL);
            if(status != OK) break; 

            if(check_repo(socket, &message) != ANSWER_OK)
                rds_log(logWARNING,"%s\n", message);

            status = answer_process(socket, GET_INSTALL);
        break;

        case PAR_UPDATE: 
            rds_log(logMESSAGE, "Update of packages was selected.\n");
            rds_log(logERROR, "Update option has not been implemented yet.\n");

            status = send_repo(params, arch, release, socket, GET_UPDATE);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK)
                rds_log(logWARNING,"%s\n", message);

            status = answer_process(socket, GET_UPDATE);

        break;

        case PAR_ERASE: 
            rds_log(logMESSAGE, "Erase of packages was selected.\n");

            status = send_repo(params, arch, release, socket, GET_ERASE);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK)
                rds_log(logWARNING,"%s\n", message);

            status = answer_process(socket, GET_ERASE);

        break;

        case PAR_CHK_DEP: 
            rds_log(logMESSAGE, "Dependency check of packages was selected.\n");
            rds_log(logERROR, "Dependency check option has not been implemented yet.\n");
        break;

        case PAR_UPDATE_ALL:
            rds_log(logMESSAGE, "Update all packages was initiated.\n");
            status = send_repo(params, arch, release, socket, GET_UPDATE_ALL);
            if(status != OK) break;

            if(check_repo(socket, &message) != ANSWER_OK)
                rds_log(logWARNING,"%s\n", message);

            status = answer_process(socket, GET_UPDATE);
        break;
    } 

    end:
        rds_log(logSSDS, "End of client.\n\n");
        free_params_cl(params);

    return status;
}
