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
#include "../common/errors.h"
#include "../common/detect_missing_repos.h"
// Paths to download folders
static const char *DOWNLOAD_TARGET_INSTALL = "/var/cache/ssds/packages/install/";
static const char *DOWNLOAD_TARGET_UPDATE = "/var/cache/ssds/packages/update/";

#ifdef __cplusplus
}
#endif

#endif
