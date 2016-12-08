# CMakeLists.txt

set(dir "tests")
set(lib ${dir})

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
  ${CMAKE_HOME_DIRECTORY}/io
)

file (GLOB header_files
  "*.h" "*/*.h" "*/*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp" "*/*/*.cpp"
)

add_executable (${lib}
  ${header_files}
  ${source_files}
)

target_link_libraries (${lib}
  Qt5::Test
  core
  LevMar
  lib
  gui
  io
)

# eof
