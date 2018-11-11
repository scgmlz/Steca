# Steca top-level configuration

cmake_minimum_required(VERSION 3.6 FATAL_ERROR) # 3.6+ required by IFW for VS2015+
cmake_policy(SET CMP0048 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

project(Steca VERSION 2.1)
message(STATUS "Building Steca ${Steca_VERSION}")

set(CMAKE_PROJECT_DESCRIPTION # for display in installer
    "Steca - the stress and texture calculator.")

option(WITH_TESTS "Build with all tests" ON)
option(BUILD_FALLBACK "Build and use the fallback 3rdparty libraries" OFF)

set(LIB_MAN     OFF CACHE INTERNAL "" FORCE)
set(LIB_INSTALL OFF CACHE INTERNAL "" FORCE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__ -DQT_LOGGING_RULES=\"*.debug=true\"")

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
    set(MS_NOWARN "/wd4018 /wd4068 /wd4101 /wd4244 /wd4267 /wd4305 /wd4715 /wd4996")
    set(MS_D "-D_CRT_SECURE_NO_WARNINGS -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${MS_NOWARN} ${MS_D}")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    set(CTEST_CONFIGURATION_TYPE "${JOB_BUILD_CONFIGURATION}")
else()
    # -fsanitize=address -fsanitize=undefined
    set(DEB_FLAGS "-Wall -Wno-sign-compare -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${DEB_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -w")
    set(BUILD_SHARED_LIBS ON)
endif()

foreach(comp Qt5Core Qt5Gui Qt5Widgets Qt5Network)
    find_package(${comp} REQUIRED)
    message(STATUS "found ${comp}: version=${${comp}_VERSION_STRING} dir=${${comp}_DIR} lib=${${comp}_LIBRARIES}")
endforeach()

find_package(YAML      MODULE)
find_package(LMFit 1.8 MODULE)
find_package(Cerf  1.8 MODULE)

message(STATUS
    "libyaml: FOUND=${yaml_FOUND}, VERSION=${yaml_VERSION}, LIB=${yaml_LIBRARIES}")
message(STATUS "lmfit:   FOUND=${LMFit_FOUND}, VERSION=${LMFit_VERSION}, LIB=${LMFit_LIBRARIES}")
message(STATUS "libcerf: FOUND=${Cerf_FOUND}, VERSION=${Cerf_VERSION}, LIB=${Cerf_LIBRARIES}, "
    "IS_CPP=${Cerf_IS_CPP}")

# how to build 3rd party libraries:
set(LIB_MAN OFF)
set(LIB_INSTALL OFF)

# build 3rd party libraries:
add_subdirectory(3rdparty/LevMar)
add_subdirectory(3rdparty/QCustomPlot)
add_subdirectory(3rdparty/Caress)
add_subdirectory(3rdparty/Mar)

if(NOT yaml_FOUND OR BUILD_FALLBACK)
    message(STATUS "we have to build 3rdparty/libyaml")
    add_subdirectory(3rdparty/libyaml)
endif()
if(NOT LMFit_FOUND OR BUILD_FALLBACK)
    message(STATUS "we have to build 3rdparty/lmfit")
    add_subdirectory(3rdparty/lmfit)
endif()
if(NOT Cerf_FOUND OR BUILD_FALLBACK)
    message(STATUS "we have to build 3rdparty/libcerf")
    set(CERF_CPP ON CACHE INTERNAL "" FORCE)
    add_subdirectory(3rdparty/libcerf)
    set(Cerf_IS_CPP ON)
endif()

if(Cerf_IS_CPP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCERF_AS_CPP")
endif()

add_subdirectory(qcr)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(main)

if(WITH_TESTS)
    include(CTest) # equivalent to "enable_testing() ???
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}) # => 'make check' is alias for 'ctest'
    set(gtest_force_shared_crt ON
        CACHE bool "Use shared(DLL) run-time lib even when Google Test is built as static lib.")
    add_subdirectory(3rdparty/gtest)
    configure_file(testdata/testdata.h.in ${CMAKE_BINARY_DIR}/testdata.h @ONLY)
    add_subdirectory(utest)
else()
    message(STATUS "Skipping tests")
endif()
