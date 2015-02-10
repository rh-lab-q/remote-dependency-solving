#ifndef _PARAMS_HPP
#define _PARAMS_HPP
#include <vector>
#include <string>

namespace ssds_params{
 class params{
 public:
  params();
  bool parse_params(int argc,const char** argv);
  void free_resources();
  void print_usage();
  
  int user_option;
  bool option_set;
  std::vector<std::string> packages;
  enum params_options: short;
 };
 
 class cmd_line{
 public:
   cmd_line();
   
   
 };
}
#endif