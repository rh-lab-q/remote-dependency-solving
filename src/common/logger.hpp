#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <sstream>
#include <ctime>

//udelat nekolik trid logovani: INFO, WARNING, ERROR
//pridat info o akci ktera se provadela
//log file ve /var/log/ ?

enum LogClass{logINFO, logWARNING, logERROR};
//enum LogOper{};

const char* log_lvl_msg[3] = {"INFO", "WARNING", "ERROR"};

namespace logger
{
	//logging class for client side
	class log
	{
		public:
			log(){}

			/*
			//recieves message and prints it along with time and other info
			void add_log(std::string message)
			{
				get_info();
				output << message << std::endl;
				std::cout << output.str();
			}*/

			//creates beginning of log message
			std::ostringstream& get_info(int logLvl)
			{
				time_t curr_time;
				struct tm* timeinfo;
				char buffer[80];

				time(&curr_time);
				timeinfo = localtime(&curr_time);
				strftime(buffer,80,"%a %d %T",timeinfo);

				output << buffer << " ";
				output << log_lvl_msg[logLvl] << " ";

				return output;
			}

			//print output when ending operations
			~log(){std::cout<<output.str();}	

		protected:
			std::ostringstream output;
	};
	

	/*
	//logging class for server side
	class server_log
	{
		public:
			server_log()
			{
				std::cout << "Starting server logger" << std::endl;
			}
	};*/

	//gathers info from logging class and print along with time etc.
}
#endif //LOGGER_H
