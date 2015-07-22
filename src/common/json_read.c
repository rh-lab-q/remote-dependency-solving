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

SsdsJsonRead* ssds_json_read_init()
{
  SsdsJsonRead* new = (SsdsJsonRead*)ssds_malloc(sizeof(SsdsJsonRead));
  new->parser=json_parser_new();
  return new;
}


gboolean ssds_read_parse(char* buffer, SsdsJsonRead* json)
{
  GError **error;
    
  gboolean ret = json_parser_load_from_data(json->parser, (const gchar*)buffer, -1, error);
  if(!ret)
    return EXIT_FAILURE;
  
  json->rootNode = json_parser_get_root(json->parser);
  
  JsonObject* obj=json_node_get_object(json->rootNode);
  json->dataNode=json_object_get_member(obj, (gchar*)"data");
  json->dataObj=json_node_get_object(json->dataNode);
  return ret;
}


int ssds_read_get_code(SsdsJsonRead* json)
{
  int ret=-1;
  JsonObject* obj=json_node_get_object(json->rootNode);
  if(json_object_has_member(obj, (gchar*)"code"))
  ret=(int)json_object_get_int_member(obj, "code");
  
  return ret;
}

void ssds_read_get_packages(struct SsdsPkgInfo* pkgs, SsdsJsonRead* json)
{
  JsonArray* array = json_object_get_array_member(json->dataObj, "req_pkgs");
  guint len=json_array_get_length(array);
  pkgs->packages=(char**)ssds_malloc(len*sizeof(char*));
  
  guint i;
  for(i=0; i<len; i++)
  {
    pkgs->packages[i]=(char*)ssds_malloc((strlen((char*)json_array_get_string_element(array,i)) + 1)*sizeof(char));
    char* word = (char*)json_array_get_string_element(array, i);
    strcpy(pkgs->packages[i], word);
    pkgs->length++;
  }
}


void ssds_read_repo_info(SsdsJsonRead* json, SsdsRepoInfoList* list)
{
  JsonArray*array = json_object_get_array_member(json->dataObj, "repolist");
  guint len=json_array_get_length(array);
  
  guint i;
  for(i=0; i<len; i++)
  {
    SsdsRepoInfo* repo=ssds_read_repoinfo_init();
    
    JsonObject* obj = json_array_get_object_element(array, i);
    repo->type=(int)json_object_get_int_member(obj, "type");
    
    char* currName=(char*)json_object_get_string_member(obj, "name");
    repo->name=(char*)ssds_malloc((strlen(currName)+1)*sizeof(char));
    strcpy(repo->name, currName);
    
    JsonArray* url_arr = json_object_get_array_member(obj, (gchar*)"repo_url");
    int arr_len = json_array_get_length(url_arr);
    repo->urls=(char**)ssds_malloc(arr_len*(sizeof(char*)));
    
    int j;
    for(j=0; j<arr_len; j++)
    {
      char* word = (char*)json_array_get_string_element(url_arr, j);
      repo->urls[j]=(char*)ssds_malloc((strlen(word)+1)*sizeof(char));
      strcpy(repo->urls[j], word);
      repo->count++;
    }
    
    list->repoInfoList=g_slist_append(list->repoInfoList, repo);
  }
}


SsdsPkgInfo* ssds_read_pkginfo_init()
{
  SsdsPkgInfo* new=(SsdsPkgInfo*)ssds_malloc(sizeof(SsdsPkgInfo));
  new->packages=NULL;
  new->length=0;
  return new;
}


SsdsRepoInfo* ssds_read_repoinfo_init()
{
  SsdsRepoInfo* new = (SsdsRepoInfo*)ssds_malloc(sizeof(SsdsRepoInfo));
  new->count=0;
  new->type=0;
  new->name=NULL;
  new->urls=NULL;
  return new;
}

SsdsRepoInfoList* ssds_read_list_init()
{
  SsdsRepoInfoList* new = (SsdsRepoInfoList*)ssds_malloc(sizeof(SsdsRepoInfoList));
  new->repoInfoList=NULL;
  return new;
}