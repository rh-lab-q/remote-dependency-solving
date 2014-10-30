#ifndef _PARAMS_HPP
#define _PARAMS_HPP
#include <vector>
#include <string>

namespace ssds_params{
 class params{
 public:
  params();
  void parse_params(int argc,const char** argv);
  void free_resources();
  
  std::vector<std::string> packages;

 };
 
 class cmd_line{
 public:
   cmd_line();
   
   
 };
}
#endif