#include <stdio.h>
#include <stdlib.h>
#include <glib-2.0/glib.h>
#include "log_handler.h"

// #define g_message(message) g_print("SSDS: MESSAGE: %s\n", message)
// #define g_info(message) g_print("SSDS: INFO: %s\n", message)
// #define g_warning(message) g_print("SSDS: WARNING: %s\n", message)
// #define g_error(message) g_print("SSDS: ERROR: %s\n", message)
// #define g_debug(message) g_print("SSDS: DEBUG: %s\n", message)
// #define g_critical(message) g_print("SSDS: CRITICAL: %s\n", message)



void main()
{
  printf("%s\n", log_lvl_msg[0]);
  rds_log("Pokusna zprava", logINFO);
  rds_log("Pokusny error", logERROR);
  
  set_verbose();
  rds_log("Pokusna zprava", logINFO);
}