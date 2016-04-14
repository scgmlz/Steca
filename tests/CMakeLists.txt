############################################################################
# CMakeLists.txt for building STeCa tests
############################################################################

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

file(GLOB header_files
  "*.h"
)

file(GLOB source_files
  "*.cpp"
)

add_executable(tests
  ${header_files}
  ${source_files}
)

target_link_libraries(tests
  Qt5::Test
  STeCaCore
  STeCaCoreIO
  STeCaGUI
)

set_property(TARGET tests PROPERTY CXX_STANDARD 11)
set_property(TARGET tests PROPERTY CXX_STANDARD_REQUIRED ON)
