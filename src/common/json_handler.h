/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _JSON_HANLDER_H
#define _JSON_HANLDER_H

#ifdef __cplusplus
extern "C"{
#endif

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
#include <unistd.h>


/** Type of url from .repo file located in /etc/yum.repos.d/ */
typedef enum {
  SSDS_BASEURL = 1, /**< #baseurl in .repo file */
  SSDS_MIRRORLIST,  /**< #mirrorlist in .repo file */
  SSDS_METALINK     /**< #metalink in .repo file*/
} SsdsJsonUrlType;

/*******************************************************************/
/* json_create part - implementation of functions in json_create.c */
/*******************************************************************/

/** Structure used for creating message in json to be sent over the network */
typedef struct SsdsJsonCreate SsdsJsonCreate;

struct SsdsJsonCreate{
   JsonGenerator * generator; /**< Holds structure for generatig json from scraps */
   JsonNode* rootNode;        /**< Points to root of json */
   JsonNode* codeNode;        /**< Points to node holding control code */
   JsonNode* currNode;        /**< Currently used node */
   JsonObject* rootObj;       /**< Root object - used for adding new objects */
   JsonObject* dataObj;       /**< Data object - used for adding objects under data */
   JsonObject* currObj;       /**< Currently used object */
   JsonArray* currArray;      /**< Currently used array */
};

/** 
 * Return new json_create structure
 * @return          new allocated json_create structure
 */
SsdsJsonCreate* ssds_js_cr_init();

/** 
 * Inserts control code sequence into json_create structure.
 * The code cannot be omitted at any circumstances
 * @param json      SsdsJsonCreate*
 * @param code      int
 */
void ssds_js_insert_code(SsdsJsonCreate* json, int code);

/** 
 * Adds name of package requested by client into json_create structure.
 * Adds only one name at a time - needs to be called in a loop
 * @param json      SsdsJsonCreate*
 * @param package   char*
 */
void ssds_js_add_package(SsdsJsonCreate* json, char* package);

/** 
 * Adds infotmation about one repo located on client machine.
 * Adds only one repo structutre at a time - needs to be called in a loop
 * @param json      SsdsJsonCreate*
 * @param url       char**
 * @param name      char*
 * @param type      int
 * @param url_count int
 */
void ssds_js_add_repo(SsdsJsonCreate* json,char** url, char* name, int type, int url_count);


/** 
 * Adds object for packages requested by client into json_create structure.
 * Used by ssds_js_pkgs_insert on server side
 * @param json      SsdsJsonCreate*
 */
void ssds_js_pkgs_init(SsdsJsonCreate* json);

/** 
 * Adds one package into install_pkgs in json_create on server side.
 * One package includes information about packages that need to be installed, erased or upgraded
 * @param json      SsdsJsonCreate*
 * @param goal      HyGoal*
 * @param name      const char*
 */
void ssds_js_pkgs_insert(SsdsJsonCreate* json,HyGoal* goal, const char* name);

/** 
 * Converts json_create into string so that it can be sent over network
 * @param json      SsdsJsonCreate*
 * @return          char*
 */
char* ssds_js_to_string(SsdsJsonCreate* json);

/** 
 * Used for debugging json_create.
 * Prints whole structure to stdout
 * @param json      SsdsJsonCreate*
 */
void ssds_js_dump(SsdsJsonCreate* json);


/******************************************************************/
/* json_read part - implementation of functions in json_read.c    */
/******************************************************************/

/** 
 * Used by client to save names of packages requested by user.
 * Names are parsed from parameters 
 */
typedef struct SsdsPkgInfo SsdsPkgInfo;

/** 
 * Structure for info about one .repo file located on client machine.
 * List of repos is in SsdsRepoInfoList
 */
typedef struct SsdsRepoInfo SsdsRepoInfo;

/** 
 * GSList of SsdsRepoInfo 
 */
typedef struct SsdsRepoInfoList SsdsRepoInfoList;

/** 
 * Structure used for parsing message recieved through socket and converting it to json structure 
 */
typedef struct SsdsJsonRead SsdsJsonRead;

struct SsdsPkgInfo{
  char** packages;  /**< names of packages requested by user */
  int length;       /**< number of packages */
};

struct SsdsRepoInfo{
  char** urls;      /**< array of char pointers in case url is baseurl - there can be more than one baseurl for one repo */
  char* name;       /**< name of repo */
  int count;        /**< number of addresses in urls */
  int type;         /**< SsdsJsonUrlType */
};

struct SsdsRepoInfoList{
  GSList* repoInfoList; /** < GSList of SsdsRepoInfo */
};

struct SsdsJsonRead{
  JsonParser* parser;         /** < parser holds SsdsJsonRead structure */
  JsonNode* rootNode;         /** < root of json - used to find objects */
  JsonNode* currNode;         /** < currently used node */
  JsonNode* dataNode;         /** < data node - often used */
  JsonObject* currObj;        /** < currently used object */
  JsonObject* dataObj;        /** < data object - often used for adding new objects */
};

/**
 * Returns new json_read structure
 * @return          newly allocated json_read
 */
SsdsJsonRead* ssds_json_read_init();

/**
 * Parses input string to SsdsJsonRead structure
 * @param buffer    char*
 * @param json      SsdsJsonRead*
 * @return          TRUE if provided buffer contained valid json, FALSE otherwise
 */
gboolean ssds_read_parse(char* buffer, SsdsJsonRead* json);

/**
 * Gathers control code from provided SsdsJsonRead structure
 * @param json      SsdsJsonRead*
 * @return          int
 */
int ssds_read_get_code(SsdsJsonRead* json);

/**
 * Used by server to get names of packages requested by client.
 * Dependency solving will be run on these
 * @param pkgs      SsdsPkgInfo*
 * @param json      SsdsJsonRead*
 */
void ssds_read_get_packages(SsdsPkgInfo* pkgs, SsdsJsonRead* json);

/**
 * Used by server to get information about repos owned by client.
 * @param json      SsdsJsonRead*
 * @param list      SsdsRepoInfoList*
 */
void ssds_read_repo_info(SsdsJsonRead* json, SsdsRepoInfoList* list);

/**
 * Returns newly allocated SsdsPkgInfo
 * @return          SsdsPkgInfo*
 */
SsdsPkgInfo* ssds_read_pkginfo_init();

/**
 * Returns newly allocated SsdsRepoInfo
 * @return          SsdsRepoInfo*
 */
SsdsRepoInfo* ssds_read_repoinfo_init();

/**
 * Returns newly allocated SsdsRepoInfoList* used as a list of SsdsRepoInfo
 * @return          SsdsRepoInfoList*
 */
SsdsRepoInfoList* ssds_read_list_init();
  

#ifdef __cplusplus
}
#endif

#endif /* _JSON_HANLDER_H */