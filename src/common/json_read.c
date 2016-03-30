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
#include <string.h>
#include <stdlib.h>

#include "json_handler.h"
#include "util.h"

JsonRead* js_rd_init() {
    JsonRead* new;

    new = (JsonRead*)rds_malloc(sizeof(JsonRead));
    new->parser = json_parser_new();
    return new;
}

gboolean js_rd_parse(char* buffer, JsonRead* json) {
    GError *error = NULL;
    gboolean ret;
    JsonObject* obj;
    
    ret = json_parser_load_from_data(json->parser, (const gchar*)buffer, -1, &error);
    if(!ret)
        return EXIT_FAILURE;

    json->rootNode = json_parser_get_root(json->parser);

    obj=json_node_get_object(json->rootNode);
    json->dataNode=json_object_get_member(obj, (gchar*)"data");
    json->dataObj=json_node_get_object(json->dataNode);
    return ret;
}

int js_get_code(JsonRead* json) {
    int ret=-1;
    JsonObject* obj;
    
    obj=json_node_get_object(json->rootNode);
    if(json_object_has_member(obj, (gchar*)"code"))
        ret=(int)json_object_get_int_member(obj, "code");

    return ret;
}

int js_rd_get_read_bytes(JsonRead* json) {
    int ret=-1;
    JsonObject* obj;
    
    obj = json->dataObj;
    if(json_object_has_member(obj, (gchar*)"read"))
        ret=(int)json_object_get_int_member(obj, "read");

    return ret;
}

char* js_rd_get_message(JsonRead* json) {
    char *msg = NULL;
    JsonObject* obj;
    
    obj = json->dataObj;
    if(json_object_has_member(obj, (gchar*)"message"))
        msg = (char *)json_object_get_string_member(obj, "message");

    return msg;
}

GList* js_rd_find(JsonRead* json, char* x_path) {
    GList* ret = NULL;
    JsonPath* new_path;
    JsonNode* root;
    JsonNode* result;
    JsonArray* result_arr;
    
    new_path = json_path_new();
    json_path_compile(new_path, x_path, NULL);

    root = json_parser_get_root(json->parser);
    result = json_path_match(new_path, root);
    result_arr = json_node_get_array(result);

    ret = json_array_get_elements(result_arr);

    return ret;
}

// =================================================================================
// original json ˇˇˇ

int js_rd_get_code(JsonRead* json) {
    int ret = -1;
    JsonObject* obj;
    
    obj = json_node_get_object(json->rootNode);
    if(json_object_has_member(obj, (gchar*)"code"))
        ret = (int)json_object_get_int_member(obj, "code");

    return ret;
}

void js_rd_get_packages(char** pkgs, JsonRead* json) {
    JsonArray* array;
    guint len;
    guint i;
    
    array = json_object_get_array_member(json->dataObj, "req_pkgs");
    len = json_array_get_length(array);
    //pkgs->packages = (char**)rds_malloc(len*sizeof(char*));

    
    for(i = 0; i < len; i++) {
        pkgs[i] = (char*)malloc((strlen((char*)json_array_get_string_element(array,i)) + 1)*sizeof(char));
        char* word = (char*)json_array_get_string_element(array, i);
        strcpy(pkgs[i], word);
    }

    pkgs[++i]=NULL;
}


void js_rd_repo_info(JsonRead* json, RepoInfoList* list) {
    JsonArray*array;
    guint len;
    guint i;

    array = json_object_get_array_member(json->dataObj, "repolist");
    len = json_array_get_length(array);
    
    for(i = 0; i < len; i++) {
        RepoInfo* repo;
        JsonObject* obj;
        char* currName;
        JsonArray* url_arr;
        int j;
        
        repo = js_rd_repoinfo_init();
        obj = json_array_get_object_element(array, i);
        repo->type = (int)json_object_get_int_member(obj, "type");

        currName = (char*)json_object_get_string_member(obj, "name");
        repo->name = (char*)rds_malloc((strlen(currName)+1)*sizeof(char));
        strcpy(repo->name, currName);

        url_arr = json_object_get_array_member(obj, (gchar*)"repo_url");
        int arr_len = json_array_get_length(url_arr);
        repo->urls = (char**)rds_malloc(arr_len*(sizeof(char*)));
        
        for(j = 0; j < arr_len; j++) {
            char* word;
            
            word = (char*)json_array_get_string_element(url_arr, j);
            repo->urls[j] = (char*)rds_malloc((strlen(word)+1)*sizeof(char));
            strcpy(repo->urls[j], word);
            repo->count++;
        }

        list->repoInfoList = g_slist_append(list->repoInfoList, repo);
    }
}


