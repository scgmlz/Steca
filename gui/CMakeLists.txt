get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/3rdparty/QCustomPlot
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/io
)

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  Qt5::Gui
  Qt5::Widgets
  Qt5::Network
  steca_QCP
  io
  core
  lib
)

# the Steca executable:
add_executable (Steca
  main.cpp
  resources.qrc
  # the following non-dependences are convenient for the qtcreator tree view:
  ../manifest.h
  ../CHANGELOG
  ../README.md
  ../VERSION
)

target_link_libraries (Steca ${lib})
