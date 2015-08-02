//GLIB
#include <glib.h>
#include <glib-2.0/glib.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-types.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct SsdsJsonCreate SsdsJsonCreate;

struct SsdsJsonCreate{
   JsonGenerator * generator; /**< Holds structure for generatig json from scraps */
   JsonNode* rootNode;        /**< Points to root of json */
   JsonNode* codeNode;        /**< Points to node holding control code */
   JsonNode* dataNode;        /**< Points to node holding data part of json */
   JsonNode* currNode;        /**< Currently used node */
   JsonObject* rootObj;       /**< Root object - used for adding new objects */
   JsonObject* dataObj;       /**< Data object - used for adding objects under data */
   JsonObject* currObj;       /**< Currently used object */
   JsonArray* currArray;      /**< Currently used array */
};

typedef enum {
  JS_STRING = 0,
  JS_INT,
  JS_BOOL,
  JS_ARRAY,
  JS_OBJ
}SsdsJsInsType;

// SsdsJsonCreate* ssds_js_cr_init();
// gboolean ssds_js_cr_find(SsdsJsonCreate* json, char* x_path);
// // void ssds_js_cr_insert(SsdsJsonCreate* json, int type, void* data, const char* name);
// void ssds_js_dump(SsdsJsonCreate* json);



typedef struct SsdsJsonRead SsdsJsonRead;

struct SsdsJsonRead{
  JsonParser* parser;         /** < parser holds SsdsJsonRead structure */
  JsonNode* rootNode;         /** < root of json - used to find objects */
  JsonNode* currNode;         /** < currently used node */
  JsonNode* dataNode;         /** < data node - often used */
  JsonObject* currObj;        /** < currently used object */
  JsonObject* dataObj;        /** < data object - often used for adding new objects */
};

int ssds_read_get_code(SsdsJsonRead* json);