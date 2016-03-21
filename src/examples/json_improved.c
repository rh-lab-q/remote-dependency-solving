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

#include "json_improved.h"

RdsJsonCreate* rds_js_cr_init(int code)
{
  RdsJsonCreate* new = (RdsJsonCreate*)malloc(sizeof(RdsJsonCreate));
  
  new->generator = json_generator_new();
  new->rootNode = json_node_new(JSON_NODE_OBJECT);
  new->rootObj = json_object_new();
  json_node_take_object(new->rootNode, new->rootObj);
  json_generator_set_root(new->generator, new->rootNode); //set new node as root of the generator
  
  //create code object
  JsonNode * addCode = json_node_new(JSON_NODE_VALUE);
  json_node_set_int(addCode, (gint64)code);
  json_object_set_member(new->rootObj, (gchar*)"code", addCode);
  new->codeNode = addCode;
  
  //create data object
  JsonNode * add = json_node_new(JSON_NODE_OBJECT);
  new->dataNode = add;
  json_object_set_member(new->rootObj, (gchar*)"data", add);
  new->dataObj = json_object_new();
  json_node_take_object(add, new->dataObj);  
  
  new->currNode = new->dataNode;
  
  return new;
}

void rds_js_cr_add_array_member(RdsJsonCreate* json, int type, void* data)
{
  switch(type)
  {
    case JS_OBJ:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_OBJECT);
      JsonObject* new_obj = json_object_new();
      json_node_take_object(new_node, new_obj);
      
      json_array_add_object_element(json->currArray, new_obj);
      json->currObj = new_obj;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      break;
    }
    case JS_ARRAY:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      JsonArray* new_arr = json_array_new();
      json_node_take_array(new_node, new_arr);
      
      json_array_add_array_element(json->currArray, new_arr);
      json->currArray = new_arr;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      break;
    }
    case JS_STRING:
    {
      json_array_add_string_element(json->currArray, (const gchar*)data);
      break;
    }
    case JS_BOOL:
    {
      json_array_add_boolean_element(json->currArray, *((gboolean*)data));
      break;
    }
    case JS_INT:
    {
      json_array_add_int_element(json->currArray, *((guint*)data));
      break;
    }
  }
}


void rds_js_cr_add_obj_member(RdsJsonCreate* json, int type, void* data, const char* name)
{
  switch(type)
  {
    case JS_STRING:
    {
      json_object_set_string_member(json->currObj, (const gchar*)name, (const char*)data);
      break;
    }
      
    case JS_BOOL:
    { 
      if((*(gboolean*)data)==TRUE)
        json_object_set_boolean_member(json->currObj, (const gchar*)name, TRUE);
      else
        json_object_set_boolean_member(json->currObj, (const gchar*)name, FALSE);
     
      break;
    }
      
    case JS_INT:
      json_object_set_int_member(json->currObj, (const gchar*)name, *((guint*)data));
    break;
      
    case JS_OBJ:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_OBJECT);
      JsonObject* new_obj = json_object_new();
      json_node_take_object(new_node, new_obj);
      
      json_object_set_object_member(json->currObj, (gchar*)name, new_obj);
      
      json->currObj = new_obj;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      break;
    } 
    case JS_ARRAY:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      JsonArray* new_arr = json_array_new();
      json_node_take_array(new_node, new_arr);
      
      json_object_set_array_member(json->currObj, (gchar*)name, new_arr);
      
      json->currArray = new_arr;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      
      break;
    }
  }
}

void rds_js_cr_new_data(RdsJsonCreate* json, int type, const char* name, void* data)
{
  switch(type)
  {
    case JS_ARRAY:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      JsonArray* new_arr = json_array_new();
      json_node_take_array(new_node, new_arr);
      
      json_object_set_array_member(json->dataObj, (const gchar*)name, new_arr);
      json->currArray = new_arr;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      break;
    }
    case JS_OBJ:
    {
      JsonNode* new_node = json_node_new(JSON_NODE_OBJECT);
      JsonObject* new_obj = json_object_new();
      json_node_take_object(new_node, new_obj);
      
      json_object_set_object_member(json->dataObj, (const gchar*)name, new_obj);
      json->currObj = new_obj;
      json_node_set_parent(new_node, json->currNode);
      json->currNode = new_node;
      break;
    } 
    case JS_STRING:
    {
      json_object_set_string_member(json->dataObj, (const gchar*) name, (const gchar*)data);
      break;
    } 
    case JS_BOOL:
    {
      if((*(gboolean*)data)==TRUE)
        json_object_set_boolean_member(json->dataObj, (const gchar*)name, TRUE);
      else
        json_object_set_boolean_member(json->dataObj, (const gchar*)name, FALSE);
      break;
    }
    case JS_INT:
    {
      json_object_set_int_member(json->currObj, (const gchar*)name, *((guint*)data));
      break;
    }
  }
}

