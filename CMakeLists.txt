#CMAKE BUILD 1.3
#Usage in CLI: "cmake .","make"; executables are in "/build" folder


CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(rds)
SET(rds_VERSION_MAJOR 0)
SET(rds_VERSION_MINOR 2)
SET(rds_VERSION_BUILD 0)
SET(rds_VERSION
   	"${rds_VERSION_MAJOR}.${rds_VERSION_MINOR}.${rds_VERSION_BUILD}")
SET(PACKAGE_VERSION
	"${rds_VERSION_MAJOR}.${rds_VERSION_MINOR}.${rds_VERSION_BUILD}")

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
          string(REGEX MATCH "[0-9]?$" HKY_REGEX_VERSION ${HAWKEY_VERSION})
#			message("Hawkey version " ${HAWKEY_VERSION})
 
			if(${HAWKEY_VERSION} VERSION_GREATER "0.5.3")
	            set(HKY_22 1)
#				message("Hawkey version higher than 0.5.3")         
			else(${HAWKEY_VERSION} VERSION_GREATER "0.5.3")
				set(HKY_22 0)
#				message("Hawkey version lower than 0.5.3")         
#          math(EXPR HKY_NUM "${HKY_REGEX_VERSION}+0")
#          if(${HKY_NUM} GREATER 3)#hawkey version is higher than 0.5.3

#          else(${HKY_NUM} GREATER 3)#hawkey version is lower or equal to 0.5.3
#            set(HKY_22 0)
            endif(${HAWKEY_VERSION} VERSION_GREATER "0.5.3")
          
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
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-O2 -g -Wall -Wextra -pedantic  -std=c11 -fmessage-length=0 -L/usr/include/librepo/ -lrepo -lz -lm -ldl -lpthread -lhawkey -lgobject-2.0 -lrpm -lrpmio -lssl -lcrypto -L/usr/lib")
#original flags for boost lib:  -I/usr/local/include -L/usr/local/lib
  set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

file(GLOB SRCS *.c *.h)

#COMMON
set (rds_common_srcs
	src/common/util.c
	src/common/json_read.c
	src/common/json_create.c
	src/common/repo_metadata.c
	src/common/repo_handler.c
	src/common/solving.c
	src/common/log_handler.c
	src/common/network_util.c
	src/common/params.c
	src/common/packages.c
	src/common/mem_management.c
	src/common/cfg_parsing.c
	src/common/detect_missing_repos.c
	src/common/network_security.c)
set (rds_common_headers
	src/common/includes.h
	src/common/json_handler.h
	src/common/repo_handler.h
	src/common/solving.h
	src/common/log_handler.h
	src/common/network_util.h
	src/common/params.h
	src/common/packages.h
	src/common/mem_management.h
	src/common/cfg_parsing.h
	src/common/errors.h
	src/common/detect_missing_repos.h
	src/common/network_security.h)
add_library(lib_rds_common SHARED ${rds_common_srcs} ${rds_common_headers})
target_link_libraries(lib_rds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

#SERVER
set (rds_server_srcs
	src/server/server_main.c
	src/server/server.c)
set (rds_server_headers
	src/server/server.h)
add_library(lib_rds_server STATIC ${rds_server_srcs} ${rds_server_headers})
# message(${GLIB2_LDFLAGS})
# message(${JSON_LDFLAGS})
target_link_libraries(lib_rds_server
	lib_rds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

#CLIENT
set (rds_client_srcs
	src/client/client_main.c
	src/client/client.c)
set (rds_client_headers
	src/client/client.h)
add_library(lib_rds_client STATIC ${rds_client_srcs} ${rds_client_headers})
target_link_libraries(lib_rds_client
	lib_rds_common
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})



PROJECT(rds-server)
add_executable(rds-server
	${rds_server_srcs}
	${rds_common_srcs})

target_compile_definitions(rds-server PUBLIC __CASE_SERVER)

#add_library(lib_serv SHARED ${SERVSRC})

target_link_libraries(
	rds-server
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

PROJECT(rds-client)
add_executable(rds-client
	${rds_client_srcs}
	${rds_common_srcs})

target_compile_definitions(rds-client PUBLIC __CASE_CLIENT)

target_link_libraries(rds-client
	${LIBS}
	${GLIB2_LDFLAGS}
	${JSON_LDFLAGS}
	${HAWKEY_LDFLAGS}
	${LIBREPO_LDFLAGS}
	${CHECK_LDFLAGS})

file(MAKE_DIRECTORY /tmp/ssds/)
file(MAKE_DIRECTORY /var/cache/ssds/packages/)
enable_testing()
add_subdirectory(tests)
