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


SsdsJsonCreate* ssds_js_cr_init()
{
  SsdsJsonCreate* new=(SsdsJsonCreate*)malloc(sizeof(SsdsJsonCreate));
  
  new->generator = json_generator_new();
  new->rootNode = json_node_new(JSON_NODE_OBJECT);
  new->rootObj = json_object_new();
  json_node_take_object(new->rootNode, new->rootObj);
  json_generator_set_root(new->generator, new->rootNode); //set new node as root of the generator
  
  //create code object
  JsonNode * addCode = json_node_new(JSON_NODE_VALUE);
  json_node_set_int(addCode, (gint64)0);
  json_object_set_member(new->rootObj, (gchar*)"code", addCode);
  new->codeNode = addCode;
  
  //create data object
  JsonNode * add = json_node_new(JSON_NODE_OBJECT);
  json_object_set_member(new->rootObj, (gchar*)"data", add);
  new->dataObj = json_object_new();
  json_node_take_object(add, new->dataObj);   
  
  return new;
}


void ssds_js_insert_code(SsdsJsonCreate* json, int code)
{
  json_node_set_int(json->codeNode, (gint64)code);
}

void ssds_js_add_package(SsdsJsonCreate* json,char* package)
{
  //adding package for the first time
  if(!json_object_has_member(json->dataObj,(const gchar*)"req_pkgs"))
  {
    JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(json->dataObj, (gchar*)"req_pkgs", new_node);
    json->currNode = new_node;
    
    JsonArray* new_array = json_array_new();
    json_node_take_array(new_node, new_array);
    json->currArray = new_array;
  }
  //if some packages are already there, only set the right array
  else
  {
    json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)"req_pkgs");
  }

  JsonNode* pkg = json_node_new(JSON_NODE_VALUE);
  json_node_set_string(pkg,(gchar*)package);
  json_array_add_element(json->currArray, pkg);
}


void ssds_js_add_repo(SsdsJsonCreate* json,char** url, char* name, int type, int url_count)
{
  if(!json_object_has_member(json->dataObj,(const gchar*)"repolist"))
  {
    JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(json->dataObj, (gchar*)"repolist", new_node);
    json->currNode = new_node;
    
    JsonArray* new_array = json_array_new();
    json_node_take_array(new_node, new_array);
    json->currArray = new_array;
  }
  else
  {
    json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)"repolist");
  }
  
  JsonNode* new_repo=json_node_new(JSON_NODE_OBJECT);
  JsonObject* new_repo_obj=json_object_new();
  json_node_take_object(new_repo, new_repo_obj);
  json_array_add_object_element(json->currArray, new_repo_obj);
  json->currObj = new_repo_obj;
  
  JsonNode* new_inside=json_node_new(JSON_NODE_ARRAY);
  json_object_set_member(json->currObj, (gchar*)"repo_url", new_inside);
  json->currNode = new_inside;
  
  JsonArray* new_array = json_array_new();
  json_node_take_array(new_inside, new_array);
  json->currArray = new_array;
  
  if(type==1)
  {
    int i;
    for(i=0; i<url_count; i++)
    {
      json_array_add_string_element(json->currArray, (gchar*)url[i]);
    }
  }
  else
    json_array_add_string_element(json->currArray, (gchar*)url[0]);
  
  JsonNode* repo_name=json_node_new(JSON_NODE_VALUE);
  json_node_set_string(repo_name, (gchar*)name);
  json_object_set_member(json->currObj, (gchar*)"name", repo_name);
  
  JsonNode * repo_type = json_node_new(JSON_NODE_VALUE);
  json_node_set_int(repo_type, (gint64)type);
  json_object_set_member(json->currObj, (gchar*)"type", repo_type);
}


void ssds_js_pkgs_init(SsdsJsonCreate* json)
{
  if(!json_object_has_member(json->dataObj,(const gchar*)"install_pkgs"))
  {
    JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(json->dataObj, (gchar*)"install_pkgs", new_node);
    json->currNode = new_node;
    
    JsonArray* new_array = json_array_new();
    json_node_take_array(new_node, new_array);
    json->currArray = new_array;
  }
}


