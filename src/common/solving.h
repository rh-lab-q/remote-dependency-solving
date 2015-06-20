#ifndef _SOLVING_H
#define _SOLVING_H

#ifdef __cplusplus
extern "C"{
#endif

//SSDS
#include "repo_handler.h"
#include "json_handler.h"

//GLIB
#include <glib.h>

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>

//LIBREPO
#include <librepo/yum.h>
#include <librepo/handle.h>

HySack* ssds_solve_init();
void ssds_fill_sack(HySack* sack, SsdsRepoMetadataList* list);
void ssds_dep_query(const char* request, SsdsJsonCreate* answer, HySack* sack);
void ssds_dep_answer(SsdsJsonRead *client_data, SsdsJsonCreate* answer, HySack* sack);

#ifdef __cplusplus
}
#endif

#endif