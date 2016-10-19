# CMakeLists.txt

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/io
)

file (GLOB header_files
  "*.h" "*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp"
)

add_library (${lib}
  ${header_files}
  ${source_files}
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
