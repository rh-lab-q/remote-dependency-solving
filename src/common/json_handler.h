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

#include <glib.h>
#include <glib-2.0/glib.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-types.h>
	
//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>


/** Type of url from .repo file located in /etc/yum.repos.d/ */
typedef enum {
    BASEURL = 1, /**< #baseurl in .repo file */
    MIRRORLIST,  /**< #mirrorlist in .repo file */
    METALINK     /**< #metalink in .repo file*/
} JsonUrlType;

/*******************************************************************/
/* json_create part - implementation of functions in json_create.c */
/*******************************************************************/

/** Structure used for creating message in json to be sent over the network */
typedef struct JsonCreate JsonCreate;

struct JsonCreate {
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
}JsInsType;


/**
 * Returns new JsonCreate structure - this structure is needed during the whole process of json creation
 * @return          newly allocated JsonCreate structure
 */
JsonCreate* js_cr_init(int code);

/**
 * Converts data in memory represented by JsonCreate to string.
 * @param json      JsonCreate structure holding the json structure in memory
 */
char* js_cr_to_string(JsonCreate* json);

/**
 * Function used for debugging - use it to dump the created json to stdout.
 * @param json      JsonCreate structure holding the json structure in memory
 */
void js_cr_dump(JsonCreate* json);

/** 
 * Inserts control code sequence into json_create structure.
 * The code cannot be omitted at any circumstances
 * @param json      JsonCreate*
 * @param code      int
 */
void js_cr_insert_code(JsonCreate* json, int code);

/**
 * Adds architecture type and release version into json_create structure.
 * @param json		JsonCreate*
 * @param arch		char*
 * @param release	char*
 */
void js_cr_gen_id(JsonCreate* json, char* arch, char* release);

/**
 * Adds architecture type and release version into json_create structure.
 * @param json		JsonCreate*
 * @param bytes_read	int
 */
void js_cr_set_read_bytes(JsonCreate* json, int bytes_read);

/**
 * Adds string message into json structure (just for error and warning messages).
 * @param json  JsonCreate*
 * @param msg	char*
 */
void js_cr_set_message(JsonCreate* json, char *msg);

/** 
 * Adds name of package requested by client into json_create structure.
 * Adds only one name at a time - needs to be called in a loop
 * @param json      JsonCreate*
 * @param package   char*
 */
void js_cr_add_package(JsonCreate* json, char* package);

/** 
 * Adds infotmation about one repo located on client machine.
 * Adds only one repo structutre at a time - needs to be called in a loop
 * @param json      JsonCreate*
 * @param url       char**
 * @param name      char*
 * @param type      int
 * @param url_count int
 */
void js_cr_add_repo(JsonCreate* json,char** url, char* name, int type, int url_count);


/** 
 * Adds object for packages requested by client into json_create structure.
 * Used by js_pkgs_insert on server side
 * @param json      JsonCreate*
 */
void js_cr_pkgs_init(JsonCreate* json);

/** 
 * Adds one package into install_pkgs in json_create on server side.
 * One package includes information about packages that need to be installed, erased or upgraded
 * @param json      JsonCreate*
 * @param goal      HyGoal*
 * @param name      const char*
 */
void js_cr_pkgs_insert(JsonCreate* json,HyGoal* goal);

/** 
 * Converts json_create into string so that it can be sent over network
 * @param json      JsonCreate*
 * @return          char*
 */
char* js_cr_to_string(JsonCreate* json);

/** 
 * Used for debugging json_create.
 * Prints whole structure to stdout
 * @param json      JsonCreate*
 */
void js_cr_dump(JsonCreate* json);


/**
 * These functions can be used for more flexible creation of JSON structures
 */

/**
 * Adds a new element into currArray from JsonCreate structure
 * Array that is created last is set to be currArray. For inserting elements to defferent array use js_cr_switch_array
 * @param json      JsonCreate structure holding the json structure in memory
 * @param type      JsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 */
void js_cr_add_array_member(JsonCreate* json, int type, void* data);

/**
 * Adds a new element into currObj from JsonCreate structure. 
 * Works the same way as js_cr_add_array_member.
 * @param json      JsonCreate structure holding the json structure in memory
 * @param type      JsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void js_cr_add_obj_member(JsonCreate* json, int type, void* data, const char* name);

/**
 * Adds new element into top level data object. It can be array, object, string, bool or int.
 * @param json      JsonCreate structure holding the json structure in memory
 * @param type      JsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void js_cr_new_data(JsonCreate* json, int type, const char* name, void* data);

/**
 * Finds an array anywhere in the json structure and sets json->currArray as the currently active array.
 * @param json      JsonCreate structure holding the json structure in memory
 * @param name      String used as the array identifier
 * @return          TRUE if array with given name is found and set, otherwise FALSE
 */
gboolean js_cr_switch_array(JsonCreate* json, const char* name);

/**
 * Sets array one level above the current array as the current array.
 * @param json      JsonCreate structure holding the json structure in memory
 */
void js_cr_upper_array(JsonCreate* json);

/**
 * Sets object one level above current object as current object
 * @param json      JsonCreate structure holding the json structure in memory
 */
void js_cr_upper_obj(JsonCreate* json);

/**********************************************************************************************
 * Json read - this part describes functions used when parsing recieved json structure        *
 **********************************************************************************************/
/** 
 * Used by client to save names of packages requested by user.
 * Names are parsed from parameters 
 */
typedef struct PkgInfo PkgInfo;

/** 
 * Structure for info about one .repo file located on client machine.
 * List of repos is in RepoInfoList
 */
typedef struct RepoInfo RepoInfo;

/** 
 * GSList of RepoInfo 
 */
