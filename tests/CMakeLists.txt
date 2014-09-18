#CMAKE BUILD 1.0
#Usage in CLI: "cd build", "cmake ..","make"; executables are in "/build" folder
#NEXT: support of Boost, LibXML++ in different platforms

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(server)

include_directories ("../${PROJECT_SOURCE_DIR}/src/common/")
include_directories ("../${PROJECT_SOURCE_DIR}/src/client/")
include_directories ("../${PROJECT_SOURCE_DIR}/src/server/")

if(CMAKE_COMPILER_IS_GNUCXX) 
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c++11  -I/usr/local/include -L/usr/local/lib -lboost_thread -lboost_system -fmessage-length=0")
    	set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.cpp *.hpp)

set(SERVSRC "../src/server/Server.cpp")
ADD_EXECUTABLE(server ${SERVSRC})

PROJECT(client)
set(CLISRC "../src/client/client.cpp")
ADD_EXECUTABLE(client ${CLISRC})
