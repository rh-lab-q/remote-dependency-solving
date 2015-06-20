#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H
  
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

enum LogClass{logINFO=0, logMESSAGE, logWARNING, logERROR};
extern const char* log_lvl_msg[4];
extern int __verbose;


#define out_message(message) fprintf(stderr, "SSDS: MESSAGE: %s\n", message)
#define out_info(message) fprintf(stderr, "SSDS: INFO: %s\n", message)
#define out_warning(message) fprintf(stderr, "SSDS: WARNING: %s\n", message)
#define out_error(message) fprintf(stderr, "SSDS: ERROR: %s\n", message)
#define out_unknown(message) fprintf(stderr, "SSDS: UNKNOWN: %s\n", message)

void ssds_log(const char *message, int log_level);
void set_verbose();
void unset_verbose();

#ifdef __cplusplus
}
#endif

#endif