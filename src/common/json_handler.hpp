#ifndef _JSON_HANDLER_HPP
#define _JSON_HANDLER_HPP
#include <json-glib-1.0/json-glib/json-glib.h>
//#include <json-glib/glib-object.h>
#include <string>
#include <iostream>
#include <exception>
#include <fstream>
#include <vector>

//GLIB
#include <glib-2.0/glib/gerror.h>
#include <glib-2.0/gio/gio.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/gobject/gobject.h>

namespace ssds_json{
 class json_debug{
 public:
   json_debug();
 };
 
 class json_read{
 public:
   json_read();

 };
 
  /*
  * Class for creating new xml documents from blank page
  */
 class json_create{
 public:
   json_create();
   void insert_code(int code);
   void add_package(char* package);
   void add_repo(char* url, char* name, int type);
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