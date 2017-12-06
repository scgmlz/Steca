add_definitions (-DTESTS)

set (srcdirs
  lib core io 3rdparty/doctest
)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/3rdparty/LevMar
)

foreach (dir ${srcdirs})
  include_directories (${CMAKE_HOME_DIRECTORY}/${dir})
endforeach (dir)

foreach (dir ${srcdirs})
  file (GLOB_RECURSE fs ${CMAKE_HOME_DIRECTORY}/${dir}/*.cpp)
  set (files ${files} ${fs})
endforeach (dir)

file (GLOB_RECURSE test_files *.h *.cpp)

add_executable (tests
  ${test_files}
  ${files}
)

target_link_libraries (tests
    Qt5::Widgets
    LevMar
)
