#include "params.hpp"
#include <iostream>
#include <string.h>
#include <vector>

namespace ssds_params 
{
  enum params_options: short{
    PAR_NO_OPT = 0, 	// 0 No option was provided
    PAR_INSTALL,	// 1 Install option chosen
    PAR_CHK_DEP		// 2 Only check if dependencies are fulfilled
  }ParOpt;
  
  params::params()
  {
    //this->option_set = false;
    this->user_option = PAR_NO_OPT;
  }

  /*
   * Parsing parameters - now only main commands, no other options
   */
  bool params::parse_params(int argc, const char** argv)
  {
    if(argc == 1){
      std::cout << "No COMMAND provided. Program exits." << std::endl;
      print_usage();
      return false;
    }
    
    if(strcmp(argv[1], "checkdep") == 0)
      this->user_option = PAR_CHK_DEP;
    else if(strcmp(argv[1], "install") == 0)
      this->user_option = PAR_INSTALL;    
    else {
      std::cout << "No COMMAND provided. Program exits." << std::endl;
      print_usage();
      return false;
    }
      
    for(int i = 1; i<argc; i++){
	packages.push_back(argv[i]);
    }
    
    return true;
  }
  
  void params::print_usage()
  {
    std::cout << "Usage: ./ssds-client COMMAND package [package ...]" << std::endl;
  }

  
  void params::free_resources()
  {
    packages.clear();
  }

}