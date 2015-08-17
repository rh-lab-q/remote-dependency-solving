/**
 * Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
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

#include "includes.h" 

#include "mem_management.h"

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
   JsonNode* dataNode;        /**< Points to node holding data part of json */
   JsonNode* currNode;        /**< Currently used node */
   JsonObject* rootObj;       /**< Root object - used for adding new objects */
   JsonObject* dataObj;       /**< Data object - used for adding objects under data */
   JsonObject* currObj;       /**< Currently used object */
   JsonArray* currArray;      /**< Currently used array */
};


/**
 * Type of data that is inserted into objects or arrays
 */
typedef enum {
  JS_STRING = 0,
  JS_INT,
  JS_BOOL,
  JS_ARRAY,
  JS_OBJ
}SsdsJsInsType;


/**
 * Returns new SsdsJsonCreate structure - this structure is needed during the whole process of json creation
 * @return          newly allocated SsdsJsonCreate structure
 */
SsdsJsonCreate* ssds_js_cr_init(int code);

/**
 * Converts data in memory represented by SsdsJsonCreate to string.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 */
char* ssds_js_cr_to_string(SsdsJsonCreate* json);

/**
 * Function used for debugging - use it to dump the created json to stdout.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 */
void ssds_js_cr_dump(SsdsJsonCreate* json);

/** 
 * Inserts control code sequence into json_create structure.
 * The code cannot be omitted at any circumstances
 * @param json      SsdsJsonCreate*
 * @param code      int
 */
void ssds_js_cr_insert_code(SsdsJsonCreate* json, int code);

/** 
 * Adds name of package requested by client into json_create structure.
 * Adds only one name at a time - needs to be called in a loop
 * @param json      SsdsJsonCreate*
 * @param package   char*
 */
void ssds_js_cr_add_package(SsdsJsonCreate* json, char* package);

/** 
 * Adds infotmation about one repo located on client machine.
 * Adds only one repo structutre at a time - needs to be called in a loop
 * @param json      SsdsJsonCreate*
 * @param url       char**
 * @param name      char*
 * @param type      int
 * @param url_count int
 */
void ssds_js_cr_add_repo(SsdsJsonCreate* json,char** url, char* name, int type, int url_count);


/** 
 * Adds object for packages requested by client into json_create structure.
 * Used by ssds_js_pkgs_insert on server side
 * @param json      SsdsJsonCreate*
 */
void ssds_js_cr_pkgs_init(SsdsJsonCreate* json);

/** 
 * Adds one package into install_pkgs in json_create on server side.
 * One package includes information about packages that need to be installed, erased or upgraded
 * @param json      SsdsJsonCreate*
 * @param goal      HyGoal*
 * @param name      const char*
 */
void ssds_js_cr_pkgs_insert(SsdsJsonCreate* json,HyGoal* goal, const char* name);

/** 
 * Converts json_create into string so that it can be sent over network
 * @param json      SsdsJsonCreate*
 * @return          char*
 */
char* ssds_js_cr_to_string(SsdsJsonCreate* json);

/** 
 * Used for debugging json_create.
 * Prints whole structure to stdout
 * @param json      SsdsJsonCreate*
 */
void ssds_js_cr_dump(SsdsJsonCreate* json);


/**
 * These functions can be used for more flexible creation of JSON structures
 */

/**
 * Adds a new element into currArray from SsdsJsonCreate structure
 * Array that is created last is set to be currArray. For inserting elements to defferent array use ssds_js_cr_switch_array
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 * @param type      SsdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 */
void ssds_js_cr_add_array_member(SsdsJsonCreate* json, int type, void* data);

/**
 * Adds a new element into currObj from SsdsJsonCreate structure. 
 * Works the same way as ssds_js_cr_add_array_member.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 * @param type      SsdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void ssds_js_cr_add_obj_member(SsdsJsonCreate* json, int type, void* data, const char* name);

/**
 * Adds new element into top level data object. It can be array, object, string, bool or int.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 * @param type      SsdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void ssds_js_cr_new_data(SsdsJsonCreate* json, int type, const char* name, void* data);

/**
 * Finds an array anywhere in the json structure and sets json->currArray as the currently active array.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 * @param name      String used as the array identifier
 * @return          TRUE if array with given name is found and set, otherwise FALSE
 */
gboolean ssds_js_cr_switch_array(SsdsJsonCreate* json, const char* name);

/**
 * Sets array one level above the current array as the current array.
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 */
void ssds_js_cr_upper_array(SsdsJsonCreate* json);

/**
 * Sets object one level above current object as current object
 * @param json      SsdsJsonCreate structure holding the json structure in memory
 */
void ssds_js_cr_upper_obj(SsdsJsonCreate* json);

/**********************************************************************************************
 * Json read - this part describes functions used when parsing recieved json structure        *
 **********************************************************************************************/
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

