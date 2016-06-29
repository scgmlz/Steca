# tests/CMakeLists.txt

include_directories (
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

file (GLOB header_files
  "*.h" "*/*.h" "*/*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp" "*/*/*.cpp"
)

add_executable (tests
  ${header_files}
  ${source_files}
)

target_link_libraries (tests
  Qt5::Test
  STeCaCore
  LevMar
  STeCaLib
  STeCaGUI
)

# eof
