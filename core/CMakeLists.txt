############################################################################
# CMakeLists.txt for building STeCaCore
############################################################################

add_subdirectory(io)
add_subdirectory(LevMar)

set(source_files
  core_array2d.cpp
  core_dataset.cpp
  core_defs.cpp
  core_file.cpp
  core_fit_fitting.cpp
  core_fit_functions.cpp
  core_fit_methods.cpp
  core_image.cpp
  core_image_cut.cpp
  core_image_transform.cpp
  core_lens.cpp
  core_reflection.cpp
  core_session.cpp
  core_types.cpp
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
)

add_library(STeCaCore
  ${source_files}
)

target_link_libraries(STeCaCore
  LevMar
  Qt5::Core
)

set_property(TARGET STeCaCore PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaCore PROPERTY CXX_STANDARD_REQUIRED ON)
