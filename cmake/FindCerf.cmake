# Find libcerf or libcerfcpp
#
# Usage:
#   find_package(Cerf [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - Cerf_FOUND        .. true if library is found
#   - Cerf_LIBRARIES    .. full path to library
#   - Cerf_INCLUDE_DIR  .. full path to include directory
#   - Cerf_IS_CPP       .. true if C++ version of library is used
#
# Copyright 2018 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(Cerf_INCLUDE_DIR cerf.h)
find_library(Cerf_LIBRARIES NAMES cerf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cerf DEFAULT_MSG Cerf_LIBRARIES Cerf_INCLUDE_DIR)

if(NOT Cerf_FOUND)
    unset(Cerf_FOUND)
    unset(Cerf_LIBRARIES)
    find_library(Cerf_LIBRARIES NAMES cerfcpp)
    find_package_handle_standard_args(Cerf DEFAULT_MSG Cerf_LIBRARIES Cerf_INCLUDE_DIR)
    if(NOT Cerf_FOUND)
        message(STATUS "Found neither libcerf nor libcerfcpp")
        return()
    else()
        message(STATUS "Found libcerfcpp")
        set(Cerf_IS_CPP ON)
    endif()
else()
   message(STATUS "Found libcerf")
   set(Cerf_IS_CPP OFF)
endif()

mark_as_advanced(Cerf_INCLUDE_DIR Cerf_LIBRARIES)

include(AssertLibraryFunction)
assert_library_function(Cerf cerf "")
assert_library_function(Cerf dawson "")
assert_library_function(Cerf voigt "")
