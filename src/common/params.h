#ifndef _PARAMS_H
#define _PARAMS_H

#include <glib.h>
// #include <glib-2.0/glib/galloca.h>
#include <stdio.h>
#include "log_handler.h"
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>



#ifdef __cplusplus
extern "C"{
#endif

enum ParamsOptions{ 
  PAR_NO_OPT = 0,   // 0 No option was provided
  PAR_INSTALL,      // 1 Install option chosen
  PAR_CHK_DEP       // 2 Only check if dependencies are fulfilled
}ParOpt;
  
typedef struct ParamOpts ParamOpt;

extern int param_opt;

int parse_params(int argc, const char** args);
ParamOpt* init_params();
void print_help();

#ifdef __cplusplus
}
#endif

#endif