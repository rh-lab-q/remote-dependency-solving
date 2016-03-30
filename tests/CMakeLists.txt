set (RDS_TEST_FILES
	test_main.c
	test_dummy.c
	test_template.c
	test_cfg_parsing.c
	test_params.c
	#test_gc.c
	test_missing_repos.c
	)

add_executable(test_main ${RDS_TEST_FILES})

find_package(Check REQUIRED)
#set(LIBS ${LIBS} ${CHECK_LIBRARIES})
#include_directories(${CHECK_INCLUDE_DIRS})
#target_link_libraries(${CHECK_LDFLAGS})
include_directories(. ../src ${CHECK_INCLUDE_DIRS})

target_link_libraries(test_main
        lib_rds_client
        lib_rds_server
        lib_rds_common
	${CHECK_LDFLAGS})

add_test(test_main ${CMAKE_CURRENT_BINARY_DIR}/../tests/test_main)
