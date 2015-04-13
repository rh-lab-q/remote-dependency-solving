#include "json_handler.hpp"

namespace ssds_json 
{
  json_read::json_read()
  {
    this->parser=json_parser_new();
    this->repoInfoList=nullptr;
  }
  
  bool json_read::parse_data(char* buffer)
  {
    GError **error;
    bool ret = json_parser_load_from_data(this->parser, (const gchar*)buffer, -1, error);
    this->rootNode = json_parser_get_root(parser);
    
    JsonObject* obj=json_node_get_object(this->rootNode);
    this->dataNode=json_object_get_member(obj, (gchar*)"data");
    this->dataObj=json_node_get_object(this->dataNode);
    return ret;
  }
  
  int json_read::get_code()
  {
    int ret=-1;
    JsonObject* obj=json_node_get_object(this->rootNode);
    if(json_object_has_member(obj, (gchar*)"code"))
      ret=(int)json_object_get_int_member(obj, "code");
    
    return ret;
  }
  
  struct json_read::pkgInfo* json_read::pkg_info_init()
  {
    struct pkgInfo* ret=(struct pkgInfo*)malloc(sizeof(struct pkgInfo));
    ret->length=0;
    return ret;
  }
  
  struct json_read::repoInfo* json_read::repo_info_init()
  {
    struct repoInfo* ret = (struct repoInfo*)malloc(sizeof(struct repoInfo));
    ret->count=0;
    ret->type=0;
    ret->name=nullptr;
    ret->urls=nullptr;
    return ret;
  }
  
  void json_read::get_packages(pkgInfo* pkgs)
  {
    JsonArray*array = json_object_get_array_member(this->dataObj, "req_pkgs");
    guint len=json_array_get_length(array);
    pkgs->packages=(char**)malloc(len*sizeof(char*));
    
    for(guint i=0; i<len; i++)
    {
      pkgs->packages[i]=(char*)malloc((strlen((char*)json_array_get_string_element(array,i)) + 1)*sizeof(char));
      char* word = (char*)json_array_get_string_element(array, i);
      strcpy(pkgs->packages[i], word);
      pkgs->length++;
    }
  }
  
  void json_read::get_repo_info()
  {
    JsonArray*array = json_object_get_array_member(this->dataObj, "repolist");
    guint len=json_array_get_length(array);
    
    for(guint i=0; i<len; i++)
    {
      struct repoInfo* repo=repo_info_init();
      
      JsonObject* obj = json_array_get_object_element(array, i);
      repo->type=(int)json_object_get_int_member(obj, "type");
      
      char* currName=(char*)json_object_get_string_member(obj, "name");
      repo->name=(char*)malloc((strlen(currName)+1)*sizeof(char));
      strcpy(repo->name, currName);
      
      JsonArray* url_arr = json_object_get_array_member(obj, (gchar*)"repo_url");
      int arr_len = json_array_get_length(url_arr);
      repo->urls=(char**)malloc(arr_len*(sizeof(char*)));
      
      for(int i=0; i<arr_len; i++)
      {
        char* word = (char*)json_array_get_string_element(url_arr, i);
        repo->urls[i]=(char*)malloc((strlen(word)+1)*sizeof(char));
        strcpy(repo->urls[i], word);
        repo->count++;
      }
      
      this->repoInfoList=g_slist_append(this->repoInfoList, repo);
    }
  }
}
