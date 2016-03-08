############################################################################
# CMakeLists.txt for building STeCa tests
############################################################################

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

set(source_files
  test_approx.cpp
  test_core_array2d.cpp
  test_core_image.cpp
  test_core_lib.cpp
  test_core_range.cpp
  test_core_session.cpp
  test_save_load_json.cpp
  tests_main.cpp
)

add_executable(tests
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
