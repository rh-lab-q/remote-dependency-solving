#ifndef _LOGGER_HPP
#define _LOGGER_HPP

enum LogClass{logINFO, logDEBUG, logWARNING, logERROR};
extern const char* log_lvl_msg[4];

namespace logger{
	class log{
	public:
		log();
		//~log();
		void add_log(int logLvl, std::string message);
		std::ostringstream& get_time();
		std::ostringstream& trace_class();
		
	protected:
		std::ostringstream output;
	};
}


#endif