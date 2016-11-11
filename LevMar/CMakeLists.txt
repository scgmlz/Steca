# CMakeLists.txt

get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_HOME_DIRECTORY}/lib
)

set(source_files
  Axb.cpp
  lmbc.cpp
  lmblec.cpp
  lmbleic.cpp
  lm.cpp
  lmlec.cpp
  misc.cpp
)

add_library(${lib}
  ${source_files}
)

target_link_libraries (${lib}
  Qt5::Core
)

# eof