PkgInfo* js_rd_pkginfo_init() {
    PkgInfo* new;
    
    new = (PkgInfo*)rds_malloc(sizeof(PkgInfo));
    new->packages = NULL;
    new->length = 0;
    return new;
}


RepoInfo* js_rd_repoinfo_init() {
    RepoInfo* new;
    
    new = (RepoInfo*)rds_malloc(sizeof(RepoInfo));
    new->count = 0;
    new->type = 0;
    new->name = NULL;
    new->urls = NULL;
    return new;
}

RepoInfoList* js_rd_list_init() {
    RepoInfoList* new;

    new = (RepoInfoList*)rds_malloc(sizeof(RepoInfoList));
    new->repoInfoList = NULL;
    return new;
}

/**********************/
/* mainly client part */
/**********************/
JsonAnswer* js_rd_answer_init() {
    JsonAnswer* new;

    new = (JsonAnswer*)rds_malloc(sizeof(JsonAnswer));
    new->name = NULL;
    new->pkgList = NULL;
    return new;
}

JsonPkg* js_rd_pkg_init() {
    JsonPkg* new;

    new = (JsonPkg*)rds_malloc(sizeof(JsonPkg));
    new->pkg_name = NULL;
    new->pkg_loc = NULL;
    new->base_url = NULL;
    new->metalink = NULL;
    return new;
}

int js_rd_get_count(JsonRead* json, char* name) {
    const char* default_path="$.data..";
    int length, ret;
    char* full_path;
    JsonPath* new_path;
    JsonNode* result;
    JsonArray* result_array, *final;

    length=strlen(default_path)+strlen(name);
    full_path=(char*)malloc((length+1)*sizeof(char));
    strncpy(full_path, default_path, strlen(default_path)+1);
    strncat(full_path, name, strlen(name));

    new_path = json_path_new();
    json_path_compile(new_path, full_path, NULL);

    result = json_path_match(new_path, json->rootNode);
    result_array = json_node_get_array(result);
    final = json_array_get_array_element(result_array, 0);
    ret = json_array_get_length(final);

    free(full_path);
    return ret;
}

GSList* js_rd_parse_answer(const char* name, JsonRead* json) {
    JsonArray* array;
    int count;
    GSList* ret;

    array = json_object_get_array_member(json->dataObj, name);
    count = js_rd_get_count(json, name);
    ret = g_slist_alloc();
    
    for(int i=0; i<count; i++) {
        JsonPkg* one_pkg;
        JsonObject* obj;
        char* pkg_name;
        char* pkg_loc;

        one_pkg = js_rd_pkg_init();
        obj = json_array_get_object_element(array, i);
        pkg_name = (char*)json_object_get_string_member(obj, "pkg_name");
        one_pkg->pkg_name = (char*)rds_malloc((strlen(pkg_name)+1)*sizeof(char));
        strcpy(one_pkg->pkg_name, pkg_name);

        //name of package location on repository
        pkg_loc = (char*)json_object_get_string_member(obj, "pkg_loc");
        one_pkg->pkg_loc = (char*)rds_malloc((strlen(pkg_loc)+1)*sizeof(char));
        strcpy(one_pkg->pkg_loc, pkg_loc);

        //baseurl or null
        if(json_object_get_null_member(obj, "base_url")) {
            char* meta;
            
            meta = (char*)json_object_get_string_member(obj, "metalink");
            one_pkg->metalink = (char*)rds_malloc((strlen(meta)+1)*sizeof(char));
            strcpy(one_pkg->metalink, meta);
        }
        else {
            char* base;
            
            base = (char*)json_object_get_string_member(obj, "base_url");
            one_pkg->base_url = (char*)rds_malloc((strlen(base)+1)*sizeof(char));
            strcpy(one_pkg->base_url, base);
        }

        ret = g_slist_append(ret, one_pkg);
    }

    return ret;
}
