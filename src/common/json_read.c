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

#include "json_handler.h"

SsdsJsonRead* ssds_js_rd_init()
{
  SsdsJsonRead* new = (SsdsJsonRead*)ssds_malloc(sizeof(SsdsJsonRead));
  new->parser = json_parser_new();
  return new;
}

gboolean ssds_js_rd_parse(char* buffer, SsdsJsonRead* json)
{
  GError *error = NULL;
    
  gboolean ret = json_parser_load_from_data(json->parser, (const gchar*)buffer, -1, &error);
  if(!ret)
    return EXIT_FAILURE;
  
  json->rootNode = json_parser_get_root(json->parser);
  
  JsonObject* obj=json_node_get_object(json->rootNode);
  json->dataNode=json_object_get_member(obj, (gchar*)"data");
  json->dataObj=json_node_get_object(json->dataNode);
  return ret;
}

int ssds_rd_get_code(SsdsJsonRead* json)
{
  int ret=-1;
  JsonObject* obj=json_node_get_object(json->rootNode);
  if(json_object_has_member(obj, (gchar*)"code"))
  ret=(int)json_object_get_int_member(obj, "code");
  
  return ret;
}

GList* ssds_js_rd_find(SsdsJsonRead* json, char* x_path)
{
  GList* ret = NULL;
  JsonPath* new_path = json_path_new();
  json_path_compile(new_path, x_path, NULL);
  
  JsonNode* root = json_parser_get_root(json->parser);
  JsonNode* result = json_path_match(new_path, root);
  JsonArray* result_arr = json_node_get_array(result);
  
  ret = json_array_get_elements(result_arr);
  
  return ret;
}

// =================================================================================
// original json ˇˇˇ

int ssds_js_rd_get_code(SsdsJsonRead* json)
{
  int ret = -1;
  JsonObject* obj = json_node_get_object(json->rootNode);
  if(json_object_has_member(obj, (gchar*)"code"))
    ret = (int)json_object_get_int_member(obj, "code");
  
  return ret;
}

void ssds_js_rd_get_packages(struct SsdsPkgInfo* pkgs, SsdsJsonRead* json)
{
  JsonArray* array = json_object_get_array_member(json->dataObj, "req_pkgs");
  guint len = json_array_get_length(array);
  pkgs->packages = (char**)ssds_malloc(len*sizeof(char*));
  
  guint i;
  for(i = 0; i < len; i++)
  {
    pkgs->packages[i] = (char*)ssds_malloc((strlen((char*)json_array_get_string_element(array,i)) + 1)*sizeof(char));
    char* word = (char*)json_array_get_string_element(array, i);
    strcpy(pkgs->packages[i], word);
    pkgs->length++;
  }
}


void ssds_js_rd_repo_info(SsdsJsonRead* json, SsdsRepoInfoList* list)
{
  JsonArray*array = json_object_get_array_member(json->dataObj, "repolist");
  guint len = json_array_get_length(array);
  
  guint i;
  for(i = 0; i < len; i++)
  {
    SsdsRepoInfo* repo = ssds_js_rd_repoinfo_init();
    
    JsonObject* obj = json_array_get_object_element(array, i);
    repo->type = (int)json_object_get_int_member(obj, "type");
    
    char* currName = (char*)json_object_get_string_member(obj, "name");
    repo->name = (char*)ssds_malloc((strlen(currName)+1)*sizeof(char));
    strcpy(repo->name, currName);
    
    JsonArray* url_arr = json_object_get_array_member(obj, (gchar*)"repo_url");
    int arr_len = json_array_get_length(url_arr);
    repo->urls = (char**)ssds_malloc(arr_len*(sizeof(char*)));
    
    int j;
    for(j = 0; j < arr_len; j++)
    {
      char* word = (char*)json_array_get_string_element(url_arr, j);
      repo->urls[j] = (char*)ssds_malloc((strlen(word)+1)*sizeof(char));
      strcpy(repo->urls[j], word);
      repo->count++;
    }
    
    list->repoInfoList = g_slist_append(list->repoInfoList, repo);
  }
}


SsdsPkgInfo* ssds_js_rd_pkginfo_init()
{
  SsdsPkgInfo* new = (SsdsPkgInfo*)ssds_malloc(sizeof(SsdsPkgInfo));
  new->packages = NULL;
  new->length = 0;
  return new;
}


