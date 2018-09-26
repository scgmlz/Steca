cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

option(CERF_CPP "Build libcerf as native C++" OFF)
option(LIB_MAN "Build libcerf man pages" ON)
option(LIB_INSTALL "Install libcerf" ON)

if (${CERF_CPP})
    project(cerfcpp CXX)
else()
    project(cerf C)
endif()

set(CERF_SOVERSION                 1) # API version
set(CERF_VERSION ${CERF_SOVERSION}.7) # minor version

set(destination ${CMAKE_INSTALL_PREFIX})

set(CERF_COMPILER_FLAGS "-g -O2 -pedantic -Wall -Wno-sign-compare -Wno-unused-result -Wno-parentheses -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CERF_COMPILER_FLAGS} -DCERF_AS_CPP")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${CERF_COMPILER_FLAGS}")
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib)
add_subdirectory(test)
if (${LIB_MAN})
    add_subdirectory(man)
endif()
