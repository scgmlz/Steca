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

add_library (STeCaGUI
  ${header_files}
  ${source_files}
)

target_link_libraries (STeCaGUI
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Multimedia
  STeCaCore
  LevMar
  STeCaLib
)

add_executable (STeCa
  main.cpp
  resources.qrc
)

target_link_libraries (STeCa
  STeCaGUI
)

install (TARGETS STeCa RUNTIME DESTINATION bin)

# eof
