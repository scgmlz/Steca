# /CMakeLists.txt

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

project (Steca2)

if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CMAKE_INCLUDE_CURRENT_DIR OFF)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-Wall")
endif ()

find_package (Qt5Core       REQUIRED)
find_package (Qt5Gui        REQUIRED)
find_package (Qt5Test       REQUIRED)
find_package (Qt5Widgets    REQUIRED)
find_package (Qt5Multimedia REQUIRED)

add_subdirectory (lib)
add_subdirectory (LevMar)
add_subdirectory (core)
add_subdirectory (gui)
add_subdirectory (tests)

# eof
