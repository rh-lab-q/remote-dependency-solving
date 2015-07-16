#ifndef _SERVER_2_H
#define _SERVER_2_H

#ifdef __cplusplus
extern "C"{
#endif

//GLIB
#include <glib.h>


#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/repo_handler.h"
#include "../common/json_handler.h"
#include "../common/solving.h"
#include "../common/params.h"
#include "../../cmake/Config/version_conf.h"


#ifdef __cplusplus
}
#endif

#endif
