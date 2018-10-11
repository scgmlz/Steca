# Find liblmfit
#
# Usage:
#   find_package(LMFit [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - LMFit_FOUND        .. true if library is found
#   - LMFit_LIBRARY    .. full path to library
#   - LMFit_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2015-2018 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(LMFit_INCLUDE_DIR lmmin.h)
find_library(LMFit_LIBRARY NAMES lmfit LMFit)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LMFit DEFAULT_MSG LMFit_LIBRARY LMFit_INCLUDE_DIR)
mark_as_advanced(LMFit_INCLUDE_DIR LMFit_LIBRARY)

if(NOT LMFit_FOUND)
    return()
endif()
include(AssertLibraryFunction)
assert_library_function(LMFit lmmin "")
