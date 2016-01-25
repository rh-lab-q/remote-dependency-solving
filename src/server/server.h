#ifndef _SERVER_2_H
#define _SERVER_2_H

#ifdef __cplusplus
extern "C"{
#endif

//Standard libraries
#include "../common/includes.h"

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/repo_handler.h"
#include "../common/json_handler.h"
#include "../common/solving.h"
#include "../common/params.h"
#include "../common/mem_management.h"
#include "../../cmake/Config/version_conf.h"
#include "../common/cfg_parsing.h"
#include "../common/errors.h"

int ssds_server_init(int *comm_desc, int *comm_addr_len);

int ssds_server_accept_connection(int comm_desc, int comm_addr_len);

int ssds_server_process(int comm_port, char *clietn_ip, int *client_end);

#ifdef __cplusplus
}
#endif
#endif
