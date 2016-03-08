############################################################################
# CMakeLists.txt for building LevMar
############################################################################

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options("-fpermissive")
endif()

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
