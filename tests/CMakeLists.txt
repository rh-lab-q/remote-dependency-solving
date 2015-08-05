set (SSDS_TEST_FILES
	test_main.c
	test_dummy.c
	test_template.c
	test_cfg_parsing.c
	test_params.c
	)

add_executable(test_main ${SSDS_TEST_FILES})

find_package(Check REQUIRED)
#set(LIBS ${LIBS} ${CHECK_LIBRARIES})
#include_directories(${CHECK_INCLUDE_DIRS})
#target_link_libraries(${CHECK_LDFLAGS})
include_directories(. ../src ${CHECK_INCLUDE_DIRS})

target_link_libraries(test_main
	lib_ssds_client
	lib_ssds_server
	lib_ssds_common
	${CHECK_LDFLAGS})

add_test(test_main ${CMAKE_CURRENT_BINARY_DIR}/../tests/test_main)
