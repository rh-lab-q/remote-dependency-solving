FIND_PACKAGE(Boost COMPONENTS filesystem system REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
add_executable(rds-client client.c)
target_link_libraries(
    rds-client
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)
