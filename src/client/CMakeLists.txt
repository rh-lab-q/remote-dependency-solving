FIND_PACKAGE(Boost COMPONENTS filesystem system REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
add_executable(ssds-client client.cpp)
target_link_libraries(
    ssds-client
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)
