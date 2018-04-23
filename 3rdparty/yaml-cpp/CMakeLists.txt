set(lib yaml_cpp)
#get_filename_component(lib ${CMAKE_CURRENT_SOURCE_DIR} NAME)

include_directories (
  ${CMAKE_HOME_DIRECTORY}
  ${CMAKE_HOME_DIRECTORY}/3rdparty
  ${CMAKE_HOME_DIRECTORY}/3rdparty/yaml-cpp/
)

file (GLOB_RECURSE files *.h *.cpp)
add_library (${lib} ${files})

target_link_libraries (${lib}
  core
  Qt5::Network
  steca_QCP
)
