#ifndef _CLIENT_H
#define _CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/params.h"
#include "../common/json_handler.h"
#include "../common/repo_handler.h"

// path to package download directory
static const char *DOWNLOAD_TARGET = "/var/cache/ssds/packages/";
/*
std::string usage = "Usage: ./ssds-client <command> [<args> ...]\n\n"
                    "List of Commands\n\n"
                    "install\t\tResolve dependencies and install packages\n"
                    "checkdep\t\tOnly show required packages - do not install yet\n"
                    "help\t\tDisplays help\n";
enum param_options{
          CHECK_DEPENDENCIES
};*/

#ifdef __cplusplus
}
#endif

#endif