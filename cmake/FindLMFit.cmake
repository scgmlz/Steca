# Find liblmfit
#
# Usage:
#   find_package(LMFit [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - LMFit_FOUND        .. true if library is found
#   - LMFit_LIBRARIES    .. full path to library
#   - LMFit_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2015-2018 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(LMFit_INCLUDE_DIR lmmin.h)
find_library(LMFit_LIBRARIES NAMES lmfit LMFit)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LMFit DEFAULT_MSG LMFit_LIBRARIES LMFit_INCLUDE_DIR)
mark_as_advanced(LMFit_INCLUDE_DIR LMFit_LIBRARIES)

if(NOT LMFit_FOUND)
    return()
endif()
include(AssertLibraryFunction)
assert_library_function(LMFit lmmin "")
