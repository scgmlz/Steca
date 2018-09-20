# Find libcerf
#
# Usage:
#   find_package(Cerf [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - Cerf_FOUND        .. true if library is found
#   - Cerf_LIBRARIES    .. full path to library
#   - Cerf_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2015 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(Cerf_INCLUDE_DIR cerf.h)
find_library(Cerf_LIBRARIES NAMES cerf Cerf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cerf DEFAULT_MSG Cerf_LIBRARIES Cerf_INCLUDE_DIR)

include(AssertLibraryFunction)
assert_library_function(Cerf cerf "")
assert_library_function(Cerf dawson "")
assert_library_function(Cerf voigt "")

mark_as_advanced(Cerf_INCLUDE_DIR Cerf_LIBRARIES)
