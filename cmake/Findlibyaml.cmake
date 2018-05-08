# Find libyaml
#
# Usage:
#   find_package(libyaml [REQUIRED] [QUIET])
#
# Sets the following variables:
#   - LIBYAML_FOUND          .. true if library is found
#   - LIBYAML_LIBRARIES      .. full path to library
#   - LIBYAML_INCLUDE_DIR    .. full path to include directory
#
# Honors the following optional variables:
#   - LIBYAML_INCLUDE_LOC    .. include directory path, to be searched before defaults
#   - LIBYAML_LIBRARY_LOC    .. the library's directory path, to be searched before defaults
#   - LIBYAML_STATIC_LIBRARY .. if true, find the static library version
#
# Copyright 2015 Joachim Wuttke, Forschungszentrum Jülich.
# modified 2018 Joachim Coenen, Forschungszentrum Jülich.
# Redistribution permitted.

include (CheckCppIncludeFile)
include (CheckCXXSourceRuns)

# find the libyaml include directory
find_path(LIBYAML_INCLUDE_DIR yaml.h
    PATH_SUFFIXES include libyaml/include libyaml
    PATHS
    ${LIBYAML_INCLUDE_LOC}
    ~/Library/Frameworks/
    /Library/Frameworks/
    /usr/local/
    /usr/
    /sw/ # Fink
    /opt/local/ # DarwinPorts
    /opt/csw/ # Blastwave
    /opt/
    )

set(CMAKE_REQUIRED_INCLUDES ${LIBYAML_INCLUDE_DIR})
set(CMAKE_REQUIRED_QUIET True)

# now look for needed libyaml include files
unset(LIBYAML_FOUND CACHE)
check_include_file("yaml.h" LIBYAML_FOUND)
if(${LIBYAML_FOUND})
else()
    message(FATAL_ERROR "Include file for libyaml not found")
endif()

# attempt to find static library first if this is set
if(LIBYAML_STATIC_LIBRARY)
    set(LIBYAML_STATIC libyaml.a)
endif()

# find the libyaml library
find_library(LIBYAML_LIBRARY
    NAMES ${LIBYAML_STATIC} yaml
    PATH_SUFFIXES lib64 lib
    PATHS
    ${LIBYAML_LIBRARY_LOC}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    )

# try to compile, link, and run a test program
unset(LIBYAML_RUNS CACHE)
#set(CMAKE_REQUIRED_LIBRARIES yaml)
#check_cxx_source_runs("#include <yaml.h>\n#include <assert.h>\nint main() {\n    YAML::Node node = YAML::Load(\"[1, 2, 3]\");\n    assert(node.IsSequence());\n}" LIBYAML_RUNS)
#if(${LIBYAML_RUNS})
#else()
#    message(FATAL_ERROR "Test of libyaml failed")
#endif()

message(STATUS "Found Findlibyaml: ${LIBYAML_LIBRARY}")
mark_as_advanced(LIBYAML_INCLUDE_DIR LIBYAML_LIBRARY)
