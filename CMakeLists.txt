# Steca top-level configuration

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_MODULE_PATH 
  ${CMAKE_SOURCE_DIR}/cmake
  ${CMAKE_SOURCE_DIR}/cmake/steca/modules)
include(PreventInSourceBuilds)

project(Steca)

option(WITH_TESTS "Build with all tests" ON)

if(CMAKE_VERSION VERSION_LESS "3.1")
  set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O1 -g -fsanitize=address -fno-omit-frame-pointer -DQT_LOGGING_RULES=\"*.debug=true\"")
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DQT_LOGGING_RULES=\"*.debug=true\"")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /wd4267 /wd4305 /wd4244 /wd4068 /wd4996 /wd4101 /wd4715 -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING -DQT_LOGGING_RULES=\"*.debug=false\"")
else()
  # -fsanitize=address
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wno-sign-compare -fsanitize=undefined -fno-omit-frame-pointer -DQT_LOGGING_RULES=\"*.debug=true\"")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -w -DQT_LOGGING_RULES=\"*.debug=false\"")
endif()


find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui     REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(LibYaml    REQUIRED)
message(STATUS "found Qt5Core=${Qt5Core_VERSION_STRING}, Qt5Gui=${Qt5Gui_VERSION_STRING}")
message(STATUS "Qt5Core=${Qt5Core_DIR}, Qt5=${Qt5Gui_DIR}")

add_subdirectory(3rdparty/LevMar)
add_subdirectory(3rdparty/QCustomPlot)
add_subdirectory(3rdparty/Caress)
add_subdirectory(3rdparty/Mar)
add_subdirectory(qcr)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(main)

if(WITH_TESTS)
  include(CTest) # equivalent to "enable_testing() ???
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}) # => 'make check' is an alias for 'ctest'
  set(gtest_force_shared_crt ON
      CACHE bool "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
  add_subdirectory(3rdparty/gtest)
  configure_file(testdata/testdata.h.in ${CMAKE_BINARY_DIR}/testdata.h @ONLY)
  add_subdirectory(utest)
endif()
