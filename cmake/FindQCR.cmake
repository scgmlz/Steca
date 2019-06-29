# Find libQCR
#
# Usage:
#   find_package(QCR [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - QCR_FOUND        .. true if library is found
#   - QCR_LIBRARIES    .. full path to library
#   - QCR_INCLUDE_DIR  .. full path to include directory
#
# Copyright 2019 Joachim Wuttke, Forschungszentrum Jülich.
# Redistribution permitted.

find_path(QCR_INCLUDE_DIR QCR/widgets/mainwindow.h)
find_library(QCR_LIBRARIES NAMES QCR QCR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QCR DEFAULT_MSG QCR_LIBRARIES QCR_INCLUDE_DIR)
mark_as_advanced(QCR_INCLUDE_DIR QCR_LIBRARIES)
