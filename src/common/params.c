#include "params.h"



int parse_params(int argc, const char** args, ParamOpt* params)
{
  if(argc==1)
  {
    ssds_log("No command provided. The program will terminate now.", logERROR);
    exit(1);
  }
  
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
  extern int optind;
  
  while(1)
  {
    c = getopt_long(argc, args, "vh", long_options, &opt_index);
    
    switch(c)
    {
      case 0:
        seen++;
        break;
      case 'h':
        print_help();
        exit(0);
        break;
      case 'v':
        set_verbose();
        break;
      case '?':
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
  
  if(seen==0)
  {
    ssds_log("No command provided. The program will terminate now.", logERROR);
    exit(1);
  }
  
  if(optind < argc)
  {
    while(optind < argc)
    {
      params->pkgs = g_slist_append(params->pkgs, args[optind++]);
      params->pkg_count++;
    }
  }
  
  return 1;
}

ParamOpt* init_params()
{
  ParamOpt* new = (ParamOpt*)malloc(sizeof(ParamOpt));
  new->pkg_count=0;
  new->command=-1;
  new->pkgs=NULL;
  return new;
}


void print_help()
{
  printf("this is help!\n");
}

