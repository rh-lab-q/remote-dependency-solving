#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <execinfo.h>
#include "logger.hpp"

//todo
//udelat nekolik trid logovani: INFO, DEBUG, WARNING, ERROR
//pridat info o akci ktera se provadela
//logovat odchyceny vyjimky??
//log file ve /var/log/ ?
//================================================================

//Needs to be compiled with following attributes:
//-g -rdynamic -finstrument-functions
//================================================================


const char* log_lvl_msg[4] = {"INFO", "DEBUG", "WARNING", "ERROR"};
//enum LogOper{};

namespace logger{	
	log::log()
	{
		add_log(logINFO, "Logging inicialized");
	}

			
	//recieves message and prints it along with time and other info
	void log::add_log(int logLvl, std::string message)
	{
		get_time();
		output << log_lvl_msg[logLvl] << " " << message << std::endl;
		std::cout << output.str();
		output.str("");
		output.clear();
	}
	
				
	//creates beginning of log message
	std::ostringstream& log::get_time()
	{
		//time information for logger
		time_t curr_time;
		struct tm* timeinfo;
		char buffer[80];
	
		time(&curr_time);
		timeinfo = localtime(&curr_time);
		strftime(buffer,80,"%a %d %T",timeinfo);
	
		//time is added to output stream
		output << buffer << " ";
	
		return output;
	}

	//function for getting backtrace info for logging purposes
	std::ostringstream& log::trace_class()
	{	
		//backtrace info for logger
		std::stringstream s;
		void* array[10]; // nevim proc 10???
		size_t size;
	
		size = backtrace(array, 10);
		char** arr = backtrace_symbols(array, size);
	
		for(unsigned int i=4; i<size; i++) //nevim proc 4??
		{
			output << arr[i] << std::endl;
		}
		free(arr);
	
		return output;
	}
	
	//log::~log(){
	 // std::cout << output.str();
	//}
}
