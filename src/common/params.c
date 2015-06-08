#include "params.h"

int parse_params(int argc, char** args)
{
  static int param_opt; //needs to be static or else make fails
  static struct option long_options[]=
  {
    {"install", no_argument, &param_opt, PAR_INSTALL},
    {"chkdep", no_argument, &param_opt, PAR_CHK_DEP},
    {"help", no_argument, 0, 'h'}
  };
  
  int c;
  int opt_index=0;
  int seen=0;
  
  while(1)
  {
    c = getopt_long(argc, args, "vh", long_options, &opt_index);
    
    switch(c)
    {
      case 0:
        seen++;
        break;
      case 'h':
        printf("this is help!\n");
        exit(0);
        break;
      case 'v':
        set_verbose();
        break;
    }
    
    if(c==-1)
      break;
  }

  if(seen>1)
  {
    ssds_log("Choose either install or chkdep. The program will terminate now.", logMESSAGE);
    ssds_log("Wrong parameter combination. Terminating.",logERROR);
    return -1;
  }
  
  return 1;
}
