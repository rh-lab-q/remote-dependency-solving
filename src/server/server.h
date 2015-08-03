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


#ifdef __cplusplus
}
#endif

#endif

int core();