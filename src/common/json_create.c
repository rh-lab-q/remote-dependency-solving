/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015 
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
#include <stdio.h>
#include <string.h>

#include "json_handler.h"
#include "log_handler.h"
#include "globvar.h"


JsonCreate* js_cr_init(int code) {
    JsonCreate* new;
    JsonNode * addCode;
    JsonNode * add;
    
    new = (JsonCreate*)malloc(sizeof(JsonCreate));
    new->generator = json_generator_new();
    new->rootNode = json_node_new(JSON_NODE_OBJECT);
    new->rootObj = json_object_new();
    json_node_take_object(new->rootNode, new->rootObj);
    json_generator_set_root(new->generator, new->rootNode); //set new node as root of the generator

    //create code object
    addCode = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(addCode, (gint64)code);
    json_object_set_member(new->rootObj, (gchar*)"code", addCode);
    new->codeNode = addCode;

    //create data object
    add = json_node_new(JSON_NODE_OBJECT); //TODO - test
    new->dataNode = add;
    json_object_set_member(new->rootObj, (gchar*)"data", add);
    new->dataObj = json_object_new();
    json_node_take_object(add, new->dataObj);  

    new->currNode = new->dataNode;

    return new;
}

void js_cr_add_array_member(JsonCreate* json, int type, void* data) {
    switch(type)
    {
        case JS_OBJ: {
            JsonNode* new_node;
            JsonObject* new_obj;
            
            new_node = json_node_new(JSON_NODE_OBJECT);
            new_obj = json_object_new();
            json_node_take_object(new_node, new_obj);

            json_array_add_object_element(json->currArray, new_obj);
            json->currObj = new_obj;
            json_node_set_parent(new_node, json->currNode);
            json->currNode = new_node;
        break;
        }
        
        case JS_ARRAY: {
            JsonNode* new_node;
            JsonArray* new_arr;
            
            new_node = json_node_new(JSON_NODE_ARRAY);
            new_arr = json_array_new();
            json_node_take_array(new_node, new_arr);

            json_array_add_array_element(json->currArray, new_arr);
            json->currArray = new_arr;
            json_node_set_parent(new_node, json->currNode);
            json->currNode = new_node;
        break;
        }
        case JS_STRING: 
            json_array_add_string_element(json->currArray, (const gchar*)data);
        break;
        
        case JS_BOOL: 
            json_array_add_boolean_element(json->currArray, *((gboolean*)data));
        break;
        
        case JS_INT: 
            json_array_add_int_element(json->currArray, *((guint*)data));
        break;
    }
}


void js_cr_add_obj_member(JsonCreate* json, int type, void* data, const char* name) {
    switch(type) {
        case JS_STRING:
            json_object_set_string_member(json->currObj, (const gchar*)name, (const char*)data);
        break;

        case JS_BOOL:
            if((*(gboolean*)data)==TRUE)
                json_object_set_boolean_member(json->currObj, (const gchar*)name, TRUE);
            else
                json_object_set_boolean_member(json->currObj, (const gchar*)name, FALSE);
        break;

        case JS_INT:
            json_object_set_int_member(json->currObj, (const gchar*)name, *((guint*)data));
        break;

        case JS_OBJ: {
            JsonNode* new_node;
            JsonObject* new_obj;
            
            new_node = json_node_new(JSON_NODE_OBJECT);
            new_obj = json_object_new();
            json_node_take_object(new_node, new_obj);

            json_object_set_object_member(json->currObj, (gchar*)name, new_obj);

            json->currObj = new_obj;
            json_node_set_parent(new_node, json->currNode);
            json->currNode = new_node;
        break;
        } 
        
        case JS_ARRAY: {
            JsonNode* new_node;
            JsonArray* new_arr;
            
            new_node = json_node_new(JSON_NODE_ARRAY);
            new_arr = json_array_new();
            json_node_take_array(new_node, new_arr);

            json_object_set_array_member(json->currObj, (gchar*)name, new_arr);

            json->currArray = new_arr;
            json_node_set_parent(new_node, json->currNode);
            json->currNode = new_node;
        break;
        }
    }
}

void js_cr_new_data(JsonCreate* json, int type, const char* name, void* data) {
    switch(type) {
    case JS_ARRAY: {
        JsonNode* new_node;
        JsonArray* new_arr;
        
        new_node = json_node_new(JSON_NODE_ARRAY);
        new_arr = json_array_new();
        json_node_take_array(new_node, new_arr);

        json_object_set_array_member(json->dataObj, (const gchar*)name, new_arr);
        json->currArray = new_arr;
        json_node_set_parent(new_node, json->currNode);
        json->currNode = new_node;
    break;
    }
    
    case JS_OBJ: {
        JsonNode* new_node;
        JsonObject* new_obj;
        
        new_node = json_node_new(JSON_NODE_OBJECT);
        new_obj = json_object_new();
        json_node_take_object(new_node, new_obj);

        json_object_set_object_member(json->dataObj, (const gchar*)name, new_obj);
        json->currObj = new_obj;
        json_node_set_parent(new_node, json->currNode);
        json->currNode = new_node;
    break;
    } 
    
    case JS_STRING:
        json_object_set_string_member(json->dataObj, (const gchar*) name, (const gchar*)data);
    break;
    
    case JS_BOOL:
        if((*(gboolean*)data)==TRUE)
            json_object_set_boolean_member(json->dataObj, (const gchar*)name, TRUE);
        else
            json_object_set_boolean_member(json->dataObj, (const gchar*)name, FALSE);
    break;
    
    case JS_INT:
        json_object_set_int_member(json->currObj, (const gchar*)name, *((guint*)data));
    break;
    }
}

