include_directories (
  ${CMAKE_HOME_DIRECTORY}
  ${CMAKE_HOME_DIRECTORY}/3rdparty
)

add_executable (Steca steca.cpp resources.qrc)

target_link_libraries (Steca gui)
