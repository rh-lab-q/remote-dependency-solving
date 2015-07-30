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
  json_object_set_member(new->rootObj, (gchar*)"data", add);
  new->dataObj = json_object_new();
  json_node_take_object(add, new->dataObj);   
  
  return new;
}

gboolean ssds_js_cr_find(SsdsJsonCreate* json, char* x_path)
{
  gboolean ret = false;
  JsonPath* new_path = json_path_new();
  json_path_compile(new_path, x_path, NULL);
  
  
  return ret;
}

void ssds_js_cr_insert(SsdsJsonCreate* json, int type, void* data)
{
  
  
  
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
int ssds_read_get_code(SsdsJsonRead* json)
{
  int ret=-1;
  JsonObject* obj=json_node_get_object(json->rootNode);
  if(json_object_has_member(obj, (gchar*)"code"))
  ret=(int)json_object_get_int_member(obj, "code");
  
  return ret;
}

int main()
{
  SsdsJsonCreate* json_cr = ssds_js_cr_init(123);
  
  
  
  ssds_js_dump(json_cr);
  return 0;
}