/**
 * GSList of SsdsJsonInstall
 */
typedef struct SsdsJsonAnswer SsdsJsonAnswer;

/**
 * Structure used for parsing incoming answer from server
 */
typedef struct SsdsJsonInstall SsdsJsonInstall;

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

struct SsdsJsonAnswer{
  char* name;       /** < name of app that was requested for install */
  GSList* pkgList;  /** < holds list of SsdsJsonInstall */
};

struct SsdsJsonInstall{
  char* pkg_name;   /** < name of requested package */
  char* pkg_loc;    /** < location of package on repository */
  char* base_url;   /** < base_url for package download or null */
  char* metalink;   /** < metalink or null */
};
/**
 * Returns new SsdsJsonRead structure - this structure is needed during the whole process of json parsing
 * @return          newly allocated SsdsJsonCreate structure
 */
SsdsJsonRead* ssds_js_rd_init();

/**
 * Parses string representing json structure into SsdsJsonRead structure.
 * @param buffer    char* Json structure in a string
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @return          Returns TRUE if the recieved json is valid, FALSE otherwise - TODO
 */
gboolean ssds_js_rd_parse(char* buffer, SsdsJsonRead* json);

/**
 * Extracts status code from json.
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @return          status code from recieved json
 */
int ssds_js_rd_get_code(SsdsJsonRead* json);

/**
 * Finds all elements from recieved json. x_path is a string describing path to the elements. 
 * All elements found are placed in a GList.
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @param x_path    JsonPath format as a string - TODO
 * @return          GList of all elements found - this needs to be freed afterwards
 */
GList* ssds_js_rd_find(SsdsJsonRead* json, char* x_path);

// ================================================================================================




/******************************************************************/
/* json_read part - implementation of functions in json_read.c    */
/******************************************************************/


/**
 * Gathers control code from provided SsdsJsonRead structure
 * @param json      SsdsJsonRead*
 * @return          int
 */
int ssds_js_rd_get_code(SsdsJsonRead* json);

/**
 * Used by server to get names of packages requested by client.
 * Dependency solving will be run on these
 * @param pkgs      SsdsPkgInfo*
 * @param json      SsdsJsonRead*
 */
void ssds_js_rd_get_packages(SsdsPkgInfo* pkgs, SsdsJsonRead* json);

/**
 * Used by server to get information about repos owned by client.
 * @param json      SsdsJsonRead*
 * @param list      SsdsRepoInfoList*
 */
void ssds_js_rd_repo_info(SsdsJsonRead* json, SsdsRepoInfoList* list);

/**
 * Returns newly allocated SsdsPkgInfo
 * @return          SsdsPkgInfo*
 */
SsdsPkgInfo* ssds_js_rd_pkginfo_init();

/**
 * Returns newly allocated SsdsRepoInfo
 * @return          SsdsRepoInfo*
 */
SsdsRepoInfo* ssds_js_rd_repoinfo_init();

/**
 * Returns newly allocated SsdsRepoInfoList* used as a list of SsdsRepoInfo
 * @return          SsdsRepoInfoList*
 */
SsdsRepoInfoList* ssds_js_rd_list_init();


/**
 * Returns newly allocated structure to store list of packages to be installed on client side
 * @return          SsdsJsonAnswer*
 */
SsdsJsonAnswer* ssds_js_rd_answer_init();

/**
 * Returns newly allocated structure that holds info about one particular rpm to be installed.
 * This structure is used in SsdsJsonAnswer in a GSList of SsdsJsonInstall
 * @return          SsdsJsonInstall
 */
SsdsJsonInstall* ssds_js_rd_install_init();

/**
 * Parses incoming json and creates a list of packages to be installed for one app
 * @param ans_list  SsdsJsonAnswer structure that holds names and adresses of all packages to be installed with one app
 * @param json      SsdsJsonRead* holds the incoming json structure that will be parsed
 * @param nmr       There can be more than one app in one request from client. For this reason, there is function
 *                  ssds_js_rd_get_count to get number of objects holding particular apps. With this number simply go through all
 *                  objects in a loop and use nmr as an order indicator.
 */
void ssds_js_rd_parse_answer(SsdsJsonAnswer* ans_list, SsdsJsonRead* json, int nmr);

/**
 * Returns number of objects or elements in an array specified by name
 * @param json      SsdsJsonRead* that holds json structure
 * @param name      string holding name of array in the json structure
 * @return          int - count of objects or elements in the array
 */
int ssds_js_rd_get_count(SsdsJsonRead* json, char* name);

/**
 * Redeclaration of strcmp due to various warning in compilation
 */
int ssds_strcmp(gconstpointer a, gconstpointer b);  

#ifdef __cplusplus
}
#endif

#endif /* _JSON_HANLDER_H */
