# Main CMakeLists.txt to build Server-Side-Dependency solving project using
# CMake (www.cmake.org)
# Written by Petr Hracek

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project (ssds)

SET(ssds_VERSION_MAJOR 0)
SET(ssds_VERSION_MINOR 0)
SET(ssds_VERSION_BUILD 1)
SET(ssds_VERSION
    "${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")
SET(PACKAGE_VERSION
    "${ssds_VERSION_MAJOR}.${ssds_VERSION_MINOR}.${ssds_VERSION_BUILD}")


add_subdirectory (src)
