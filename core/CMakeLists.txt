############################################################################
# CMakeLists.txt for building STeCaCore
############################################################################

add_subdirectory(io)
add_subdirectory(LevMar)

file(GLOB header_files
  "*.h"
  "types/*.h"
  "types/inc/*.h"
)

file(GLOB source_files
  "*.cpp"
  "types/*.cpp"
  "types/inc/*.cpp"
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
)

add_library(STeCaCore
  ${source_files}
  ${header_files}
)

target_link_libraries(STeCaCore
  LevMar
  Qt5::Core
)

set_property(TARGET STeCaCore PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaCore PROPERTY CXX_STANDARD_REQUIRED ON)
