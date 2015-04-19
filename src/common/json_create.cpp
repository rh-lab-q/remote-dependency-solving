#include "json_handler.hpp"

namespace ssds_json
{
  json_create::json_create()
  {
    this->generator = json_generator_new();
    this->rootNode = json_node_new(JSON_NODE_OBJECT);
    this->rootObj = json_object_new();
    json_node_take_object(this->rootNode, this->rootObj);
    json_generator_set_root(this->generator, this->rootNode);
    
    //create code object
    JsonNode * addCode = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(addCode, (gint64)0);
    json_object_set_member(this->rootObj, (gchar*)"code", addCode);
    this->codeNode = addCode;
    
    //create data object
    JsonNode * add = json_node_new(JSON_NODE_OBJECT);
    json_object_set_member(this->rootObj, (gchar*)"data", add);
    this->dataObj = json_object_new();
    json_node_take_object(add, this->dataObj);
  }
  
  void json_create::insert_code(int code)
  {
    json_node_set_int(this->codeNode, (gint64)code);
  }
  
  void json_create::add_package(char* package)
  {
    if(!json_object_has_member(this->dataObj,(const gchar*)"req_pkgs"))
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      json_object_set_member(this->dataObj, (gchar*)"req_pkgs", new_node);
      this->currNode = new_node;
      
      JsonArray* new_array = json_array_new();
      json_node_take_array(new_node, new_array);
      this->currArray = new_array;
    }
    else
    {
      this->currArray = json_object_get_array_member(this->dataObj, (const gchar*)"req_pkgs");
    }
    
    JsonNode* pkg = json_node_new(JSON_NODE_VALUE);
    json_node_set_string(pkg,(gchar*)package);
    json_array_add_element(this->currArray, pkg);
  }
  
  void json_create::add_repo(char* url, char* name, int type)
  {
    if(!json_object_has_member(this->dataObj,(const gchar*)"repolist"))
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      json_object_set_member(this->dataObj, (gchar*)"repolist", new_node);
      this->currNode = new_node;
      
      JsonArray* new_array = json_array_new();
      json_node_take_array(new_node, new_array);
      this->currArray = new_array;
    }
    else
    {
      this->currArray = json_object_get_array_member(this->dataObj, (const gchar*)"repolist");
    }
    
    JsonNode* new_repo=json_node_new(JSON_NODE_OBJECT);
    JsonObject* new_repo_obj=json_object_new();
    json_node_take_object(new_repo, new_repo_obj);
    json_array_add_object_element(this->currArray, new_repo_obj);
    this->currObj = new_repo_obj;
    
    JsonNode* new_inside=json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(this->currObj, (gchar*)"repo_url", new_inside);
    this->currNode = new_inside;
    
    JsonArray* new_array = json_array_new();
    json_node_take_array(new_inside, new_array);
    this->currArray = new_array;
    
    json_array_add_string_element(this->currArray, (gchar*)url);
    
    JsonNode* repo_name=json_node_new(JSON_NODE_VALUE);
    json_node_set_string(repo_name, (gchar*)name);
    json_object_set_member(this->currObj, (gchar*)"name", repo_name);
    
    JsonNode * repo_type = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(repo_type, (gint64)type);
    json_object_set_member(this->currObj, (gchar*)"type", repo_type);
  }
  
  void json_create::json_dump()//this will always dump error when some array or object is empty - just ignore it
  {
    gchar *data;
    gsize len;
    json_generator_set_pretty(this->generator, true);
    data = json_generator_to_data (this->generator, NULL);
    std::cout << (char*)data << std::endl;
  }
  
  char* json_create::json_to_string()
  {
    gsize len;
    char* data;
    data = (char*)json_generator_to_data (this->generator, &len);
//     printf("from json_to_string: %s\n", data);
//     buffer=(char*)malloc(len*sizeof(char));
    return data;
  }
  
  void json_create::install_pkgs_init()
  {
    if(!json_object_has_member(this->dataObj,(const gchar*)"install_pkgs"))
    {
      JsonNode* new_node = json_node_new(JSON_NODE_ARRAY);
      json_object_set_member(this->dataObj, (gchar*)"install_pkgs", new_node);
      this->currNode = new_node;
      
      JsonArray* new_array = json_array_new();
      json_node_take_array(new_node, new_array);
      this->currArray = new_array;
    }
  }
}