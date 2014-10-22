#include "params.hpp"
#include <iostream>
#include <string.h>
#include <vector>

namespace ssds_params 
{
  params::params()
  {

  }

  void params::parse_params(int argc, const char** argv)
  {
    for(int i = 1; i<argc; i++){
      if(strcmp(argv[i], "checkdep") == 0)
	std::cout << "check" << std::endl;
      else if(strcmp(argv[i], "install") == 0)
	std::cout << "install" << std::endl;
      else
	packages.push_back(argv[i]);
    }
  }
}