SsdsRepoInfo* ssds_js_rd_repoinfo_init()
{
  SsdsRepoInfo* new = (SsdsRepoInfo*)ssds_malloc(sizeof(SsdsRepoInfo));
  new->count = 0;
  new->type = 0;
  new->name = NULL;
  new->urls = NULL;
  return new;
}

SsdsRepoInfoList* ssds_js_rd_list_init()
{
  SsdsRepoInfoList* new = (SsdsRepoInfoList*)ssds_malloc(sizeof(SsdsRepoInfoList));
  new->repoInfoList = NULL;
  return new;
}

/**********************/
/* mainly client part */
/**********************/
SsdsJsonAnswer* ssds_js_rd_answer_init()
{
  SsdsJsonAnswer* new = (SsdsJsonAnswer*)ssds_malloc(sizeof(SsdsJsonAnswer));
  new->name = NULL;
  new->pkgList = NULL;
  return new;
}

SsdsJsonInstall* ssds_js_rd_install_init()
{
  SsdsJsonInstall* new = (SsdsJsonInstall*)ssds_malloc(sizeof(SsdsJsonInstall));
  new->pkg_name = NULL;
  new->pkg_loc = NULL;
  new->base_url = NULL;
  new->metalink = NULL;
  return new;
}

int ssds_js_rd_get_count(SsdsJsonRead* json, char* name)
{
  const char* default_path="$.data..";
  int length=strlen(default_path)+strlen(name);
  char* full_path=(char*)malloc((length+1)*sizeof(char));
  strncpy(full_path, default_path, strlen(default_path)+1);
  strncat(full_path, name, strlen(name));
  
  JsonPath* new_path = json_path_new();
  json_path_compile(new_path, full_path, NULL);
  
  JsonNode* result = json_path_match(new_path, json->rootNode);
  JsonArray* result_array = json_node_get_array(result);
  JsonArray* final = json_array_get_array_element(result_array, 0);
  int ret = json_array_get_length(final);
  
  free(full_path);
  return ret;
}

void ssds_js_rd_parse_answer(SsdsJsonAnswer* ans_list, SsdsJsonRead* json, int nmr)
{
  //TODO - multiple apps in answer
  //right now only one app can be parsed at a time
  JsonArray*array = json_object_get_array_member(json->dataObj, "install_pkgs");
  
  JsonObject* main_obj = json_array_get_object_element(array, nmr);
  char* name = (char*)json_object_get_string_member(main_obj, "name");
  ans_list->name = (char*)ssds_malloc((strlen(name)+1)*sizeof(char));
  strcpy(ans_list->name, name);
  
  JsonArray* inner_array = json_object_get_array_member(main_obj, "install");
  guint len = json_array_get_length(inner_array);
  //through all the packages for one app
  for(guint i = 0; i < len; i++)
  {
    SsdsJsonInstall* install = ssds_js_rd_install_init();
    JsonObject* obj = json_array_get_object_element(inner_array, i);
    
    //name of one package to install
    char* pkg_name = (char*)json_object_get_string_member(obj, "pkg_name");
    install->pkg_name = (char*)ssds_malloc((strlen(pkg_name)+1)*sizeof(char));
    strcpy(install->pkg_name, pkg_name);
    
    //name of package location on repository
    char* pkg_loc = (char*)json_object_get_string_member(obj, "pkg_loc");
    install->pkg_loc = (char*)ssds_malloc((strlen(pkg_loc)+1)*sizeof(char));
    strcpy(install->pkg_loc, pkg_loc);
    
    //baseurl or null
    if(json_object_get_null_member(obj, "base_url"))
    {
      char* meta = (char*)json_object_get_string_member(obj, "metalink");
      install->metalink = (char*)ssds_malloc((strlen(meta)+1)*sizeof(char));
      strcpy(install->metalink, meta);
    }
    else
    {
      char* base = (char*)json_object_get_string_member(obj, "base_url");
      install->base_url = (char*)ssds_malloc((strlen(base)+1)*sizeof(char));
      strcpy(install->base_url, base);
    }
    
    ans_list->pkgList = g_slist_append(ans_list->pkgList, install);
  }
}
