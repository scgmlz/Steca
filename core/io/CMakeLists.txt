############################################################################
# CMakeLists.txt for building STeCaCoreIO
############################################################################

file(GLOB header_files
  "*.h"
)

file(GLOB source_files
  "*.cpp"
)

# TODO globbing
set(caress_header_files
  Caress/car_datatypes.h
  Caress/car_globals.h
  Caress/raw.h
 )

set(caress_source_files
  Caress/raw.cpp
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
)

add_library(STeCaCoreIO
  ${header_files}
  ${source_files}
  ${caress_header_files}
  ${caress_source_files}
)

target_link_libraries(STeCaCoreIO
  Qt5::Core
  Qt5::Gui
)

set_property(TARGET STeCaCoreIO PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaCoreIO PROPERTY CXX_STANDARD_REQUIRED ON)
