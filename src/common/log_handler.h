#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H
  
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

enum LogClass{logINFO=0, logMESSAGE, logWARNING, logERROR, logDEBUG};
extern const char* log_lvl_msg[4];
extern int __verbose;
extern int __debug;

#define LOG_FILE "/tmp/ssds.log"

void ssds_log(int log_level, const char *message, ...);

void set_verbose();
void unset_verbose();

void set_debug();
void unset_debug();

#ifdef __cplusplus
}
#endif

#endif