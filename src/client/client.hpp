#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/program_options.hpp>
#include "../common/logger.hpp"
#include "../common/xml_handler.hpp"
#include "../common/repo_handler.hpp"

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
		client();
		
	public:
		int command;
		boost::asio::io_service io_service_object;//object that grants access to system I/O operations
};


}
#endif
