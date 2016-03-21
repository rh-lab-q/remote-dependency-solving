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
} RdsJsonUrlType;

/**********************************************************************************************
 * Json create - this part describes functions used when creating json structure from scratch *
 **********************************************************************************************/

/**
 * Structure used for creating message in json to be sent over the network 
 */
typedef struct RdsJsonCreate RdsJsonCreate;

struct RdsJsonCreate{
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
}RdsJsInsType;

/**
 * Returns new RdsJsonCreate structure - this structure is needed during the whole process of json creation
 * @return          newly allocated RdsJsonCreate structure
 */
RdsJsonCreate* rds_js_cr_init(int code);

/**
 * Adds a new element into currArray from RdsJsonCreate structure
 * Array that is created last is set to be currArray. For inserting elements to defferent array use rds_js_cr_switch_array
 * @param json      RdsJsonCreate structure holding the json structure in memory
 * @param type      RdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 */
void rds_js_cr_add_array_member(RdsJsonCreate* json, int type, void* data);

/**
 * Adds a new element into currObj from RdsJsonCreate structure. 
 * Works the same way as rds_js_cr_add_array_member.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 * @param type      RdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void rds_js_cr_add_obj_member(RdsJsonCreate* json, int type, void* data, const char* name);

/**
 * Adds new element into top level data object. It can be array, object, string, bool or int.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 * @param type      RdsJsInsType - integer identifying type of data inserted as a new element
 * @param data      data to be inserted - string, int, or bool - for object and array use NULL
 * @param name      string to be used as a name for new element
 */
void rds_js_cr_new_data(RdsJsonCreate* json, int type, const char* name, void* data);

/**
 * Finds an array anywhere in the json structure and sets json->currArray as the currently active array.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 * @param name      String used as the array identifier
 * @return          TRUE if array with given name is found and set, otherwise FALSE
 */
gboolean rds_js_cr_switch_array(RdsJsonCreate* json, const char* name);

/**
 * Sets array one level above the current array as the current array.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 */
void rds_js_cr_upper_array(RdsJsonCreate* json);

/**
 * Sets object one level above current object as current object
 * @param json      RdsJsonCreate structure holding the json structure in memory
 */
void rds_js_cr_upper_obj(RdsJsonCreate* json);

/**
 * Converts data in memory represented by RdsJsonCreate to string.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 */
char* rds_js_cr_to_string(RdsJsonCreate* json);

/**
 * Function used for debugging - use it to dump the created json to stdout.
 * @param json      RdsJsonCreate structure holding the json structure in memory
 */
void rds_js_cr_dump(RdsJsonCreate* json);

/**********************************************************************************************
 * Json read - this part describes functions used when parsing recieved json structure        *
 **********************************************************************************************/
/** 
 * Structure used for parsing message recieved through socket and converting it to json structure 
 */
typedef struct RdsJsonRead RdsJsonRead;

struct RdsJsonRead{
  JsonParser* parser;         /** < parser holds RdsJsonRead structure */
  JsonNode* rootNode;         /** < root of json - used to find objects */
  JsonNode* currNode;         /** < currently used node */
  JsonNode* dataNode;         /** < data node - often used */
  JsonObject* currObj;        /** < currently used object */
  JsonObject* dataObj;        /** < data object - often used for adding new objects */
};

/**
 * Returns new RdsJsonRead structure - this structure is needed during the whole process of json parsing
 * @return          newly allocated RdsJsonCreate structure
 */
RdsJsonRead* rds_json_rd_init();

/**
 * Parses string representing json structure into RdsJsonRead structure.
 * @param buffer    Json structure in a string
 * @param json      RdsJsonRead structure holding parsed json in memory
 * @return          Returns TRUE if the recieved json is valid, FALSE otherwise - TODO
 */
gboolean rds_rd_parse(char* buffer, RdsJsonRead* json);

/**
 * Extracts status code from json.
 * @param json      RdsJsonRead structure holding parsed json in memory
 * @return          status code from recieved json
 */
int rds_rd_get_code(RdsJsonRead* json);

/**
 * Finds all elements from recieved json. x_path is a string describing path to the elements. 
 * All elements found are placed in a GList.
 * @param json      RdsJsonRead structure holding parsed json in memory
 * @param x_path    JsonPath format as a string - TODO
 * @return          GList of all elements found - this needs to be freed afterwards
 */
GList* rds_js_rd_find(RdsJsonRead* json, char* x_path);