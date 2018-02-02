# Steca top-level configuration

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

project(Steca)

option(WITH_TESTS "Build with all tests" ON)

if(CMAKE_VERSION VERSION_LESS "3.1")
  set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_LOGGING_RULES=\"*.debug=true\"")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-Wall")
endif()

find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui     REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
message(STATUS "found Qt5Core=${Qt5Core_VERSION_STRING}, Qt5Gui=${Qt5Gui_VERSION_STRING}")

add_subdirectory(3rdparty/LevMar)
add_subdirectory(3rdparty/QCustomPlot)
add_subdirectory(3rdparty/Caress)
add_subdirectory(3rdparty/Mar)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(main)

if(WITH_TESTS)
  include(CTest) # equivalent to "enable_testing() ???
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}) # => 'make check' is an alias for 'ctest'
  set(gtest_force_shared_crt ON
      CACHE bool "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
  add_subdirectory(3rdparty/googletest)
  configure_file(testdata/testdata.h.in ${CMAKE_BINARY_DIR}/testdata.h @ONLY)
  add_subdirectory(utest)
endif()
