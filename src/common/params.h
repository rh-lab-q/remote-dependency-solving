#ifndef PARAMS_H
#define PARAMS_H

#include <stdio.h>
#include "log_handler.h"
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

enum params_options{ 
  PAR_NO_OPT = 0,     // 0 No option was provided
  PAR_INSTALL,    // 1 Install option chosen
  PAR_CHK_DEP               // 2 Only check if dependencies are fulfilled
}ParOpt;
  
extern int param_opt;

int parse_params(int argc, char** args);

#ifdef __cplusplus
}
#endif

#endif