void ssds_js_pkgs_insert(SsdsJsonCreate* json,HyGoal* goal, const char* name)
{
  if(!json_object_has_member(json->dataObj,(const gchar*)"install_pkgs"))
  {
    ssds_js_pkgs_init(json);
  }
  else
  {
    json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)"install_pkgs");
  }
  
  JsonNode* new_node = json_node_new(JSON_NODE_OBJECT);
  JsonObject* new_obj = json_object_new();
  json_node_take_object(new_node, new_obj);
  
  json_array_add_object_element(json->currArray, new_obj);
  json->currObj=new_obj;
  
  HyPackageList goal_pkgs = hy_packagelist_create();
  HyPackage pkg;
  
  goal_pkgs = hy_goal_list_installs(*goal);
  JsonNode* new_inside=json_node_new(JSON_NODE_ARRAY);
  json_object_set_member(json->currObj, (gchar*)"name", new_inside);
  JsonArray* new_arr = json_array_new();
  json_node_take_array(new_inside, new_arr);
  json_array_add_string_element(new_arr, name);
  
  goal_pkgs = hy_goal_list_installs(*goal);
  new_inside=json_node_new(JSON_NODE_ARRAY);
  json_object_set_member(json->currObj, (gchar*)"install", new_inside);
  new_arr = json_array_new();
  json_node_take_array(new_inside, new_arr);
  
  int i;
  for(i=hy_packagelist_count(goal_pkgs)-1; i>=0; i--)
  {
    pkg=hy_packagelist_get(goal_pkgs, i);
    int name_length = strlen(hy_package_get_name(pkg)) + strlen(hy_package_get_arch(pkg));
    char* full_name = (char*) malloc((name_length+3)*sizeof(char));
    
    sprintf(full_name ,"%s.%s", hy_package_get_name(pkg), hy_package_get_arch(pkg));
    //std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
    json_array_add_string_element(new_arr, full_name);
  }
  
  goal_pkgs = hy_goal_list_erasures(*goal);
  new_inside=json_node_new(JSON_NODE_ARRAY);
  json_object_set_member(json->currObj, (gchar*)"erase", new_inside);
  new_arr = json_array_new();
  json_node_take_array(new_inside, new_arr);
  
  for(i=0; i<hy_packagelist_count(goal_pkgs); i++)
  {
    pkg=hy_packagelist_get(goal_pkgs, i);
    int name_length = strlen(hy_package_get_name(pkg)) + strlen(hy_package_get_version(pkg)) + strlen(hy_package_get_release(pkg)) + strlen(hy_package_get_arch(pkg));
    char* full_name = (char*) malloc((name_length+4)*sizeof(char));
    
    sprintf(full_name ,"%s-%s-%s-%s", hy_package_get_name(pkg), hy_package_get_version(pkg), hy_package_get_release(pkg), hy_package_get_arch(pkg));
//     std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
    json_array_add_string_element(new_arr, full_name);
  }
  
  goal_pkgs = hy_goal_list_upgrades(*goal);
  new_inside=json_node_new(JSON_NODE_ARRAY);
  json_object_set_member(json->currObj, (gchar*)"upgrade", new_inside);
  new_arr = json_array_new();
  json_node_take_array(new_inside, new_arr);
  
  for(i=0; i<hy_packagelist_count(goal_pkgs); i++)
  {
    pkg=hy_packagelist_get(goal_pkgs, i);
    int name_length = strlen(hy_package_get_name(pkg)) + strlen(hy_package_get_version(pkg)) + strlen(hy_package_get_release(pkg)) + strlen(hy_package_get_arch(pkg));
    char* full_name = (char*) malloc((name_length+4)*sizeof(char));
    
    sprintf(full_name ,"%s-%s-%s-%s", hy_package_get_name(pkg), hy_package_get_version(pkg), hy_package_get_release(pkg), hy_package_get_arch(pkg));
//     std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
    json_array_add_string_element(new_arr, full_name);
  }
}

char* ssds_js_to_string(SsdsJsonCreate* json)
{
  gsize len;
  char* data;
  data = (char*)json_generator_to_data (json->generator, &len);
  return data;
}


void ssds_js_dump(SsdsJsonCreate* json)//this will always dump error when some array or object is empty - just ignore it
{
  gchar *data;
  json_generator_set_pretty(json->generator, 1);
  data = json_generator_to_data (json->generator, NULL);
  printf("%s\n", (char*)data);
}
