#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#ifdef __cplusplus
extern "C"{
#endif
  
#include <stdio.h>
#include <stdlib.h>

enum LogClass{logINFO=0, logMESSAGE, logWARNING, logERROR};
extern const char* log_lvl_msg[4];
extern int __verbose;


#define out_message(message) printf("SSDS: MESSAGE: %s\n", message)
#define out_info(message) printf("SSDS: INFO: %s\n", message)
#define out_warning(message) printf("SSDS: WARNING: %s\n", message)
#define out_error(message) printf("SSDS: ERROR: %s\n", message)
#define out_unknown(message) printf("SSDS: UNKNOWN: %s\n", message)

void ssds_log(const char *message, int log_level);
void set_verbose();
void unset_verbose();

#ifdef __cplusplus
}
#endif

#endif