gboolean js_cr_switch_array(JsonCreate* json, const char* name) {
    JsonPath* new_path;
    const char* default_path="$.data..";
    int length; 
    char* full_path; 

    length=strlen(default_path)+strlen(name);
    full_path=(char*)malloc((length+1)*sizeof(char));
    
    strncpy(full_path, default_path, strlen(default_path)+1);
    strncat(full_path, name, strlen(name));
    
    //json path for finding 
    new_path = json_path_new();
    if(!json_path_compile(new_path, full_path, NULL))
        return FALSE;

    //get the result
    JsonNode* root;
    JsonNode* result;
    JsonArray* result_arr;
    JsonNode* selected_node;
    
    root = json_generator_get_root(json->generator);
    result = json_path_match(new_path, root);
    result_arr = json_node_get_array(result);
    selected_node = json_array_get_element(result_arr, 0);
    
    json->currArray = json_node_get_array(selected_node);

    //clean up
    free(full_path);
    g_object_unref(new_path);
    json_node_free(result);
    return TRUE;
}

void js_cr_upper_array(JsonCreate* json) {
    do {
        json->currNode = json_node_get_parent(json->currNode);
    }while(!JSON_NODE_HOLDS_ARRAY(json->currNode));

    json->currArray = json_node_get_array(json->currNode);
}

void js_cr_upper_obj(JsonCreate* json)
{
    do {
        json->currNode = json_node_get_parent(json->currNode);
    }while(!JSON_NODE_HOLDS_OBJECT(json->currNode));

    json->currObj = json_node_get_object(json->currNode);
}

char* js_cr_to_string(JsonCreate* json)
{
    gsize len;
    char* data;
    
    data = (char*)json_generator_to_data(json->generator, &len);
    return data;
}

void js_cr_dump(JsonCreate* json)//this will always dump error when some array or object is empty - just ignore it
{
    gchar *data;
    
    json_generator_set_pretty(json->generator, 1);
    data = json_generator_to_data (json->generator, NULL);
    printf("%s\n", (char*)data);
    
    free(data);
}

// =============================================================================================
// original json ˇˇˇ


void js_cr_insert_code(JsonCreate* json, int code) {
    json_node_set_int(json->codeNode, (gint64)code);
}

void js_cr_gen_id(JsonCreate* json, char* arch, char* release) {
    JsonNode* json_arch;
    JsonNode* json_rel;
    
    json_arch = json_node_new(JSON_NODE_VALUE);
    json_node_set_string(json_arch, (gchar*)arch);
    json_object_set_member(json->dataObj, (gchar*)"arch", json_arch);

    json_rel = json_node_new(JSON_NODE_VALUE);
    json_node_set_string(json_rel,(gchar*) release);
    json_object_set_member(json->dataObj, (gchar*)"release", json_rel);
}

void js_cr_set_read_bytes(JsonCreate* json, int bytes_read) {
    JsonNode* json_bytes;
    
    json_bytes = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(json_bytes, (gint64)bytes_read);
    json_object_set_member(json->dataObj, (gchar*)"read", json_bytes);
}
 
void js_cr_set_message(JsonCreate* json, char *msg) {
  JsonNode* json_msg;
  
  json_msg = json_node_new(JSON_NODE_VALUE);
  json_node_set_string(json_msg, (gchar*)msg);
  json_object_set_member(json->dataObj, (gchar*)"message", json_msg);
}
                                                 
void js_cr_add_package(JsonCreate* json,char* package) {
    //adding package for the first time
    if(!json_object_has_member(json->dataObj,(const gchar*)"req_pkgs")) {
        JsonNode* new_node;
        JsonArray* new_array;
        
        new_node = json_node_new(JSON_NODE_ARRAY);
        json_object_set_member(json->dataObj, (gchar*)"req_pkgs", new_node);
        json->currNode = new_node;

        new_array = json_array_new();
        json_node_take_array(new_node, new_array);
        json->currArray = new_array;
    }
    //if some packages are already there, only set the right array
    else
        json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)"req_pkgs");
    
    JsonNode* pkg;

    pkg = json_node_new(JSON_NODE_VALUE);
    json_node_set_string(pkg,(gchar*)package);
    json_array_add_element(json->currArray, pkg);
}


