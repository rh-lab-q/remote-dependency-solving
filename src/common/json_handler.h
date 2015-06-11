#ifndef _JSON_HANDLER_H
#define _JSON_HANDLER_H
#include <json-glib-1.0/json-glib/json-glib.h>
//#include <json-glib/glib-object.h>
#include <string>
#include <iostream>
#include <exception>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdio.h>

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>

//GLIB
// #include <glib-2.0/glib/gerror.h>
// #include <glib-2.0/glib/glist.h>
// #include <glib-2.0/glib/gslist.h>
// #include <glib-2.0/gio/gio.h>
// #include <glib-2.0/glib.h>
// #include <glib-2.0/gobject/gobject.h>
#include <glib.h>

// #include "repo_handler.hpp"

namespace ssds_json{
  enum url_type :short{
    SSDS_BASEURL = 1,
    SSDS_MIRRORLIST,
    SSDS_METALINK
  }; 
  
  class json_read{
  public:
   struct pkgInfo{
     char** packages;
     int length;
   };
   
   struct repoInfo{
     char** urls; //array of char pointers in case url is baseurl - there can be more than one baseurl for one repo
     char* name;
     int count;//number of addresses in urls
     int type;
   };
   
   json_read();
   bool parse_data(char* buffer);
   int get_code();
   void get_packages(struct pkgInfo* pkgs);
   void get_repo_info();
   struct pkgInfo* pkg_info_init();
   struct repoInfo* repo_info_init();
   
   GSList* repoInfoList;
 private:
   JsonParser* parser;
   JsonNode* rootNode;
   JsonNode* currNode;
   JsonNode* dataNode;
   JsonObject* currObj;
   JsonObject* dataObj;
   
 };
 
  /*
  * Class for creating new xml documents from blank page
  */
 class json_create{
 public:
   json_create();
   void insert_code(int code);
   void add_package(char* package);
   void add_repo(char** url, char* name, int type, int url_count);
   void install_pkgs_init();
   void install_pkgs_insert(HyGoal* goal, const char* name);
   char* json_to_string();
   void json_dump();
   
 private:
   JsonGenerator * generator;
   JsonNode* rootNode;
   JsonNode* codeNode;
   JsonNode* currNode;
   JsonObject* rootObj;
   JsonObject* dataObj;
   JsonObject* currObj;
   JsonArray* currArray;
 };
}

#endif