#ifndef _CLIENT_H
#define _CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../common/includes.h"
#include <librepo/librepo.h>
//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/params.h"
#include "../common/json_handler.h"
#include "../common/repo_handler.h"
#include "../common/util.h"
#include "../common/mem_management.h"
#include "../../cmake/Config/version_conf.h"
#include "../common/cfg_parsing.h"
#include "../common/errors.h"

/** path to package download directory */
static const char *DOWNLOAD_TARGET_INSTALL = "/var/cache/ssds/packages/install/";
//static const char *DOWNLOAD_TARGET_UPDATE = "/var/cache/ssds/packages/update/";
//int create_json(ParamOptsCl *params, SsdsJsonRead **json_read_ret, char **repo_output_ret, char **msg_output_ret);
//int network_part(SsdsJsonRead *json_read, char *repo_output, char *msg_output, SsdsJsonAnswer** answer_from_srv_ret);
//int download_packages(SsdsJsonAnswer* answer_from_srv, GSList **package_list_ret);
//int install_packages(SsdsJsonAnswer* answer_from_srv, GSList *package_list);

#ifdef __cplusplus
}
#endif

#endif
