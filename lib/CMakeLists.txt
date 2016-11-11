# CMakeLists.txt

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
)

file (GLOB header_files
  "*/*.h"
)

file (GLOB source_files
  "*/*.cpp"
)

add_library (${lib}
  ${source_files}
  ${header_files}
)

target_link_libraries (${lib}
  Qt5::Core
)

# eof
