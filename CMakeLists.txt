############################################################################
# top-level CMakeLists.txt for building STeCa
############################################################################

# TODO actually cmake 3.1+ is required to handle PROPERTY CXX_STANDARD 11
# but it may not be available
# for cmake < 3.1, add an environment variable:
# CXXFLAGS=-std=c+11 before running cmake

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(STeCa)

# --- The version number ---
set(STeCa_VERSION_MAJOR 2)
set(STeCa_VERSION_MINOR 0)
set(STeCa_VERSION_PATCH 0)

# --- Global settings ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# --- Enable compiler warnings ---
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-Wall")
endif()

# --- Check for required packages ---
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)

# --- Recurse into the given subdirectories ---
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(tests)
