#ifndef _CLIENT_H
#define _CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../common/includes.h"
#include <librepo/librepo.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmps.h>

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/params.h"
#include "../common/json_handler.h"
#include "../common/repo_handler.h"
#include "../common/util.h"
#include "../common/mem_management.h"
#include "../../cmake/Config/version_conf.h"
#include "../common/errors.h"
#include "../common/detect_missing_repos.h"
#include "../common/packages.h"

// Paths to download folders
static const char *DOWNLOAD_TARGET_INSTALL = "/var/cache/ssds/packages/install/";
static const char *DOWNLOAD_TARGET_UPDATE = "/var/cache/ssds/packages/update/";

int ssds_get_new_id(int socket, char **id, char *arch, char *release);
int ssds_send_System_solv(int comm_sock, int data_sock, char *path);
int ssds_send_repo(ParamOptsCl* params, char *arch, char *release, int comm_sock, int action);
int ssds_check_repo(int socket, char **message);
int ssds_answer_process(int socket, int action);
#ifdef __cplusplus
}
#endif

#endif
