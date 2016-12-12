# tests

set (srcdirs
  lib LevMar core io
)

foreach (dir ${srcdirs})
  set (path ${CMAKE_HOME_DIRECTORY}/${dir})
  include_directories (${path})

  file (GLOB_RECURSE fs ${path}/*.cpp)
  set (files ${files} ${fs})
endforeach (dir)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions (-DTESTS)

add_executable (tests
  tests_main.cpp
  ${files}
)

target_link_libraries (tests
  Qt5::Test
  Qt5::Widgets
)

# eof
