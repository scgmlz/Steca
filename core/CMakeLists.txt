# CMakeLists.txt

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/LevMar
)

file (GLOB header_files
  "*.h" "*/*.h"
)

file (GLOB source_files
  "*.cpp" "*/*.cpp"
)

add_library (${lib}
  ${source_files}
  ${header_files}
)

target_link_libraries (${lib}
  Qt5::Core
)

#eof
