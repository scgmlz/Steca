# LevMar/CMakeLists.txt

include_directories (
  ${CMAKE_HOME_DIRECTORY}/lib
  ${CMAKE_HOME_DIRECTORY}/core
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

add_library(LevMar
  ${source_files}
)

target_link_libraries (LevMar
  Qt5::Core
)

# eof
