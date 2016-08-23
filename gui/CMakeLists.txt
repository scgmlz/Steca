# gui/CMakeLists.txt

include_directories (
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

file (GLOB header_files
  "*.h" "*/*.h" "*/*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp" "*/*/*.cpp"
)

add_library (StecaGUI
  ${header_files}
  ${source_files}
)

target_link_libraries (StecaGUI
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Multimedia
  StecaCore
  LevMar
  StecaLib
)

add_executable (Steca
  main.cpp
  resources.qrc
)

target_link_libraries (Steca
  StecaGUI
)

install (TARGETS Steca RUNTIME DESTINATION bin)

# eof
