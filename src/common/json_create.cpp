#include "json_handler.h"
// #include "json_handler_2.h"
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
  
  /* Inserts given code in the json structure
   * @param code    message code for the server
   */
  void json_create::insert_code(int code)
  {
    json_node_set_int(this->codeNode, (gint64)code);
  }
  
  /* Adds a package as a string in the json structure
   * @param package name of the package as a string
   */
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
  
  
  /* Adds repo structure in the json
   * @param url               possible urls of repo, multiple urls are typical for baseurl - type 1
   * @param name              name of parsed repo 
   * @param type              type of repo - baseurl, mirrorlist or metalink
   * @param url_count         number of urls in url
   */  
  void json_create::add_repo(char** url, char* name, int type, int url_count)
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
    
    if(type==1)
    {
      for(int i=0; i<url_count; i++)
        json_array_add_string_element(this->currArray, (gchar*)url[i]);
    }
    else
      json_array_add_string_element(this->currArray, (gchar*)url[0]);
    
    JsonNode* repo_name=json_node_new(JSON_NODE_VALUE);
    json_node_set_string(repo_name, (gchar*)name);
    json_object_set_member(this->currObj, (gchar*)"name", repo_name);
    
    JsonNode * repo_type = json_node_new(JSON_NODE_VALUE);
    json_node_set_int(repo_type, (gint64)type);
    json_object_set_member(this->currObj, (gchar*)"type", repo_type);
  }
  
  /* Dumps json as a string to output for debugging
   * 
   */
  void json_create::json_dump()//this will always dump error when some array or object is empty - just ignore it
  {
    gchar *data;
    gsize len;
    json_generator_set_pretty(this->generator, true);
    data = json_generator_to_data (this->generator, NULL);
    std::cout << (char*)data << std::endl;
  }
  
  /* Converts json structure to string for sending json over the network
   * @return json structure as a string
   */
  char* json_create::json_to_string()
  {
    gsize len;
    char* data;
    data = (char*)json_generator_to_data (this->generator, &len);
//     printf("from json_to_string: %s\n", data);
//     buffer=(char*)malloc(len*sizeof(char));
    return data;
  }
  
  /* Creates a inner structure in json for answer from the server
   * 
   */
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
  
  void json_create::install_pkgs_insert(HyGoal* goal, const char* name)
  {
//     std::cout << "install_pkgs_insert" << std::endl;
    if(!json_object_has_member(this->dataObj,(const gchar*)"install_pkgs"))
    {
      this->install_pkgs_init();
    }
    else
    {
      this->currArray = json_object_get_array_member(this->dataObj, (const gchar*)"install_pkgs");
    }
    
    JsonNode* new_node = json_node_new(JSON_NODE_OBJECT);
    JsonObject* new_obj = json_object_new();
    json_node_take_object(new_node, new_obj);
    
    json_array_add_object_element(this->currArray, new_obj);
    this->currObj=new_obj;
    
    HyPackageList goal_pkgs = hy_packagelist_create();
    HyPackage pkg;
    
    goal_pkgs = hy_goal_list_installs(*goal);
    JsonNode* new_inside=json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(this->currObj, (gchar*)"name", new_inside);
    JsonArray* new_arr = json_array_new();
    json_node_take_array(new_inside, new_arr);
    json_array_add_string_element(new_arr, name);
    
    goal_pkgs = hy_goal_list_installs(*goal);
    new_inside=json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(this->currObj, (gchar*)"install", new_inside);
    new_arr = json_array_new();
    json_node_take_array(new_inside, new_arr);
    
    for(int i=0; i<hy_packagelist_count(goal_pkgs); i++)
    {
      pkg=hy_packagelist_get(goal_pkgs, i);
      std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
      json_array_add_string_element(new_arr, full_pkg.c_str());
    }
    
    goal_pkgs = hy_goal_list_erasures(*goal);
    new_inside=json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(this->currObj, (gchar*)"erase", new_inside);
    new_arr = json_array_new();
    json_node_take_array(new_inside, new_arr);
    
    for(int i=0; i<hy_packagelist_count(goal_pkgs); i++)
    {
      pkg=hy_packagelist_get(goal_pkgs, i);
      std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
      json_array_add_string_element(new_arr, full_pkg.c_str());
    }
    
    goal_pkgs = hy_goal_list_upgrades(*goal);
    new_inside=json_node_new(JSON_NODE_ARRAY);
    json_object_set_member(this->currObj, (gchar*)"upgrade", new_inside);
    new_arr = json_array_new();
    json_node_take_array(new_inside, new_arr);
    
    for(int i=0; i<hy_packagelist_count(goal_pkgs); i++)
    {
      pkg=hy_packagelist_get(goal_pkgs, i);
      std::string full_pkg = hy_package_get_name(pkg)+(std::string)"-"+hy_package_get_version(pkg)+(std::string)"-" + hy_package_get_release(pkg)+ (std::string)"-" + hy_package_get_arch(pkg);
      json_array_add_string_element(new_arr, full_pkg.c_str());
    }
    
  }
}