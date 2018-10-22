# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# PreventInSourceBuilds
# ---------------------
#
# Prevent in-source builds
#
# It is generally acknowledged that it is preferable to run CMake out of source,
# in a dedicated build directory(typically named "build"). To prevent users from
# accidentally running CMake in the source directory, just include this module.

# make sure the user doesn't play dirty with symlinks
get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

# disallow in-source builds
if("${srcdir}" STREQUAL "${bindir}")
    message(FATAL_ERROR "\

CMake must not to be run in the source directory. \
Rather create a dedicated build directory and run CMake there. \
To clean up after this aborted in-place compilation:
  rm -r CMakeCache.txt CMakeFiles
")
endif()
