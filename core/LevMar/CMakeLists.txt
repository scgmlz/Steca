############################################################################
# CMakeLists.txt for building LevMar
############################################################################

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

set_property(TARGET LevMar PROPERTY CXX_STANDARD 11)
set_property(TARGET LevMar PROPERTY CXX_STANDARD_REQUIRED ON)
