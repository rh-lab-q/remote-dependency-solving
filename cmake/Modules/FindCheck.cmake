include(FindPkgConfig)

#CHECK
PKG_CHECK_MODULES(CHECK check REQUIRED)
if(NOT CHECK_FOUND)
  message(FATAL_ERROR "Library check not found.")
endif(NOT CHECK_FOUND)