gboolean rds_js_cr_switch_array(RdsJsonCreate* json, const char* name)
{
  //create full path in jsonpath format from name of array provided
  const char* default_path="$.data..";
  int length=strlen(default_path)+strlen(name);
  char* full_path=(char*)malloc((length+1)*sizeof(char));
  strncpy(full_path, default_path, strlen(default_path)+1);
  strncat(full_path, name, strlen(name));

  //json path for finding 
  JsonPath* new_path = json_path_new();
  if(!json_path_compile(new_path, full_path, NULL))
    return FALSE;
  
  //get the result
  JsonNode* root = json_generator_get_root(json->generator);
  JsonNode* result = json_path_match(new_path, root);
  JsonArray* result_arr = json_node_get_array(result);
  
  JsonNode* selected_node = json_array_get_element(result_arr, 0);
  json->currArray = json_node_get_array(selected_node);
  
  //clean up
  free(full_path);
  g_object_unref(new_path);
  json_node_free(result);
  return TRUE;
}

void rds_js_cr_upper_array(RdsJsonCreate* json)
{
  do
  {
    json->currNode = json_node_get_parent(json->currNode);
  }while(!JSON_NODE_HOLDS_ARRAY(json->currNode));
  
  json->currArray = json_node_get_array(json->currNode);
}

void rds_js_cr_upper_obj(RdsJsonCreate* json)
{
  do
  {
    json->currNode = json_node_get_parent(json->currNode);
  }while(!JSON_NODE_HOLDS_OBJECT(json->currNode));
  
  json->currObj = json_node_get_object(json->currNode);
}

char* rds_js_cr_to_string(RdsJsonCreate* json)
{
  gsize len;
  char* data;
  data = (char*)json_generator_to_data (json->generator, &len);
  return data;
}

void rds_js_cr_dump(RdsJsonCreate* json)//this will always dump error when some array or object is empty - just ignore it
{
  gchar *data;
  json_generator_set_pretty(json->generator, 1);
  data = json_generator_to_data (json->generator, NULL);
  printf("%s\n", (char*)data);
}

/*****************************************
 * This part is for json parsing         *
 * ***************************************/
RdsJsonRead* rds_json_rd_init()
{
  RdsJsonRead* new = (RdsJsonRead*)malloc(sizeof(RdsJsonRead));
  new->parser=json_parser_new();
  return new;
}


gboolean rds_rd_parse(char* buffer, RdsJsonRead* json)
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

int rds_rd_get_code(RdsJsonRead* json)
{
  int ret=-1;
  JsonObject* obj=json_node_get_object(json->rootNode);
  if(json_object_has_member(obj, (gchar*)"code"))
  ret=(int)json_object_get_int_member(obj, "code");
  
  return ret;
}

GList* rds_js_rd_find(RdsJsonRead* json, char* x_path)
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

int main()
{
  RdsJsonCreate* json_cr = rds_js_cr_init(123);
  rds_js_cr_new_data(json_cr, JS_ARRAY, "install_pkgs", NULL);
  rds_js_cr_add_array_member(json_cr, JS_OBJ, NULL);
  rds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "install");
  rds_js_cr_add_array_member(json_cr, JS_OBJ, NULL);
  
  rds_js_cr_add_obj_member(json_cr, JS_STRING, "pokusny text", "pkg_name");
  
  gboolean attempt = TRUE;
  rds_js_cr_add_obj_member(json_cr, JS_BOOL, &attempt, "allowed");
  int attempt2 = 64;
  rds_js_cr_add_obj_member(json_cr, JS_INT, &attempt2, "some_value");
  
  rds_js_cr_upper_obj(json_cr);
  rds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "erase");
  
  rds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "upgrade");
  rds_js_cr_add_array_member(json_cr, JS_STRING, "hokus_pokus");
  rds_js_cr_switch_array(json_cr, "erase");
  rds_js_cr_add_array_member(json_cr, JS_STRING, "hokus_pokus");
  
  rds_js_cr_dump(json_cr);
  
  
  //x path example
  char* result = rds_js_cr_to_string(json_cr);
  RdsJsonRead* json_rd = rds_json_rd_init();
  rds_rd_parse(result, json_rd);
  
  GList* list = rds_js_rd_find(json_rd, "$.data.install_pkgs..pkg_name");
  if(list==NULL)
    printf("list is empty\n");
  else{
    int len = g_list_length(list);
    for(int i=0; i<len; i++)
    {
      JsonNode* node = g_list_nth_data(list, i);
      printf("%s\n", (char*)json_node_get_string(node));
      
    }
  }
  
  
  
  return 0;
}
