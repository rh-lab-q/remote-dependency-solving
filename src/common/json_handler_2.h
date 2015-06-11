#ifndef _JSON_HANLDER_H
#define _JSON_HANLDER_H

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>

//JSON-GLIB-1.0
#include <json-glib/json-glib.h>
#include <json-glib/json-types.h>

//GLIB-2.0
#include <glib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

// typedef enum {
//   SSDS_BASEURL = 1,
//   SSDS_MIRRORLIST,
//   SSDS_METALINK
// } JsonUrlType;

/******************************************************************/
/* json_create part - implementation of functions in json_create.c*/
/******************************************************************/
typedef struct SsdsJsonCreate SsdsJsonCreate;

struct SsdsJsonCreate{
   JsonGenerator * generator;
   JsonNode* rootNode;
   JsonNode* codeNode;
   JsonNode* currNode;
   JsonObject* rootObj;
   JsonObject* dataObj;
   JsonObject* currObj;
   JsonArray* currArray;
};

SsdsJsonCreate* ssds_js_cr_init();
void ssds_js_insert_code(SsdsJsonCreate* json, int code);
void ssds_js_add_package(SsdsJsonCreate* json, char* package);
void ssds_js_add_repo(SsdsJsonCreate* json,char** url, char* name, int type, int url_count);
void ssds_js_pkgs_init(SsdsJsonCreate* json);
void ssds_js_pkgs_insert(SsdsJsonCreate* json,HyGoal* goal, const char* name);
char* ssds_js_to_string(SsdsJsonCreate* json);
void ssds_js_dump(SsdsJsonCreate* json);

















/******************************************************************/
/* json_read part - implementation of functions in json_read.c    */
/******************************************************************/


#endif


#ifdef __cplusplus
}
#endif