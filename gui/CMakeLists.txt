############################################################################
# CMakeLists.txt for building STeCaGUI
############################################################################

file(GLOB header_files
  "*.h"
  "panel/*.h"
  "QCP/*.h"
)

file(GLOB source_files
  "*.cpp"
  "panel/*.cpp"
  "QCP/*.cpp"
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

add_library(STeCaGUI
  ${header_files}
  ${source_files}
)

target_link_libraries(STeCaGUI
  Qt5::Core
  Qt5::Gui
  Qt5::PrintSupport
  Qt5::Widgets
  STeCaCore
  STeCaCoreIO
)

set_property(TARGET STeCaGUI PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaGUI PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(STeCa
  main.cpp
  resources.qrc
)

target_link_libraries(STeCa
  STeCaGUI
)

set_property(TARGET STeCa PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCa PROPERTY CXX_STANDARD_REQUIRED ON)
