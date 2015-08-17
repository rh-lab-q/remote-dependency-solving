#ifndef _PACKAGES_H
#define _PACKAGES_H

#ifdef __cplusplus
extern "C"{
#endif

#include "includes.h"
#include "log_handler.h"
#include "errors.h"

#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmdb.h>

enum{
        SSDS_INSTALL = 0,
        SSDS_UPDATE
};

int ssds_add_to_transaction (rpmts ts, char *pkg, int action);
int ssds_add_to_erase (rpmts ts, char *pkg);
void *rpmCallback(const void * arg,
                  const rpmCallbackType what,
                  const rpm_loff_t amount,
                  const rpm_loff_t total,
                  fnpyKey key,
                  void * data);

#ifdef __cplusplus
}
#endif

#endif

