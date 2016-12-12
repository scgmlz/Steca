# Steca gui - main app

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

include_directories (
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/io
)

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  Qt5::Core
)

target_link_libraries (${lib}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Multimedia
  core
  LevMar
  io
  lib
)

add_executable (Steca
  main.cpp
  resources.qrc
)

target_link_libraries (Steca
  ${lib}
)

# eof
