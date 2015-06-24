#include "params.h"

int parse_params_cl(int argc, char* argv[], ParamOptsCl* params)
{
  if(argc==1)
  {
    ssds_log(logERROR, "No command provided. The program will terminate now\n");
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
    c = getopt_long(argc, argv, "vhd", long_options, &opt_index);
    
    switch(c)
    {
      case 0:
        seen++;
        break;
        
      case 'h':
        print_help();
        exit(EXIT_SUCCESS);
        break;
        
      case 'v':
        set_verbose();
        break;
        
      case 'd':
        set_debug();
        break;
        
      case '?':
        print_help();
        exit(EXIT_FAILURE);
        break;
    }
    
    if(c==-1)
      break;
  }

  if(seen>1)
  {
    ssds_log(logMESSAGE, "Choose either install or chkdep. The program will terminate now\n");
    ssds_log(logERROR, "Wrong parameter combination. Terminating\n");
    return -1;
  }
  
  if(seen==0)
  {
    ssds_log(logERROR, "No command provided. The program will terminate now\n");
    exit(1);
  }
  
  if(optind < argc)
  {
    while(optind < argc)
    {
      params->pkgs = g_slist_append(params->pkgs, argv[optind++]);
      params->pkg_count++;
    }
  }
  
  return 1;
}

ParamOptsCl* init_params_cl()
{
  ParamOptsCl* new = (ParamOptsCl*)malloc(sizeof(ParamOptsCl));
  new->pkg_count=0;
  new->command=-1;
  new->pkgs=NULL;
  return new;
}


void parse_params_srv(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "vd")) != -1) {
    switch (opt) {
      case 'v':
        set_verbose();
        break;
      case 'd':
        set_debug();
        break;
      default: /* '?' */
        print_help();
        exit(EXIT_FAILURE);
    }
  }
}


void print_help()
{
  printf("this is help!\n");
}

