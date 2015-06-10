#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <vector>
#include <stdio.h>

//BOOST - needs to be compiled with -lboost_system option
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/program_options.hpp>

//SSDS
#include "../common/log_handler.h"
#include "../common/network_util.h"
#include "../common/repo_handler.hpp"
#include "../common/solving.hpp"
#include "../common/params.h"

//GLIB-JSON
#include "../common/json_handler.hpp"

/*
std::string usage = "Usage: ./ssds-client <command> [<args> ...]\n\n"
		"List of Commands\n\n"
		"install\t\tResolve dependencies and install packages\n"
		"checkdep\t\tOnly show required packages - do not install yet\n"
		"help\t\tDisplays help\n";

enum param_options{
	CHECK_DEPENDENCIES
};*/

namespace ssds_client{
  class client{
  public:
    client(){};
    
    int command;
    boost::asio::io_service io_service_object;//object that grants access to system I/O operations
  };
}
#endif
