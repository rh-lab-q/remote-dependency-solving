/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký
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

//GLIB
#include <glib.h>
#include <glib-2.0/glib.h>

//JSON-GLIB
#include <json-glib/json-glib.h>
#include <json-glib/json-types.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** 
 *Type of url from .repo file located in /etc/yum.repos.d/ 
 */
typedef enum {
  SSDS_BASEURL = 1, /**< #baseurl in .repo file */
  SSDS_MIRRORLIST,  /**< #mirrorlist in .repo file */
  SSDS_METALINK     /**< #metalink in .repo file*/
} SsdsJsonUrlType;

/**********************************************************************************************
 * Json create - this part describes functions used when creating json structure from scratch *
 **********************************************************************************************/

/**
 * Structure used for creating message in json to be sent over the network 
 */
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

/**********************************************************************************************
 * Json read - this part describes functions used when parsing recieved json structure        *
 **********************************************************************************************/
/** 
 * Structure used for parsing message recieved through socket and converting it to json structure 
 */
typedef struct SsdsJsonRead SsdsJsonRead;

struct SsdsJsonRead{
  JsonParser* parser;         /** < parser holds SsdsJsonRead structure */
  JsonNode* rootNode;         /** < root of json - used to find objects */
  JsonNode* currNode;         /** < currently used node */
  JsonNode* dataNode;         /** < data node - often used */
  JsonObject* currObj;        /** < currently used object */
  JsonObject* dataObj;        /** < data object - often used for adding new objects */
};

/**
 * Returns new SsdsJsonRead structure - this structure is needed during the whole process of json parsing
 * @return          newly allocated SsdsJsonCreate structure
 */
SsdsJsonRead* ssds_json_rd_init();

/**
 * Parses string representing json structure into SsdsJsonRead structure.
 * @param buffer    Json structure in a string
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @return          Returns TRUE if the recieved json is valid, FALSE otherwise - TODO
 */
gboolean ssds_rd_parse(char* buffer, SsdsJsonRead* json);

/**
 * Extracts status code from json.
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @return          status code from recieved json
 */
int ssds_rd_get_code(SsdsJsonRead* json);

/**
 * Finds all elements from recieved json. x_path is a string describing path to the elements. 
 * All elements found are placed in a GList.
 * @param json      SsdsJsonRead structure holding parsed json in memory
 * @param x_path    JsonPath format as a string - TODO
 * @return          GList of all elements found - this needs to be freed afterwards
 */
GList* ssds_js_rd_find(SsdsJsonRead* json, char* x_path);