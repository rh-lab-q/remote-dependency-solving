#CMAKE BUILD 1.3
#Usage in CLI: "cmake .","make"; executables are in "/build" folder

#Upravy: 	1. Nechtela se mi tam nalinkovat libxml2 a nevedel jsem jak za kompilace pridat prikaz `xml2-config --libs --cflags`
#		   Tak jsem ten prikaz spustil mimo cmake a jeho vystup (-I/usr/include/libxml2 -lxml2 -lz -lm -ldl) jsem pridal do cflags
#		   Funguje to ale pokud bys mela lepsi reseni klido sem s nim :)
#		2. Potreboval jsem dalsi cast boost knihovny tak jsem pridal -lboost_program_options

#Used compile parameters:
#for libxml:	-I/usr/include/libxml2 -lxml2 -lz -lm -ldl
#for glib.h:	-I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/
#for librepo.h:	-L/usr/include/librepo/ -lrepo

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(ssds)
SET(ssds_VERSION_MAJOR 0)
SET(ssds_VERSION_MINOR 0)
SET(ssds_VERSION_BUILD 1)
SET(ssds_VERSION
   	"${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")
SET(PACKAGE_VERSION
	"${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")


include_directories ("${PROJECT_SOURCE_DIR}/src/common/")
include_directories ("${PROJECT_SOURCE_DIR}/src/client/")
include_directories ("${PROJECT_SOURCE_DIR}/src/server/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

#BOOST
find_package(Boost REQUIRED thread system filesystem)
if (NOT Boost_FOUND)
	message(FATAL_ERROR "Library Boost required, but not found!")
else(NOT Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif(NOT Boost_FOUND)

#LIBXML++
# find_package(LibXML++ REQUIRED)
# if (NOT LIBXML++_FOUND)
# 	message(FATAL_ERROR "Library LibXML++ required, but not found!")
# else(NOT LIBXMLPP_FOUND)
# 	include_directories(${LIBXML++_INCLUDE_DIRS})
# 	#include_directories( "${LIBXMLPP_SOURCE_DIR}/include" "${CMAKE_INSTALL_PREFIX}/include")
# 	#LINK_DIRECTORIES("${CMAKE_INSTALL_PREFIX}/lib")
# 	set(LIBS ${LIBS} ${LIBXML++_LIBRARIES})
# endif(NOT LIBXML++_FOUND)

if(CMAKE_COMPILER_IS_GNUCXX) 
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0 -I/usr/include/libxml2/ -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ -L/usr/include/librepo/ -lrepo -lxml2 -lz -lm -ldl -lboost_program_options -lpthread -lhawkey -lglib-2.0")
#original flags for boost lib:  -I/usr/local/include -L/usr/local/lib
    	set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.cpp *.hpp)

PROJECT(ssds-server)
set(SERVSRC "../src/server/server_main.cpp")
add_executable(ssds-server ${SERVSRC}
		../src/common/logger.cpp
		../src/common/xml_debug.cpp
		../src/common/read_xml.cpp
		../src/common/create_xml.cpp
		../src/common/repo_metadata.cpp
		../src/common/repo_handler.cpp
		../src/common/solving.cpp
		../src/server/server.cpp)

target_link_libraries(
	ssds-server
	${Boost_SYSTEM_LIBRARY} #or ${Boost_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${LIBS}
)

PROJECT(ssds-client)
set(CLISRC "../src/client/client_main.cpp")
#add_library("../src/client/client.cpp")
add_executable(ssds-client ${CLISRC}
		../src/common/logger.cpp
		../src/common/repo_handler.cpp
		../src/common/repo_metadata.cpp
		../src/common/xml_debug.cpp
		../src/common/params.cpp
		../src/common/create_xml.cpp
		../src/common/read_xml.cpp)
target_link_libraries(
	ssds-client
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${LIBS}
)
