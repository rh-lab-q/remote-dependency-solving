#CMAKE BUILD 1.1
#Usage in CLI: "cd build", "cmake ..","make"; executables are in "/build" folder
#NEXT: support of LibXML++ in different platforms

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


include_directories ("../${PROJECT_SOURCE_DIR}/src/common/")
include_directories ("../${PROJECT_SOURCE_DIR}/src/client/")
include_directories ("../${PROJECT_SOURCE_DIR}/src/server/")

find_package(Boost REQUIRED thread system filesystem)
include_directories(${Boost_INCLUDE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX) 
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c++11 -lboost_thread -lboost_system -fmessage-length=0")
#original flags for boost lib:  -I/usr/local/include -L/usr/local/lib
    	set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.cpp *.hpp)

PROJECT(ssds-server)
set(SERVSRC "../src/server/Server.cpp")
add_executable(ssds-server ${SERVSRC})
target_link_libraries(
    ssds-server
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

PROJECT(ssds-client)
set(CLISRC "../src/client/client.cpp")
add_executable(ssds-client ${CLISRC})
target_link_libraries(
    ssds-client
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)
