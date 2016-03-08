############################################################################
# CMakeLists.txt for building STeCaCoreIO
############################################################################

set(source_files
  core_io.cpp
  core_io_caress.cpp
  core_io_tiff.cpp
)

set(caress_source_files
  Caress/ReadCaress.cpp
  Caress/raw.cpp
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
)

add_library(STeCaCoreIO
  ${source_files}
  ${caress_source_files}
)

target_link_libraries(STeCaCoreIO
  Qt5::Core
  Qt5::Gui
)

set_property(TARGET STeCaCoreIO PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaCoreIO PROPERTY CXX_STANDARD_REQUIRED ON)
