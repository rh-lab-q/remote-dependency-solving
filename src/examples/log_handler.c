#include "log_handler.h"

const char* log_lvl_msg[4] = {"INFO", "MESSAGE", "WARNING", "ERROR"};
int __verbose = 0;

void ssds_log(char *message, int log_level)
{
  FILE *flog = fopen("/tmp/ssds.log", "a");
  
  if(flog==NULL)
  {
    out_error("Unable to open log file. Need to run as root.");
    exit(1);
  }
  
  fprintf(flog, "%s: %s\n", log_lvl_msg[log_level], message);
  fclose(flog);
  
  switch(log_level){
    case logINFO:
      if(__verbose==1)
        out_info(message);
      break;
    case logMESSAGE:
      if(__verbose==1)
        out_message(message);
      break;
    case logWARNING:
      out_warning(message);
      break;
    case logERROR:
      out_error(message);
      break;
    default:
      out_unknown(message);
      break;
  }
}

void set_verbose()
{
  __verbose=1;
}

void unset_verbose()
{
  __verbose=0;
}