typedef struct RepoInfoList RepoInfoList;

/** 
 * Structure used for parsing message recieved through socket and converting it to json structure 
 */
typedef struct JsonRead JsonRead;

/**
 * GSList of JsonPkg
 */
typedef struct JsonAnswer JsonAnswer;

/**
 * Structure used for parsing incoming answer from server
 */
typedef struct JsonPkg JsonPkg;

struct PkgInfo {
    char** packages;  /**< names of packages requested by user */
    int length;       /**< number of packages */
};

struct RepoInfo {
    char** urls;      /**< array of char pointers in case url is baseurl - there can be more than one baseurl for one repo */
    char* name;       /**< name of repo */
    int count;        /**< number of addresses in urls */
    int type;         /**< JsonUrlType */
};

struct RepoInfoList {
    GSList* repoInfoList; /** < GSList of RepoInfo */
};

struct JsonRead {
    JsonParser* parser;         /** < parser holds JsonRead structure */
    JsonNode* rootNode;         /** < root of json - used to find objects */
    JsonNode* currNode;         /** < currently used node */
    JsonNode* dataNode;         /** < data node - often used */
    JsonObject* currObj;        /** < currently used object */
    JsonObject* dataObj;        /** < data object - often used for adding new objects */
};

struct JsonAnswer {
    char* name;       /** < name of app that was requested for install */
    GSList* pkgList;  /** < holds list of JsonPkg */
};

struct JsonPkg {
    char* pkg_name;   /** < name of requested package */
    char* pkg_loc;    /** < location of package on repository */
    char* base_url;   /** < base_url for package download or null */
    char* metalink;   /** < metalink or null */
};
/**
 * Returns new JsonRead structure - this structure is needed during the whole process of json parsing
 * @return          newly allocated JsonCreate structure
 */
JsonRead* js_rd_init();

/**
 * Parses string representing json structure into JsonRead structure.
 * @param buffer    char* Json structure in a string
 * @param json      JsonRead structure holding parsed json in memory
 * @return          Returns TRUE if the recieved json is valid, FALSE otherwise - TODO
 */
gboolean js_rd_parse(char* buffer, JsonRead* json);

/**
 * Extracts status code from json.
 * @param json      JsonRead structure holding parsed json in memory
 * @return          status code from recieved json
 */
int js_rd_get_code(JsonRead* json);

/**
 * Extracts count of send bytes from json.
 * @param json      JsonRead structure holding parsed json in memory
 * @return          bytes count from json
 */
int js_rd_get_read_bytes(JsonRead* json);

/**
 * Extracts string message from json.
 * @param json      JsonRead structure holding parsed json in memory
 * @return          message
 */
char* js_rd_get_message(JsonRead* json);

/**
 * Finds all elements from recieved json. x_path is a string describing path to the elements. 
 * All elements found are placed in a GList.
 * @param json      JsonRead structure holding parsed json in memory
 * @param x_path    JsonPath format as a string - TODO
 * @return          GList of all elements found - this needs to be freed afterwards
 */
GList* js_rd_find(JsonRead* json, char* x_path);

// ================================================================================================




/******************************************************************/
/* json_read part - implementation of functions in json_read.c    */
/******************************************************************/


/**
 * Gathers control code from provided JsonRead structure
 * @param json      JsonRead*
 * @return          int
 */
int js_rd_get_code(JsonRead* json);

/**
 * Used by server to get names of packages requested by client.
 * Dependency solving will be run on these
 * @param pkgs      PkgInfo*
 * @param json      JsonRead*
 */
void js_rd_get_packages(char** pkgs, JsonRead* json);

/**
 * Used by server to get information about repos owned by client.
 * @param json      JsonRead*
 * @param list      RepoInfoList*
 */
void js_rd_repo_info(JsonRead* json, RepoInfoList* list);

/**
 * Returns newly allocated PkgInfo
 * @return          PkgInfo*
 */
PkgInfo* js_rd_pkginfo_init();

/**
 * Returns newly allocated RepoInfo
 * @return          RepoInfo*
 */
RepoInfo* js_rd_repoinfo_init();

/**
 * Returns newly allocated RepoInfoList* used as a list of RepoInfo
 * @return          RepoInfoList*
 */
RepoInfoList* js_rd_list_init();


/**
 * Returns newly allocated structure to store list of packages to be installed on client side
 * @return          JsonAnswer*
 */
JsonAnswer* js_rd_answer_init();

/**
 * Returns newly allocated structure that holds info about one particular rpm to be installed.
 * This structure is used in JsonAnswer in a GSList of JsonPkg
 * @return          JsonPkg
 */
JsonPkg* js_rd_pkg_init();

/**
 * Parses incoming json and creates a list of packages to be installed for one app
 * @param ans_list  JsonAnswer structure that holds names and adresses of all packages to be installed with one app
 * @param json      JsonRead* holds the incoming json structure that will be parsed
 * @param nmr       There can be more than one app in one request from client. For this reason, there is function
 *                  js_rd_get_count to get number of objects holding particular apps. With this number simply go through all
 *                  objects in a loop and use nmr as an order indicator.
 */
GSList* js_rd_parse_answer(const char *name, JsonRead* json);

/**
 * Returns number of objects or elements in an array specified by name
 * @param json      JsonRead* that holds json structure
 * @param name      string holding name of array in the json structure
 * @return          int - count of objects or elements in the array
 */
int js_rd_get_count(JsonRead* json, char* name);

/**
 * Redeclaration of strcmp due to various warning in compilation
 */
int rds_strcmp(gconstpointer a, gconstpointer b);  

#ifdef __cplusplus
}
#endif

#endif /* _JSON_HANLDER_H */
