cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(cerf)
set(cerf_SOVERSION                 1) # API version
set(cerf_VERSION ${cerf_SOVERSION}.6) # minor version

# --- Declare project-wide user flags, and set default values ---
option(FITTEST "Build with FitTest" OFF)

set(destination ${CMAKE_INSTALL_PREFIX})

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -pedantic -Wall -Wno-sign-compare -Wno-unused-result -Wno-parentheses  -Wno-unknown-pragmas")

add_subdirectory(lib)
add_subdirectory(test)
#{Steca patch} add_subdirectory(man)
