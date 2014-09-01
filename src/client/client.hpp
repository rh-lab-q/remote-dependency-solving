#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace ssds_client{
class client{
	public:
		client()
		{
		}

		//object that grants access to system I/O operations
		boost::asio::io_service io_service_object;
		
};


}
#endif
