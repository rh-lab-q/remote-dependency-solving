#ifndef SOLVING_HPP
#define SOLVING_HPP

#include <string>
#include <iostream>

namespace ssds_solving {

	class solve{ 
    	    public:
		
		solve(/* repos class instance with all enabled repos handlers */); 
	        ~solve();
	    	void parseMessage(std::string message);
		std::string answer(std::string message);
	    private:
		/* repos class instance with all enabled repos handlers */
	};

}//ssds_ solving
#endif
