#include "json_improved.h"
/*****************************************
 * This part is for json creation        *
 * ***************************************/
SsdsJsonCreate* ssds_js_cr_init(int code)
{
  SsdsJsonCreate* new = (SsdsJsonCreate*)malloc(sizeof(SsdsJsonCreate));
  
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

void ssds_js_cr_add_array_member(SsdsJsonCreate* json, int type, void* data)
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
      json_array_add_int_element(json->currArray, *((guint64*)data));
      break;
    }
  }
}


void ssds_js_cr_add_obj_member(SsdsJsonCreate* json, int type, void* data, const char* name)
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
      json_object_set_int_member(json->currObj, (const gchar*)name, *((guint64*)data));
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

void ssds_js_cr_new_data(SsdsJsonCreate* json, int type, const char* name, void* data)
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
      json_object_set_int_member(json->currObj, (const gchar*)name, *((guint64*)data));
      break;
    }
  }
}

void ssds_js_switch_array(SsdsJsonCreate* json, const char* name)
{
  json->currArray = json_object_get_array_member(json->dataObj, (const gchar*)name);
}

void ssds_js_cr_upper_array(SsdsJsonCreate* json)
{
  do
  {
    json->currNode = json_node_get_parent(json->currNode);
  }while(!JSON_NODE_HOLDS_ARRAY(json->currNode));
  
  json->currArray = json_node_get_array(json->currNode);
}

void ssds_js_cr_upper_obj(SsdsJsonCreate* json)
{
  do
  {
    json->currNode = json_node_get_parent(json->currNode);
  }while(!JSON_NODE_HOLDS_OBJECT(json->currNode));
  
  json->currObj = json_node_get_object(json->currNode);
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

/*****************************************
 * This part is for json parsing         *
 * ***************************************/
SsdsJsonRead* ssds_json_read_init()
{
  SsdsJsonRead* new = (SsdsJsonRead*)malloc(sizeof(SsdsJsonRead));
  new->parser=json_parser_new();
  return new;
}


gboolean ssds_read_parse(char* buffer, SsdsJsonRead* json)
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

int ssds_read_get_code(SsdsJsonRead* json)
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

int main()
{
  SsdsJsonCreate* json_cr = ssds_js_cr_init(123);
  ssds_js_cr_new_data(json_cr, JS_ARRAY, "install_pkgs", NULL);
  ssds_js_cr_add_array_member(json_cr, JS_OBJ, NULL);
  ssds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "install");
  ssds_js_cr_add_array_member(json_cr, JS_OBJ, NULL);
  
  ssds_js_cr_add_obj_member(json_cr, JS_STRING, "pokusny text", "pkg_name");
  
  gboolean attempt = TRUE;
  ssds_js_cr_add_obj_member(json_cr, JS_BOOL, &attempt, "allowed");
  int attempt2 = 64;
  ssds_js_cr_add_obj_member(json_cr, JS_INT, &attempt2, "some_value");
  
  ssds_js_cr_upper_obj(json_cr);
  ssds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "erase");
  
  ssds_js_cr_add_obj_member(json_cr, JS_ARRAY, NULL, "upgrade");
  
  ssds_js_dump(json_cr);
  
  
  //x path example
  char* result = ssds_js_to_string(json_cr);
  SsdsJsonRead* json_rd = ssds_json_read_init();
  ssds_read_parse(result, json_rd);
  
  GList* list = ssds_js_rd_find(json_rd, "$.data.install_pkgs..pkg_name");
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
