#CMAKE BUILD 1.3
#Usage in CLI: "cmake .","make"; executables are in "/build" folder

INCLUDE(FindPkgConfig)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(ssds)
SET(ssds_VERSION_MAJOR 0)
SET(ssds_VERSION_MINOR 2)
SET(ssds_VERSION_BUILD 0)
SET(ssds_VERSION
   	"${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")
SET(PACKAGE_VERSION
	"${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")

message(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../cmake/Modules/")

#pkg-config is needed to find and properly link some libraries
FIND_PACKAGE(PkgConfig)

#GLIB-2.0
PKG_CHECK_MODULES(GLIB2 glib-2.0 REQUIRED)
if (NOT GLIB2_FOUND)
	message(FATAL_ERROR "Library Glib-2.0 not found.")
else (NOT GLIB2_FOUND)
	include_directories(${GLIB2_INCLUDE_DIRS})
endif(NOT GLIB2_FOUND)

#JSON-GLIB-1.0
PKG_CHECK_MODULES(JSON json-glib-1.0 REQUIRED)
if(NOT JSON_FOUND)
	message(FATAL_ERROR "Library json-glib-1.0 not found.")
else (NOT JSON_FOUND)
	include_directories(${JSON_INCLUDE_DIRS})
endif(NOT JSON_FOUND)

#HAWKEY
PKG_CHECK_MODULES(HAWKEY hawkey  REQUIRED)
if(NOT HAWKEY_FOUND)
	message(FATAL_ERROR "Library hawkey not found.")
else (NOT HAWKEY_FOUND)
	include_directories(${HAWKEY_INCLUDE_DIRS})
endif(NOT HAWKEY_FOUND)

#LIBREPO
PKG_CHECK_MODULES(LIBREPO librepo REQUIRED)
if(NOT LIBREPO_FOUND)
	message(FATAL_ERROR "Library librepo not found.")
else (NOT LIBREPO_FOUND)
	include_directories(${LIBREPO_INCLUDE_DIRS})
endif(NOT LIBREPO_FOUND)

enable_testing()
include_directories(../tests)
#add_subdirectory(../tests)

#LIBREPO
#PKG_CHECK_MODULES(CHECK check REQUIRED)
#if(NOT CHECK_FOUND)
	#message(FATAL_ERROR "Library check not found.")
#else (NOT CHECK_FOUND)
	#include_directories(${CHECK_INCLUDE_DIRS})
#endif(NOT CHECK_FOUND)

if(CMAKE_COMPILER_IS_GNUCC) 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c11 -fmessage-length=0 -L/usr/include/librepo/ -lrepo -lz -lm -ldl -lpthread -lhawkey -lgobject-2.0")
#original flags for boost lib:  -I/usr/local/include -L/usr/local/lib
    	set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.c *.h)

PROJECT(ssds-server)
set(SERVSRC "../src/server/server_main.c")
add_executable(ssds-server ${SERVSRC}
		../src/common/json_read.c
		../src/common/json_create.c
		../src/common/repo_metadata.c
		../src/common/repo_handler.c
		../src/common/solving.c
		../src/common/log_handler.c
		../src/common/network_util.c)

target_link_libraries(
	ssds-server
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	#${CHECK_LDFLAGS}
)

PROJECT(ssds-client)
set(CLISRC "../src/client/client_main.c")
add_executable(ssds-client ${CLISRC}
		../src/common/json_read.c
		../src/common/json_create.c
		../src/common/repo_handler.c
		../src/common/log_handler.c
		../src/common/network_util.c
		../src/common/params.c)
target_link_libraries(
	ssds-client
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	#${CHECK_LDFLAGS}
)

file(MAKE_DIRECTORY /tmp/ssds/)
file(MAKE_DIRECTORY /var/cache/ssds/packages/)