void js_cr_add_repo(JsonCreate* json,char** url, char* name, int type, int url_count) {
    if(!json_object_has_member(json->dataObj,(const gchar*)"repolist")) {
        JsonNode* new_node;
        JsonArray* new_array;
        
        new_node = json_node_new(JSON_NODE_ARRAY);
        json_object_set_member(json->dataObj, (gchar*)"repolist", new_node);
        json->currNode = new_node;

        new_array = json_array_new();
        json_node_take_array(new_node, new_array);
        json->currArray = new_array;
    }
    else
        json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)"repolist");

    JsonNode* new_repo;
    JsonObject* new_repo_obj;
    JsonNode* new_inside;
    JsonArray* new_array;
    JsonNode* repo_name;
    JsonNode* repo_type;
    
    new_repo = json_node_new(JSON_NODE_OBJECT);
    new_repo_obj = json_object_new();
    json_node_take_object(new_repo, new_repo_obj);
    json_array_add_object_element(json->currArray, new_repo_obj);
    json->currObj = new_repo_obj;
    
    new_inside = json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(json->currObj, (gchar*)"repo_url", new_inside);
    json->currNode = new_inside;

    new_array = json_array_new();
    json_node_take_array(new_inside, new_array);
    json->currArray = new_array;

    if(type == 1) {
        int i;
        for(i = 0; i < url_count; i++)
            json_array_add_string_element(json->currArray, (gchar*)url[i]);
    }
    else
        json_array_add_string_element(json->currArray, (gchar*)url[0]);

    repo_name = json_node_new(JSON_NODE_VALUE);
    json_node_set_string(repo_name, (gchar*)name);
    json_object_set_member(json->currObj, (gchar*)"name", repo_name);

    repo_type = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(repo_type, (gint64)type);
    json_object_set_member(json->currObj, (gchar*)"type", repo_type);
}


void js_cr_pkgs_init(JsonCreate* json) {
    if(!json_object_has_member(json->dataObj,(const gchar*)"install_pkgs")) {
        JsonNode* new_node;
        JsonArray* new_array;
        
        new_node = json_node_new(JSON_NODE_ARRAY);
        json_object_set_member(json->dataObj, (gchar*)"install_pkgs", new_node);
        json->currNode = new_node;

        new_array = json_array_new();
        json_node_take_array(new_node, new_array);
        json->currArray = new_array;
    }
}


void js_cr_pkgs_insert(JsonCreate* json,HyGoal* goal)
{
    int count;
    
    for(int i=0; i<=JS_ARR_OBSOLETE; i++) {
        HyPackageList goal_pkgs; 
        HyPackage pkg;
        int pkg_count;
        
        if(!json_object_has_member(json->dataObj,(const gchar*)JsArrayStr[i]))
            js_cr_new_data(json, JS_ARRAY, JsArrayStr[i], NULL);
        else
            json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)JsArrayStr[i]);

        goal_pkgs = hy_packagelist_create();
        switch(i)
        {
            case JS_ARR_INSTALL:
                goal_pkgs = hy_goal_list_installs(*goal);
                count += hy_packagelist_count(goal_pkgs);
            break;

            case JS_ARR_UPGRADE:
                goal_pkgs = hy_goal_list_upgrades(*goal);
                count += hy_packagelist_count(goal_pkgs);
            break;

            case JS_ARR_ERASE:
                goal_pkgs = hy_goal_list_erasures(*goal);
                count += hy_packagelist_count(goal_pkgs);
            break;

            case JS_ARR_OBSOLETE:
                goal_pkgs = hy_goal_list_obsoleted(*goal);
                count += hy_packagelist_count(goal_pkgs);
            break;

            case JS_ARR_UNNEEDED:
                goal_pkgs = hy_goal_list_unneeded(*goal);
                count += hy_packagelist_count(goal_pkgs);
            break;
        }

        //adding objects to install array
        pkg_count=hy_packagelist_count(goal_pkgs);
        if(pkg_count >0) {
            for(int i = pkg_count-1; i >= 0; i--) {
                pkg = hy_packagelist_get(goal_pkgs, i);

                js_cr_add_array_member(json, JS_OBJ, NULL);
                js_cr_add_obj_member(json, JS_STRING, hy_package_get_name(pkg), (gchar*)"pkg_name");
                js_cr_add_obj_member(json, JS_STRING, hy_package_get_location(pkg), (gchar*)"pkg_loc");

                if(hy_package_get_baseurl(pkg) == NULL) {
                    js_cr_add_obj_member(json, JS_STRING, NULL, (gchar*)"base_url");
                    js_cr_add_obj_member(json, JS_STRING, hy_package_get_reponame(pkg), (gchar*)"metalink");
                }
                else {
                    js_cr_add_obj_member(json, JS_STRING, hy_package_get_baseurl(pkg), (gchar*)"base_url");
                    js_cr_add_obj_member(json, JS_STRING, hy_package_get_reponame(pkg), (gchar*)"metalink");
                }
            }
        }
    }
    rds_log(logINFO, "Total packages to install: %d\n", count);
}

void js_cr_dispose(JsonCreate* json)
{
    json_node_free(json->rootNode);
    g_object_unref(json->generator);
}

int rds_strcmp(gconstpointer a, gconstpointer b) {
    char *s_a = (char *)a;
    char *s_b = (char *)b;
    return strcmp(s_a, s_b);	
}
