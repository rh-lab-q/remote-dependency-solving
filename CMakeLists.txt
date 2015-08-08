#CMAKE BUILD 1.3
#Usage in CLI: "cmake .","make"; executables are in "/build" folder


CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

#pkg-config is needed to find and properly link some libraries
FIND_PACKAGE(PkgConfig)
INCLUDE(FindPkgConfig)

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
        message(FATAL_ERROR "Library json-glib-devel not found.")
else (NOT JSON_FOUND)
	include_directories(${JSON_INCLUDE_DIRS})
endif(NOT JSON_FOUND)

#HAWKEY - 0.5.7
PKG_CHECK_MODULES(HAWKEY hawkey REQUIRED)
if(NOT HAWKEY_FOUND)
	message(FATAL_ERROR "Library hawkey not found.")
else (NOT HAWKEY_FOUND)
          string(REGEX MATCH "[0-9]?$" HKY_REGEX ${HAWKEY_VERSION})
          
          math(EXPR HKY_NUM "${HKY_REGEX}+0")
          if(${HKY_NUM} GREATER 3)#hawkey version is higher than 0.5.3
            set(HKY_22 1)
          else(${HKY_NUM} GREATER 3)#hawkey version is lower or equal to 0.5.3
            set(HKY_22 0)
          endif(${HKY_NUM} GREATER 3)
          
          add_definitions(-DVERSION_HAWKEY=${HKY_22})
	include_directories(${HAWKEY_INCLUDE_DIRS})
endif(NOT HAWKEY_FOUND)

#LIBREPO
PKG_CHECK_MODULES(LIBREPO librepo REQUIRED)
if(NOT LIBREPO_FOUND)
	message(FATAL_ERROR "Library librepo not found.")
else (NOT LIBREPO_FOUND)
	include_directories(${LIBREPO_INCLUDE_DIRS})
endif(NOT LIBREPO_FOUND)

#CHECK
find_package(Check REQUIRED)
include_directories(${CHECKLIB_INCLUDE_DIRS})

#HAWKEY VERSION
configure_file(${PROJECT_SOURCE_DIR}/cmake/Config/version_conf.h.in ${PROJECT_SOURCE_DIR}/cmake/Config/version_conf.h)
include_directories(${PROJECT_SOURCE_DIR}/cmake/Config)

if(CMAKE_COMPILER_IS_GNUCC) 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c11 -fmessage-length=0 -L/usr/include/librepo/ -lrepo -lz -lm -ldl -lpthread -lhawkey -lgobject-2.0")
#original flags for boost lib:  -I/usr/local/include -L/usr/local/lib
  set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.c *.h)

#COMMON
set (ssds_common_srcs
	src/common/util.c
	src/common/json_read.c
	src/common/json_create.c
	src/common/repo_metadata.c
	src/common/repo_handler.c
	src/common/solving.c
	src/common/log_handler.c
	src/common/network_util.c
	src/common/params.c
	src/common/mem_management.c
	src/common/cfg_parsing.c)
set (ssds_common_headers
	src/common/includes.h
	src/common/json_handler.h
	src/common/repo_handler.h
	src/common/solving.h
	src/common/log_handler.h
	src/common/network_util.h
	src/common/params.h
	src/common/mem_management.h
	src/common/cfg_parsing.h
	src/common/errors.h)
add_library(lib_ssds_common SHARED ${ssds_common_srcs} ${ssds_common_headers})
target_link_libraries(lib_ssds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

#SERVER
set (ssds_server_srcs
	src/server/server_main.c)
set (ssds_server_headers
	src/server/server.h)
add_library(lib_ssds_server STATIC ${ssds_server_srcs} ${ssds_server_headers})
target_link_libraries(lib_ssds_server
	lib_ssds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

#CLIENT
set (ssds_client_srcs
	src/client/client_main.c)
set (ssds_client_headers
	src/client/client.h)
add_library(lib_ssds_client STATIC ${ssds_client_srcs} ${ssds_client_headers})
target_link_libraries(lib_ssds_client
	lib_ssds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})



PROJECT(ssds-server)
add_executable(ssds-server
	${ssds_server_srcs}
	${ssds_common_srcs})

#add_library(lib_serv SHARED ${SERVSRC})

target_link_libraries(
	ssds-server
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

PROJECT(ssds-client)
add_executable(ssds-client
	${ssds_client_srcs}
	${ssds_common_srcs})

target_link_libraries(ssds-client
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

file(MAKE_DIRECTORY /tmp/ssds/)
file(MAKE_DIRECTORY /var/cache/ssds/packages/install/)
file(MAKE_DIRECTORY /var/cache/ssds/packages/update/)
enable_testing()
add_subdirectory(tests)
