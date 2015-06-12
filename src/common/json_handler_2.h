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

typedef enum {
  SSDS_BASEURL = 1,
  SSDS_MIRRORLIST,
  SSDS_METALINK
} SsdsJsonUrlType;

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
typedef struct SsdsPkgInfo SsdsPkgInfo;
typedef struct SsdsRepoInfo SsdsRepoInfo;
typedef struct SsdsRepoInfoList SsdsRepoInfoList;
typedef struct SsdsJsonRead SsdsJsonRead;

struct SsdsPkgInfo{
  char** packages;
  int length;
};

struct SsdsRepoInfo{
  char** urls; //array of char pointers in case url is baseurl - there can be more than one baseurl for one repo
  char* name;
  int count;//number of addresses in urls
  int type;
};

struct SsdsRepoInfoList{
  GSList* repoInfoList;
};

struct SsdsJsonRead{
  JsonParser* parser;
  JsonNode* rootNode;
  JsonNode* currNode;
  JsonNode* dataNode;
  JsonObject* currObj;
  JsonObject* dataObj;
};

SsdsJsonRead* ssds_json_read_init();
int ssds_read_parse(char* buffer, SsdsJsonRead* json);
int ssds_read_get_code(SsdsJsonRead* json);
void ssds_read_get_packages(struct SsdsPkgInfo* pkgs, SsdsJsonRead* json);//used by server to get packages required by client
void ssds_read_repo_info(SsdsJsonRead* json, SsdsRepoInfoList* list);
SsdsPkgInfo* ssds_read_pkginfo_init();
SsdsRepoInfo* ssds_read_repoinfo_init();
  
// GSList* repoInfoList;


#ifdef __cplusplus
}
#endif

#endif