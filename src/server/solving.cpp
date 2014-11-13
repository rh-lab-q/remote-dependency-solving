//============================================================================
// Name		: Solving.cpp
// Author	: Jozkar
// Version	: 0.1
// Copyright	: GNU GPL
// Description	: Solving class at server side of SSDS
//============================================================================

/*BUILDED WITH THESE FLAGS: -O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0*/

#include "solving.hpp"
namespace ssds_solving {
  
  solve::solve(/* repos class instance */){
  }

  solve::~solve() {

  }
  
  // parsing message
  void solve::parseMessage(std::string message){
 	std::cout << message << std::endl;
  }

  // result of SSDS
  std::string solve::answer(std::string message){
	parseMessage(message);
	return "Connection accepted. In future, server will tell you what to do.\n";
  }
}
